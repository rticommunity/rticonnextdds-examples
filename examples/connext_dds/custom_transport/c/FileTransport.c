/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/
/* $Id: Intra.c,v 1.20 2008/10/22 19:16:46 jim Exp $

 (c) Copyright, Real-Time Innovations, $Date: 2008/10/22 19:16:46 $.
 All rights reserved.
 No duplications, whole or partial, manual or electronic, may be made
 without express written permission.  Any such copies, or
 revisions thereof, must display this notice unaltered.
 This code contains trade secrets of Real-Time Innovations, Inc.

 modification history
------------ -------
18apr2013,gpc  Written
=========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/stat.h"
#include "sys/errno.h"
#include "signal.h"


#include "ndds/osapi/osapi_process.h"
#include "ndds/ndds_c.h"

#include "FileTransport.h"

/* =====================================================================
 *               FILE Transport-Plugin
 *
 * See:
 * http://community.rti.com/rti-doc/500/RTI_Transport_Plugin_5.0.0/doc/html/index.html
 *
 * for general documentation on the RTI Connext DDS Transport Plugin API
 *
 * Overview:
 * --------
 *
 * The FileTransport Plugin is intended as an example of how to use the
 * the transport plugin API.  The emphasis is on simplicity and
 * understandability. There has been no effort to make this a "real"
 * transport with any reasonable performance. Rather the goal was to use
 * simple APIs for communications that would not distract from the APIs
 * required to interface with the Connext DDS Core.
 *
 * The transport is limited to communicate between computers that have
 * access to a common file-system.
 *
 * The transport was implemented for Unix systems (Linux, MacOSX) in that
 * it uses some Unix system calls to do things like lock files and read/write
 * bytes to the file. It should be possible to port to other operating systems
 * as long as they offer similar functionality.
 *
 * Design:
 * ------
 *
 * The FileTransport uses simple files for communication. Each address/port
 * combination maps to a separate file placed in an agreed location in the
 * file-system. To send to an address/port the transport opens the file and
 * appends to the end. To receive, the transport opens the file and reads
 * whatever was written beyond the position previously read.
 *
 * The access to the files is serialized using file locking. So that multiple
 * applications can send "simultaneously" to the same address/port and the
 * bytes sent are serialized in the corresponding file. The receiver also
 * used file-locking to ensure it does not read from a file that is being
 * concurrently updated.
 *
 * The "receive" operation on the TransportPlugin is required to block until
 * data is available. To keep things simple the FileTransport implements
 * this by polling the file to see if there were changes and if not
 * sleeping 1 second before checking again. This is obviously not efficient
 * but it keep the code simple which is the main goal.
 *
 * The transport can be configured via properties. These properties control
 * the location in the file-system where the files are placed, the network
 * address of the transport, and the tracing verbosity.
 *
 * A single participant can instantiate more then one FileTransports, each
 * should be given a different name and network address.
 *
 * Configuration
 * -------------
 *
 * The simplest way to load and configure the transport is do it using the
 * DomainParticipantQos and moreover specify the DomainParticipantQos using
 * the XML file USER_QOS_PROFILES.xml that is automatically loaded by the
 * application.
 *
 * You can find the USER_QOS_PROFILES.xml used with this example in the
 * same directoryRoot where this source file is.
 *
 * Properties:
 * ----------
 *
 * In this properties the string "myPlugin" represents an arbitrary name that
 * is given in order to name a particular instantiation of the transport plugin
 * such that it can be distinguished from  other instantiations. The choice of name
 * is arbitrary as long as the same name is used consistenty in all the properties
 * that configure a particular transport instance.
 *
 * Property:                                Meaning:
 * ds.transport.load_plugins                Property common to all transport plugins
 *                                          used to indicate there is a new plugin to
 *                                          load and give it a name
 *                                          (in this example we assume the name
 *                                           chosen is "myPlugin" )
 *
 * dds.transport.FILE.myPlugin.library      Defines the name of the library
 *                                          that contains the plugin implementation
 *                                          for the plugin named in the load_plugins
 *                                          property
 *
 * dds.transport.FILE.myPlugin.create_function  Defines the name of the entry-function
 *                                              in the library defined by the previous
 *                                              property. This function must conform to
 *                                              the signature:
 *
 *                          NDDS_Transport_Plugin* TransportPluginCreateFunctionName(
 *                                  NDDS_Transport_Address_t *default_network_address_out,
 *                                  const struct DDS_PropertyQosPolicy *property_in);
 *
 * dds.transport.FILE.myPlugin.address          The network address given to the instance of the
 *                                              transport plugin. Each FILE plugin instance
 *                                              can have a single network address. This is also
 *                                              propagated via discovery and must be used
 *                                              in the NDDS_DISCOVERY_PEERS environment variable.
 *                                              By default: "1.1.1.1"
 *
 * dds.transport.FILE.myPlugin.dirname          The 'home' directoryRoot under which the files used
 *                                              to hold the received messages are written
 *                                              By default:  "/tmp/dds/<address>"
 *
 * ===================================================================== */


#define NDDS_Transport_FILE_DIRROOT   "/tmp/dds"
#define NDDS_Transport_FILE_DIRBASE     NDDS_Transport_FILE_DIRROOT "/FileTransport"

/* Max length of the signature message is placed as the first message to mark/identify
 * how the file was created
 */
#define NDDS_Transport_FILE_SIGNATURE_MAX_LEN (256)

/* The NDDS_Transport_RecvResource represents the state needed to receive
 * information to a given address/port. It is intended to be defined
 * by each transport plugin implementation. In the case of UDP the
 * NDDS_Transport_RecvResource would wrap a receive socket bound to
 * that port.
 *
 * NDDS_Transport_RecvResource_FILE is created by the operation
 * NDDS_Transport_FILE_create_recvresource_rrEA()
 */
struct NDDS_Transport_RecvResource_FILE {
    /* The file associated with the resource. It is kept open in read mode */
    FILE *_file;
    /* The port number associated with the resource */
    int  _portNum;
    int _fileDescriptor;
    int _receiveMessageCount;
};

/* The NDDS_Transport_SendResource represents the state needed to send
 * information to a given address/port. It is intended to be defined
 * by each transport plugin implementation.  In the case of UDP the
 * NDDS_Transport_SendResource would wrap a send socket.
 *
 * NDDS_Transport_SendResource_FILE is created by the operation
 * NDDS_Transport_FILE_create_sendresource_rrEA()
 */
struct NDDS_Transport_SendResource_FILE {
    /* The destination port number */
    int  _portNum;
    /* The destination address */
    NDDS_Transport_Address_t _address;
    /* The file name associated with the resource */
    char _fileName[NDDS_TRANSPORT_FILE_MAX_FILEPATH_LEN];
    int _sendMessageCount;
};

/* The NDDS_Transport_Plugin structure is intended to be implemented in
 * a different way by each transport plugin implementation.
 *
 * NDDS_Transport_Pugin_FILE represents the state of the transport as it
 * it instantiated. It contains any necessary resources that would
 * be required to create send and receive resources.
 *
 * NDDS_Transport_Pugin_FILE must extend the NDDS_Transport_Plugin structure
 * by including it as its fist member with the conventional name "_parent"
 *
 */
struct NDDS_Transport_FILE {
    NDDS_Transport_Plugin         parent;     /* must be first entry! */
    struct NDDS_Transport_FILE_Property_t _property;
    char        *_buffer;
};


/* ================================================================= */
/*               Debugging utilities                                 */
/* ================================================================= */


/* ----------------------------------------------------------------- */
/* Test whether the transport us enabled logging at level 1 */
#define NDDS_Transport_Log1Enabled(self) ( self->_property.trace_level >= 1)
#define NDDS_Transport_Log2Enabled(self) ( self->_property.trace_level >= 2)

/* ----------------------------------------------------------------- */
/* Conditionally Log a message id the transport us enabled logging at level 1 */
#define NDDS_Transport_Log0printf printf

#define NDDS_Transport_Log1print(self, message) \
    if ( NDDS_Transport_Log1Enabled(self) ) printf("%s: %s\n", METHOD_NAME, message);

#define NDDS_Transport_Log2print(self, message) \
    if ( NDDS_Transport_Log2Enabled(self) ) printf("%s: %s\n", METHOD_NAME, message);

/* ----------------------------------------------------------------- */
/* Utility function to print NDDS_Transport_Address_t and NDDS_Transport_Port_t
 */
static
void NDDS_Transport_FILE_print_address(
    const NDDS_Transport_Address_t      *address_in, 
    const NDDS_Transport_Port_t          port_in,
    int verbosity )
{
    unsigned const char *addressBytes = address_in->network_ordered_value;
    if ( port_in != -1 ) {
        NDDS_Transport_Log0printf("port= %d, ", port_in);
    }
    NDDS_Transport_Log0printf("address= %d.%d.%d.%d",
                addressBytes[12], addressBytes[13], addressBytes[14], addressBytes[15]);

    if ( verbosity >= 2 ) {
        char ifaddr[256];

        NDDS_Transport_Address_to_string(address_in, ifaddr, 256);
        NDDS_Transport_Log0printf(" (%s)\n", ifaddr);
    } else {
        NDDS_Transport_Log0printf("\n");
    }
}


/* ----------------------------------------------------------------- */
/* Member function to retrieve the directoryRoot where to put the files used
 * to write the data sent by the transport.
 */
const char *NDDS_Transport_FILE_getdirname(
        const struct NDDS_Transport_FILE *me)
{
    return me->_property.directoryRoot;
}


/* ----------------------------------------------------------------- */
/* Utility function to construct the filename that will be read by
 * the transport when receiving on a port.
 */
void NDDS_Transport_FILE_getfilename_for_port(
        const struct NDDS_Transport_FILE *me,
        char *fname_out,
        int   fname_out_len,
        const NDDS_Transport_Port_t  port_in)
{
    const char *METHOD_NAME="NDDS_Transport_FILE_getfilename_for_port";

    if ( strlen(NDDS_Transport_FILE_getdirname(me)) + 16  >= fname_out_len ) {
        NDDS_Transport_Log0printf("%s: %s\n", METHOD_NAME, "fname_out is not big enough for filename");
    }
    sprintf(fname_out, "%s/%s/%d", NDDS_Transport_FILE_getdirname(me), me->_property.address, port_in);
}

/* ----------------------------------------------------------------- */
/* Utility function to construct the filename that will be read by
 * the transport when receiving on a port.
 */
void NDDS_Transport_FILE_getfilename_for_address_and_port(
        const struct NDDS_Transport_FILE *me,
        char *fname_out,
        int   fname_out_len,
        const NDDS_Transport_Address_t *address_in,
        const NDDS_Transport_Port_t  port_in)
{
    const char *METHOD_NAME="NDDS_Transport_FILE_getfilename_for_port";
    unsigned const char *addrBytes = address_in->network_ordered_value;

    if ( strlen(NDDS_Transport_FILE_getdirname(me)) + 6  >= fname_out_len ) {
        NDDS_Transport_Log0printf("%s: %s\n", METHOD_NAME, "fname_out is not big enough for filename");
    }

    sprintf(fname_out, "%s/%d.%d.%d.%d/%d", NDDS_Transport_FILE_getdirname(me),
            addrBytes[12], addrBytes[13], addrBytes[14], addrBytes[15],
            port_in);
}

/* ----------------------------------------------------------------- */
/* Utility function to create the directoryRoot that will contain the
 * files used by the transport to send/receive date
 */
RTIBool NDDS_Transport_FILE_ensure_directory_exists(const char *path)
{
    const char *METHOD_NAME="NDDS_Transport_FILE_ensure_directory_exists";
    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
    RTIBool returnValue = RTI_TRUE;

    // Set the mask for file creation to allow full configuration of permissions
    mode_t oldmask = umask(~mode);

    if ( mkdir(path, mode) == -1 ) {
        if (errno != EEXIST ) {
            NDDS_Transport_Log0printf("%s: failed to create directory \"%s\" errno= %d (%s)\n",
                    METHOD_NAME, path, errno, strerror(errno));
            returnValue = RTI_FALSE; // fail
        }
    }

    /* restore mask */
    umask(oldmask);
    return returnValue;
}

RTIBool NDDS_Transport_FILE_ensure_receive_directory_exists(
        struct NDDS_Transport_FILE *me)
{
    char directoryPath[NDDS_TRANSPORT_FILE_MAX_FILEPATH_LEN];
    char *ptrToBegin, *ptrToEnd;

    sprintf(directoryPath, "%s/%s", NDDS_Transport_FILE_getdirname(me), me->_property.address);
    ptrToBegin = directoryPath;

    while ( (ptrToEnd = strchr(ptrToBegin+1, '/')) !=  NULL )  {
        *ptrToEnd = '\0';
        if ( !NDDS_Transport_FILE_ensure_directory_exists(directoryPath) ) {
            return RTI_FALSE;
        }
        *ptrToEnd = '/';
        ptrToBegin = ptrToEnd;
    }
    /* One last one in case directoryPath does not end in '/' */
    if ( !NDDS_Transport_FILE_ensure_directory_exists(directoryPath) ) {
        return RTI_FALSE;
    }

    return RTI_TRUE;
}


/* ----------------------------------------------------------------- */
/* When sending a message, the transport writes the corresponding
 * bytes to a file. But it puts a 10-Byte header before to frame the message.
 *
 * This is not strictly required as it would be sufficient to rely
 * on the fact the the messages are RTPS messages and have a well-known
 * structure. However adding this simplifies the visual inspection
 * of the file and makes the transport implementation independent of
 * of the fact that it will be used to send only RTPS messages
 *
 * The 10-byte header introduced around each message is:
 * "\nFILE"<lengthOfMessage>"\n" where <lengthOfMessage> is the length of
 * the message wrapped by the header expressed as a 4-byte integer in the
 * native endianess of the computer that writes the file.
 */
const char *NDDS_Transport_FILE_writeMessageHeaderPREFIX="\nFILE";
const char *NDDS_Transport_FILE_writeMessageHeaderSUFFIX="\n";

/* ----------------------------------------------------------------- */
/* Returns the length of the message header (10 Bytes in our case)
 */
static RTI_INT32 NDDS_Transport_FILE_messageHeaderLength()
{
    return ( strlen(NDDS_Transport_FILE_writeMessageHeaderPREFIX)
             + 4  /* length of message */
             + strlen(NDDS_Transport_FILE_writeMessageHeaderSUFFIX));
}

/* ----------------------------------------------------------------- */
/* Write the (10-Byte) message header to the file
 */
static void NDDS_Transport_FILE_writeMessageHeader(FILE *file, int messageLen)
{
    fwrite(NDDS_Transport_FILE_writeMessageHeaderPREFIX, 1,
            strlen(NDDS_Transport_FILE_writeMessageHeaderPREFIX), file);
    fwrite(&messageLen, 4, 1, file);
    fwrite(NDDS_Transport_FILE_writeMessageHeaderSUFFIX, 1,
            strlen(NDDS_Transport_FILE_writeMessageHeaderSUFFIX), file);
}

/* ----------------------------------------------------------------- */
/* Utility function to read the next message header from the file and
 * return the number of bytes of the message that follow the message header.
 *
 * There are 3 possible return values:
 *    0  ==>  indicates there is no data in the file beyond the last byte read
 *       this is normal. Should not be considered an error
 *
 *    -1 ==> indicates an error occurred. For example bytes were read but
 *       not enough for a complete header or the header signature is wrong
 *
 *    Positive Int ==>  indicates success and the return is the size of the
 *                      message that appears in the file right after the header.
 *
 * The function verifies that the file position is pointing to a valid
 * transport message header. If that is not the case the function returns -1 and leaves
 * the file position unaltered.
 *
 * If the file position is pointing is pointing to a message header then the
 * function returns the number of bytes belonging to the message
 * that follow the message header and leaves the file seek position to
 * the next byte after the message header
 */
static RTI_INT32 NDDS_Transport_FILE_readMessageHeader(FILE *file)
{
    const char *METHOD_NAME="NDDS_Transport_FILE_readMessageHeader";

    char headerBuffer[256];
    char *messageSizePtr;
    int filePos;
    RTI_INT32 headerLength = NDDS_Transport_FILE_messageHeaderLength();
    RTI_INT32 bytesRead;

    if ( sizeof(headerBuffer) < headerLength) {
        NDDS_Transport_Log0printf("%s: header buffer of size %d is too small. It needs to be at least %d bytes\n",
                METHOD_NAME, (int)sizeof(headerBuffer), headerLength);
        return -1;
    }

    filePos = ftell(file);
    if ( filePos == -1 ) {
        NDDS_Transport_Log0printf("%s: failed to get file position\n", METHOD_NAME);
        return -1;
    }

    bytesRead = fread(headerBuffer, 1, headerLength, file);
    if ( bytesRead == 0 ) {
        /* This may be and EOF or an error. We need to distinguish between the two. */
        if ( feof(file) ) {
            /* End of file was reached. It just indicates there is no new data */
            /* Important: Clear the EOF "error" otherwise subsequent reads will fail */
            clearerr(file);
            return 0;
        } else {
            /* This is a real error */
            NDDS_Transport_Log0printf("%s: failed, errno= %d (%s)\n", METHOD_NAME, errno, strerror(errno));
            return -1;
        }
    }

    if ( bytesRead != headerLength ) {
        NDDS_Transport_Log0printf("%s: at file position= %d failed to read %d bytes of header. Got only %d\n",
                METHOD_NAME, (int)filePos, headerLength, bytesRead);

        fseek(file, filePos, SEEK_SET);
        return -1;
    }

    if ( strncmp (NDDS_Transport_FILE_writeMessageHeaderPREFIX, headerBuffer,
            strlen(NDDS_Transport_FILE_writeMessageHeaderPREFIX) ) != 0 ) {
        NDDS_Transport_Log0printf("%s: the message does not contain the transport header "
                "position = %d\n", METHOD_NAME, (int)filePos);

        fseek(file, filePos, SEEK_SET);
        return -1;
    }

    messageSizePtr = ((char *)headerBuffer) + strlen(NDDS_Transport_FILE_writeMessageHeaderPREFIX);

    return *(RTI_INT32 *)messageSizePtr;
}

/* ----------------------------------------------------------------- */
/*
 * This function reads the next message from the file.
 * The FILE position must be pointing to a transport message header.
 * If this is not the case the function will return -1 and leave
 * the file position unaltered
 *
 * On input:
 *    outBuffer - points to an allocated buffer to receive the message body
 *    outBufferLen - contains the size in bytes of outBuffer.
 *
 * On output:
 *    outBuffer - is filled with the message body on success and left
 *                unchanged on failure
 *
 * Return Value:  Indicates weather there was an error and if not
 *                the numbers of bytes copied into the output buffer:
 *    -1 ==> Indicates an error occurred. For example the caller passed
 *           outBuffer==NULL
 *
 *    0  ==> Indicates no data was copied. This can be either a normal
 *           return or an error:
 *           - If *neededBufferSize is also set to zero it indicates
 *             there was no data to read. This is normal
 *           - If *neededBufferSize is not set to zero it indicates that
 *             the provided buffer was too small.
 *
 *    Positive Int  ==> Indicates a message was read successfully.
 *                      The returned value is the number of bytes in teh message
 *                      which are the ones copied to outBuffer.
 * Arguments:
 *  The function verifies that the next message
 *  in the file fits in the provided outBuffer
 *   - If it fits the function :
 *      a) copies the message body into outBuffer
 *         (skipping the transport header)
 *      b) fills the output parameter neededBufferSize with the
 *         number of bytes copied into outBuffer
 *      c) moves the file position to point to the transport header
 *         of the next message
 *      d) returns the number of bytes copied into outBuffer
 *         (which differs from the ones read from the file by size
 *         of the message header)
 *
 *   - If it does not fit, the function:
 *      a) leaves outBuffer unchanged
 *      b) fills the output parameter neededBufferSize with the
 *         number of bytes that would be required to receive the message
 *      c) leaves the file position unchanged
 *      d) returns 0.
 *
 *   - The situation where there is no message in the file can be detected
 *     because the will return 0 with a *neededBufferSize==0
 */
static int NDDS_Transport_FILE_readNextMessage(
        struct NDDS_Transport_FILE *me,
        FILE *file, int portNum,
        char *outBuffer, int outBufferSize, int *messageSize)
{
    const char *METHOD_NAME="NDDS_Transport_FILE_readNextMessage";

    int filePos;
    RTI_INT32 messageLenght;
    RTI_INT32 bytesRead;

    *messageSize = 0;

    if ( outBuffer == NULL ) {
        NDDS_Transport_Log0printf("%s (port %d): error specified outBuffer is NULL\n", METHOD_NAME, portNum);
        return -1;
    }

    filePos = ftell(file);
    if ( filePos == -1 ) {
        NDDS_Transport_Log0printf("%s (port %d): failed to get file position errno=%d (%s)\n",
                METHOD_NAME, portNum, errno, strerror(errno));
        return -1;
    }

    messageLenght = NDDS_Transport_FILE_readMessageHeader(file);
    /* Check for errors */
    if ( messageLenght == -1 )  {
        NDDS_Transport_Log0printf("%s (port %d): failed to read transport message header\n", METHOD_NAME, portNum);
        return -1;
    }

    /* Check for no data */
    if ( messageLenght == 0 )  {
        if ( NDDS_Transport_Log2Enabled(me) ) {
            printf("%s (port %d): no data found\n", METHOD_NAME, portNum);
        }
        return 0;
    }

    /* Check message fits in output buffer */
    if ( outBufferSize < messageLenght )  {
        /* It does not fit. This is an error. Print it and revert file position */
        fseek(file, filePos, SEEK_SET);
        *messageSize = messageLenght;

        NDDS_Transport_Log0printf("%s (port %d): buffer of size %d is too small for message. It needs at least %d\n",
                METHOD_NAME, portNum, outBufferSize, messageLenght);

        return -1;
    }

    /* All is good, Read the message */
    bytesRead = fread(outBuffer, 1, messageLenght, file);
    if ( bytesRead != messageLenght ) {
        /* There was an error. Leave the file position unchanged */
        fseek(file, filePos, SEEK_SET);

        NDDS_Transport_Log0printf("%s (port %d): position= %d, read only %d bytes, expected %d bytes\n",
                METHOD_NAME, portNum, (int)ftell(file), bytesRead, messageLenght);

        return -1;
    }

    /* Success */
    *messageSize = messageLenght;
    return messageLenght;
}

/* ----------------------------------------------------------------- */
/* Utility function to check whether the process that created
 * the receive resource associated with the file is still running
 *
 * The first message in the file contains the process id of the
 * process that created the file as a result of the call to
 * NDDS_Transport_FILE_open_file_for_port() which is called from
 * NDDS_Transport_FILE_create_recvresource_rrEA.
 *
 * This function parses that message to locate the process Id
 * and then checks if the process is still running
 */
RTIBool NDDS_Transport_FILE_check_file_in_active_use(
        struct NDDS_Transport_FILE *me,
        FILE *file, int portNum)
{
    RTI_INT32 readResult;
    char signatureBuffer[NDDS_Transport_FILE_SIGNATURE_MAX_LEN];
    int messageSize;
    const char *processIdStr;
    char *processIdEndStr;
    int processId;
    int killReturn;

    /* By default we return true indicating file is active. This is safest */
    RTIBool result = RTI_TRUE;

    const char *METHOD_NAME="NDDS_Transport_FILE_check_file_in_active_use";

    /* First get the position so we can restore it prior to return */
    int filePos = ftell(file);
    if ( filePos == -1 ) {
        NDDS_Transport_Log0printf("%s (port %d): file=%p, failed to get file position errno=%d (%s)\n",
                METHOD_NAME, portNum, file, errno, strerror(errno));
        return -1;
    }

    if ( fseek(file, 0, SEEK_SET) != 0 ) {
        NDDS_Transport_Log0printf("%s (port %d): failed to set file position to begining errno=%d (%s)\n",
                METHOD_NAME, portNum, errno, strerror(errno));
        return -1;
    }

    readResult = NDDS_Transport_FILE_readNextMessage(
            me, file, portNum,
            signatureBuffer, NDDS_Transport_FILE_SIGNATURE_MAX_LEN, &messageSize);

    /* Since the file exists, it must have at least the first message.
     * if it does not then there is some error.
     */
    if ( readResult <= 0 ) {
        NDDS_Transport_Log0printf("%s: failed to read first message in file\n", METHOD_NAME);
        goto restoreFilePosAndReturn;
    }

    /* Parse the first message to find the processId*/
    processIdStr = strstr(signatureBuffer, "processId=\"");
    if ( processIdStr == NULL ) {
        NDDS_Transport_Log0printf("%s: failed to find \"processId\" in fist message\n", METHOD_NAME);
        NDDS_Transport_Log0printf("..... first message was: %s\n", signatureBuffer);
        goto restoreFilePosAndReturn;
    }

    /* get the PID */
    processIdStr += strlen("processId=\"");
    processId = strtoul(processIdStr, &processIdEndStr, 10);
    if ( *processIdEndStr != '"' ) {
        NDDS_Transport_Log0printf("%s: Value of \"processId\" was not followed by \"\n", METHOD_NAME);
        NDDS_Transport_Log0printf("..... first message was: %s\n", signatureBuffer);
        goto restoreFilePosAndReturn;
    }

    /* Send signal "0" to the process. Which is not really sending a signal but it can be used
     * to test if the process exists.
     * If the process does not exist kill sets errno to ESRCH.
     */
    if ( kill(processId, 0) != 0 ) {
        result = (errno != ESRCH);
    }

    if ( NDDS_Transport_Log1Enabled(me) ) {
         printf("%s: file=%p (port %d) owned by process %d. Process is %s\n",
                 METHOD_NAME, file, portNum, processId,
                 result?"ALIVE":"NOT ALIVE");
    }

  restoreFilePosAndReturn:
    fseek(file, 0, SEEK_SET);
    return result;
}

/* ----------------------------------------------------------------- */
/*
 * Each port maps to a filename. We check if the file already exists
 * if it exists we fail. If it does not exist we create it and succeed.
 */
struct NDDS_Transport_RecvResource_FILE *NDDS_Transport_FILE_open_file_for_port(
    struct NDDS_Transport_FILE *me,
    const NDDS_Transport_Port_t  port_in)
{
    const char *METHOD_NAME="NDDS_Transport_FILE_open_file_for_port";
    char fileName[256];
    char tinfo[NDDS_Transport_FILE_SIGNATURE_MAX_LEN];
    FILE *file;
    struct NDDS_Transport_RecvResource_FILE *recvResourceStruct;
    int tInfoBytes;

    NDDS_Transport_FILE_getfilename_for_port(me, fileName, 256, port_in);
    file = fopen(fileName, "r");

    /* file != NULL indicates the file already exists */
    if ( file != NULL ) {
        if ( NDDS_Transport_Log1Enabled(me) ) {
            printf("%s: file '%s' already exists\n", METHOD_NAME, fileName);
        }
        /* Check if the owing PID is still alive. If not then cleanup the content and
         * reuse it.
         */
        if ( NDDS_Transport_FILE_check_file_in_active_use(me, file, port_in)) {
            if ( NDDS_Transport_Log1Enabled(me) ) {
                printf("%s: file '%s' actively being used\n", METHOD_NAME, fileName);
            }
            fclose(file);
            return NULL;
        }

        /* Else. File exists and not in active. Reuse it */
        if ( NDDS_Transport_Log1Enabled(me) ) {
             printf("%s: file '%s' not actively being used. Reclaiming it.\n", METHOD_NAME, fileName);
        }
        fclose(file);
    }

    /* Either the file did not exist or else the owning process is no longer active
     * either way we open the file (wiping any contents if the file existed) and
     * initialize the to be used by this process
     * */
    file = fopen(fileName, "wb+");
    if ( file == NULL ) {
        /* Failed to create a file that did not exist. This should not happen */
        NDDS_Transport_Log0printf("%s: Error failed to create file '%s' errno= %d (%s)\n",
                METHOD_NAME, fileName, errno, strerror(errno));
        return NULL;
    }

    /* We successfully opened the file. Write a HEADER to mark the file and the owner */
    sprintf(tinfo, "transportClass=\"%d\";transportName=\"%s\";address=\"%s\";portNum=\"%d\";"
            "fileName=\"%s\";processId=\"%d\"",
            NDDS_TRANSPORT_CLASSID_FILE, NDDS_TRANSPORT_FILE_CLASS_NAME,
            me->_property.address, port_in,
            fileName, RTIOsapiProcess_getId());

    tInfoBytes = strlen(tinfo);
    NDDS_Transport_FILE_writeMessageHeader(file, tInfoBytes);
    /* Write the initial message containing file details */
    fprintf(file, "%s", tinfo);


    /* In principle we could reopen file as read-only because from not on we will only
     * fread() from it. However we cannot because we use use lockf() to lock the file
     * and serialize reads and writes to it.  And in some platforms lockf() requires
     * the process have write access to the file
     */
    /* Rewind to the beginning of the file */
    fseek(file, 0, SEEK_SET);

    /* Create the receive resource */
    recvResourceStruct = (struct NDDS_Transport_RecvResource_FILE *)calloc(1, sizeof(*recvResourceStruct));
    recvResourceStruct->_portNum = port_in;
    recvResourceStruct->_file = file;
    recvResourceStruct->_fileDescriptor = fileno(file);

    if ( NDDS_Transport_Log1Enabled(me) ) {
         printf("%s (port %d): initialized file=%p, descriptor=%d, fileName: \"%s\"\n",
                 METHOD_NAME, port_in, recvResourceStruct->_file,
                 recvResourceStruct->_fileDescriptor, fileName);
    }

    return recvResourceStruct;
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Validate properties
*/
RTI_INT32 NDDS_Transport_FILE_Property_verify(
    const struct NDDS_Transport_FILE_Property_t *property )
{
    RTI_INT32 ok = 1;
    const char *const METHOD_NAME = "NDDS_Transport_FILE_Property_verify";
	
    if (property == NULL) {
        NDDS_Transport_Log0printf("%s: Error, NULL property\n", METHOD_NAME);
	return 0;
    }
    
    ok = NDDS_Transport_Property_verify(&property->parent);

    if (property->parent.classid != NDDS_TRANSPORT_CLASSID_FILE) {
        NDDS_Transport_Log0printf("%s: %s\n", METHOD_NAME, "classid is incorrect");
	ok = 0;
    }

    if (property->parent.address_bit_count != NDDS_TRANSPORT_FILE_ADDRESS_BIT_COUNT) {
        NDDS_Transport_Log0printf("%s: %s\n", METHOD_NAME, "address_bit_count is incorrect");
	ok = 0;
    }

    if (property->received_message_count_max < 1) {
        NDDS_Transport_Log0printf("%s: %s\n", METHOD_NAME, "received_message_count_max < 1");
	ok = 0;
    }

    if (property->receive_buffer_size < property->parent.message_size_max ) {
        NDDS_Transport_Log0printf("%s: %s\n", METHOD_NAME, "receive_buffer_size < message_size_max");
	ok = 0;
    }

    return ok;
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Send a message over the transport-plugin.
*/
static 
RTI_INT32 NDDS_Transport_FILE_send(
    NDDS_Transport_Plugin               *self,
    const NDDS_Transport_SendResource_t *sendresource_in,
    const NDDS_Transport_Address_t      *dest_address_in, 
    const NDDS_Transport_Port_t          dest_port_in,
    RTI_INT32                            transport_priority_in,     
    const NDDS_Transport_Buffer_t        buffer_in[], 
    RTI_INT32                            buffer_count_in,
    void                                *reserved )
{
    const char *METHOD_NAME = "NDDS_Transport_FILE_send";
    struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *)self;

    struct NDDS_Transport_SendResource_FILE *sendResourceStruct =
            (struct NDDS_Transport_SendResource_FILE *)*sendresource_in;

    int i = 0, bytesToSend = 0;
    RTI_INT32 returnValue = 0; /* Indicates error */

    /* Verify pre-conditions */
    if ( me == NULL || buffer_in == NULL || buffer_count_in <= 0
        || (*sendresource_in == NULL) || dest_address_in == NULL ) {
        NDDS_Transport_Log0printf("%s: RTIOsapiSemaphore_give error\n", METHOD_NAME);
        return returnValue;
    }

    REDABufferArray_getSize(&bytesToSend, buffer_in, buffer_count_in);
    if ( NDDS_Transport_Log2Enabled(me) ) {
         printf("%s: sending %d bytes using resource %p to destination:\n",
                 METHOD_NAME, bytesToSend, sendResourceStruct);
         NDDS_Transport_FILE_print_address(dest_address_in, dest_port_in, me->_property.trace_level);
     }

    /* First check the file exists */
    FILE *file = fopen(sendResourceStruct->_fileName, "rb+");
    if ( file == NULL ) {
        /* This is normal. Indicates that there is no receiver at a particular address/port
         * it may indicate that discovery is sending to a port that is not there, or that
         * the application that was receiving data is no longer there
         */
        if ( NDDS_Transport_Log2Enabled(me) ) {
            printf("%s: file \"%s\" does not exist\n", METHOD_NAME, sendResourceStruct->_fileName);
        }
        return returnValue;
    }

    /* Now we know the file exists, reopen for writing */
    file = freopen ( NULL, "ab", file);
    if (  file == NULL ) {
        NDDS_Transport_Log0printf("%s: error opening file for write filename= \"%s\" errno= %d (%s)\n", METHOD_NAME,
                 sendResourceStruct->_fileName, errno, strerror(errno));

        goto doneClose;
    }

    /**** After this point we have to return via "goto doneClose" to ensure we close the file ****/

    /* Lock the file to make sure there is only one application writing to it */
    int fd = fileno(file);
    if ( fd == -1 ) {
        NDDS_Transport_Log0printf("%s: error in fileno. filename= \"%s\" errno= %d (%s)\n", METHOD_NAME,
                sendResourceStruct->_fileName, errno, strerror(errno));
    }

    /* This blocks until file can be locked. Use F_TLOCK to just test */
    if ( lockf(fd, F_LOCK, 0) == -1 ) {  /* zero offset indicates lock whole file */
        NDDS_Transport_Log0printf("%s: error locking filename= \"%s\" fd= %d, errno= %d (%s)\n", METHOD_NAME,
                sendResourceStruct->_fileName, fd, errno, strerror(errno));
        goto doneClose;
    }

    /* File successfully locked. */
    /**** After this point we have to return via "goto doneUnlock" to ensure we release the lock ****/

    /* Write a TransportMessageHeader that frames the packet to send (This is similar intent to the
     * UDP/IP header that the UDP transport puts around each message
     * '\n' 'F' 'I' 'F' '0' <messageSize> '\n'
     */
    NDDS_Transport_FILE_writeMessageHeader(file, bytesToSend);

    /* Write bytes to the end of the file */
    for (i = 0; i < buffer_count_in; ++i) { /* write data from each iovec */
        if ( fwrite(buffer_in[i].pointer, 1, buffer_in[i].length, file) != buffer_in[i].length ) {
            NDDS_Transport_Log0printf("%s: error writing to file filename= \"%s\" errno= %d (%s)\n", METHOD_NAME,
                   sendResourceStruct->_fileName, errno, strerror(errno));
            goto doneUnlock;
        }
    }

    ++sendResourceStruct->_sendMessageCount;
    if ( NDDS_Transport_Log2Enabled(me) ) {
         printf("%s: sent message count=%d  of %d bytes to destination: ",
                 METHOD_NAME, sendResourceStruct->_sendMessageCount, bytesToSend);
         NDDS_Transport_FILE_print_address(dest_address_in, dest_port_in, me->_property.trace_level);
    }

    returnValue = 1; /* Indicates success */

  doneUnlock:
    if ( lockf(fd, F_ULOCK, 0) == -1 ) {  /* zero offset indicates lock whole file */
        NDDS_Transport_Log0printf("%s: error unlocking filename= \"%s\" errno= %d (%s)\n", METHOD_NAME,
                sendResourceStruct->_fileName, errno, strerror(errno));
    }

  doneClose:
    fclose(file);

    return returnValue;
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Called to receive messages from a ReceiveResource.

    According to the API this function should block until there is data
    to return. To keep thisn simple in this example rather than block
    the function polls the file each 1 second until it gets new data.
*/
static 
RTI_INT32 NDDS_Transport_FILE_receive_rEA(
    NDDS_Transport_Plugin               *self,
    NDDS_Transport_Message_t            *message_out, 
    const NDDS_Transport_Buffer_t       *buffer_in,
    const NDDS_Transport_RecvResource_t *recvresource_in, 
    void                                *reserved )
{
    struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *)self;
    const char *METHOD_NAME = "NDDS_Transport_FILE_receive_rEA";
    struct NDDS_Transport_RecvResource_FILE *receiveResourceStruct
        = (struct NDDS_Transport_RecvResource_FILE *)*recvresource_in;

    int bytesNeededToRead = 0;
    int bytesCopiedToBuffer = 0;
    RTI_INT32 returnErrorValue = 0;   /* Indicates an error return */
    RTI_INT32 returnSuccessValue = 1; /* Indicates a successful return */

    /* Verify pre-conditions */
    if ( me == NULL || message_out == NULL
        || receiveResourceStruct == NULL || buffer_in == NULL ) {
        NDDS_Transport_Log0printf("%s: precondition error\n", METHOD_NAME);
        return returnErrorValue;
    }

    if ( NDDS_Transport_Log2Enabled(me) ) {
         printf("%s (port %d): file= %p, position= %d. Receive buffer can hold %d bytes\n",
                 METHOD_NAME, receiveResourceStruct->_portNum, receiveResourceStruct->_file,
                 (int)ftell(receiveResourceStruct->_file),
                 buffer_in->length );
    }

    while ( bytesCopiedToBuffer == 0 ) {

        if ( NDDS_Transport_Log2Enabled(me) ) {
             printf("%s (port %d): Waiting for message (received so far %d), file= %p, position= %d...\n",
                     METHOD_NAME, receiveResourceStruct->_portNum,
                     receiveResourceStruct->_receiveMessageCount,
                     receiveResourceStruct->_file,
                     (int)ftell(receiveResourceStruct->_file));
        }

        /* BEGIN of FILE LOCK ------------->>> */
        /* This blocks until file can be locked. Use F_TLOCK to just test */
        if ( lockf(receiveResourceStruct->_fileDescriptor, F_LOCK, 0) == -1 ) {  /* zero offset indicates lock whole file */
            NDDS_Transport_Log0printf("%s (port %d): error locking file fd= %d, errno= %d (%s)\n", METHOD_NAME,
                    receiveResourceStruct->_portNum, receiveResourceStruct->_fileDescriptor,
                    errno, strerror(errno));
            return returnErrorValue;
        }

        bytesCopiedToBuffer = NDDS_Transport_FILE_readNextMessage(
                                me,
                                receiveResourceStruct->_file,
                                receiveResourceStruct->_portNum,
                                buffer_in->pointer, buffer_in->length,
                                &bytesNeededToRead);

        if ( lockf(receiveResourceStruct->_fileDescriptor, F_ULOCK, 0) == -1 ) {  /* zero offset indicates lock whole file */
            NDDS_Transport_Log0printf("%s (port %d): error unlocking file errno= %d (%s)\n", METHOD_NAME,
                    receiveResourceStruct->_portNum, errno, strerror(errno));
        }
        /* END of FILE LOCK -------------<<< */

        if ( NDDS_Transport_Log2Enabled(me) ) {
             printf("%s (port %d): NDDS_Transport_FILE_readNextMessage returned %d, bytesNeeded= %d\n",
                     METHOD_NAME, receiveResourceStruct->_portNum, bytesCopiedToBuffer, bytesNeededToRead);
        }

        /* Check for errors */
        if ( bytesCopiedToBuffer == -1) {
            NDDS_Transport_Log0printf("%s (%d): unknown error\n", METHOD_NAME, receiveResourceStruct->_portNum);
            return returnErrorValue;
        }

        /* Check if there was something copied to buffer */
        if ( bytesCopiedToBuffer == 0 ) {
            if (bytesNeededToRead == 0)  {
                /* There was no data. Sleep a bit and try again */
                sleep(1);
                continue;
            }
            else {
                /* Indicates buffer was not big enough to copy message */
                 NDDS_Transport_Log0printf("%s (port %d): buffer of size %d is too small, it needs %d\n",
                         METHOD_NAME, receiveResourceStruct->_portNum,
                         buffer_in->length, bytesNeededToRead);
                 return returnErrorValue;
            }
        }
    }


    /* If we exit the loop here it is because we copied a message
     * to the buffer_in->pointer
     */

    message_out->buffer.pointer = buffer_in->pointer;
    message_out->buffer.length = bytesCopiedToBuffer;

    /* The loaned_buffer_param is only needed if the transport is zero-copy and
     * wants to loan a buffer to the middleware. In that case it should save in
     * loaned_buffer_param any information needed to retuer that loan when the
     * middleware calls return_loaned_buffer_rEA().
     * The FileTransport does not loan buffers so we can leave this unset or
     * put some arbitrary value
     */
    message_out->loaned_buffer_param = (void *)-1;

    ++receiveResourceStruct->_receiveMessageCount;
    if ( NDDS_Transport_Log2Enabled(me) ) {
         printf("%s (port %d): received message count=%d of %d bytes\n",
                 METHOD_NAME, receiveResourceStruct->_portNum,
                 receiveResourceStruct->_receiveMessageCount, bytesCopiedToBuffer );
    }

    return returnSuccessValue;
}


/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Returns a loaned buffer.
 */
static 
void NDDS_Transport_FILE_return_loaned_buffer_rEA(
    NDDS_Transport_Plugin    *self,
    const NDDS_Transport_RecvResource_t *recvresource_in, 
    NDDS_Transport_Message_t *message_in,
    void                     *reserved)
{
    /* This transport does not loan buffers on the receive_rEA() call
     * so there is really nothing to do here
     */
    message_in->loaned_buffer_param = NULL;
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Unblocks a receive thread.
 */
static 
RTI_INT32 NDDS_Transport_FILE_unblock_receive_rrEA(
    NDDS_Transport_Plugin               *self,
    const NDDS_Transport_RecvResource_t *recvresource_in, 
    void                                *reserved )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_unblock_receive_rrEA";
    struct NDDS_Transport_RecvResource_FILE *recvRes =
            (struct NDDS_Transport_RecvResource_FILE *)*recvresource_in;

    RTI_INT32 ok = 0;
    
    if (self == NULL ||	NDDS_Transport_Plugin_is_polled(self) ) {
        NDDS_Transport_Log0printf("%s: cannot unblock polled transport\n", METHOD_NAME);

	goto done;
    }
    
    NDDS_Transport_FILE_writeMessageHeader(recvRes->_file, 0);

    ok = 1;
 done:
    return ok;
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief Creates a ReceiveResource for a given port with a given
         transport_priority.

  Doesn't support multicast.
 */
static 
RTI_INT32 NDDS_Transport_FILE_create_recvresource_rrEA(
    NDDS_Transport_Plugin          *self, 
    NDDS_Transport_RecvResource_t  *recvresource_out, 
    NDDS_Transport_Port_t          *dest_port_inout,
    const NDDS_Transport_Address_t *multicast_address_in,
    RTI_INT32                       reserved )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_create_recvresource_rrEA";
    struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;

    if (*dest_port_inout == NDDS_TRANSPORT_PORT_INVALID) {
        char str[256];
        sprintf(str, "invalid port specified: %d", *dest_port_inout);
        NDDS_Transport_Log1print(me, str);
	return 0;
    }

    /* FileTransport does not support multicast. Fail if a multicast address is
     * specified
     */
    if ( multicast_address_in != NULL ) {
        NDDS_Transport_Log1print(me, "transport does not support multicast. ");
        if ( NDDS_Transport_Log2Enabled(me) ) {
            NDDS_Transport_FILE_print_address(multicast_address_in, *dest_port_inout,
                    me->_property.trace_level);
        }

        return 0;
    }

    /* Each port maps to a filename. We check if the file already exists
     * if it exists we fail. If it does not exist we create it and succeed.
     */
    *recvresource_out = NDDS_Transport_FILE_open_file_for_port(me, *dest_port_inout);
    if ( *recvresource_out == NULL) {
        NDDS_Transport_Log0printf("%s: failed to create receive resource for address= \"%s\", port= %d\n",
                METHOD_NAME, me->_property.address, *dest_port_inout);
        return 0;
    }

    /* Success */
    if ( NDDS_Transport_Log1Enabled(me) ) {
        printf("%s: created receive resource for address= \"%s\", port= %d\n",
                METHOD_NAME, me->_property.address, *dest_port_inout);
    }

    return 1;
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Try to share a ReceiveResource for receiving messages on a
    port. 

    Doesn't support multicast.
    Each port maps to a different file so only suceed if the same port
    is specified.
*/
static 
RTI_INT32 NDDS_Transport_FILE_share_recvresource_rrEA(
    NDDS_Transport_Plugin               *self, 
    const NDDS_Transport_RecvResource_t *recvresource_in,
    const NDDS_Transport_Port_t          recv_port_in,
    const NDDS_Transport_Address_t      *multicast_address_in,
    RTI_INT32                            reserved )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_share_recvresource_rrEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;
    struct NDDS_Transport_RecvResource_FILE *recvRes =
            (struct NDDS_Transport_RecvResource_FILE *)*recvresource_in;

    /* Transport does nor support multicast */
    if (multicast_address_in != NULL) {
        return 0;
    }

    /* This transport can only use a NDDS_Transport_RecvResource_t for a single port
     * so this function can only return TRUE if the requested port matches the one
     * in the NDDS_Transport_RecvResource_t
     */
    if ( recvRes->_portNum != recv_port_in ) {
        if ( NDDS_Transport_Log1Enabled(me) ) {
            printf("%s: resource at port '%d' not reused for port %d\n",
                    METHOD_NAME, recvRes->_portNum , recv_port_in);
        }

        return 0;
    }

    /* It is exactly the same port and we only have one address per transport
     * so you can reuse
     */
    if ( NDDS_Transport_Log1Enabled(me) ) {
        printf("%s: resource at port '%d' reused for port '%d'\n",
                METHOD_NAME, recvRes->_portNum, recv_port_in);
    }

    return 1;
}


/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief Unshares a recvresource.
*/
static 
RTI_INT32 NDDS_Transport_FILE_unshare_recvresource_rrEA(
    NDDS_Transport_Plugin               *self,
    const NDDS_Transport_RecvResource_t *recvresource_in,
    const NDDS_Transport_Port_t          dest_port_in, 
    const NDDS_Transport_Address_t      *multicast_address_in,
    RTI_INT32                            reserved )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_unshare_recvresource_rrEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;
    NDDS_Transport_Log1print(me, "");

    /* If we had done something special to share a resource on a prior call
     * to NDDS_Transport_FILE_share_recvresource_rrEA() we would undo that
     * here. Since we did not this function is a NOOP
     */
    return 1;
}


/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Creates a SendResource for the given destination address
           and port. 
*/
static 
RTI_INT32 NDDS_Transport_FILE_create_sendresource_srEA(
    NDDS_Transport_Plugin          *self, 
    NDDS_Transport_SendResource_t  *sendresource_out,
    const NDDS_Transport_Address_t *dest_address_in, 
    const NDDS_Transport_Port_t     dest_port_in,
    RTI_INT32                       transport_priority_in )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_create_sendresource_srEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;
    struct NDDS_Transport_SendResource_FILE *sendResourceStruct;

    *sendresource_out = NULL;

    /* multicast not supported */
    if ( NDDS_Transport_Address_is_multicast(dest_address_in) ) {
        if ( NDDS_Transport_Log1Enabled(me) ) {
            printf("%s: specified address is multicast and transport does not support multicast: ",
                    METHOD_NAME);
            NDDS_Transport_FILE_print_address(dest_address_in, dest_port_in, me->_property.trace_level);
        }

        return 0;
    }

    sendResourceStruct = (struct NDDS_Transport_SendResource_FILE *)
            calloc(1, sizeof(*sendResourceStruct));

    NDDS_Transport_Address_copy(&sendResourceStruct->_address, dest_address_in);
    sendResourceStruct->_portNum = dest_port_in;
    NDDS_Transport_FILE_getfilename_for_address_and_port(me, sendResourceStruct->_fileName,
            NDDS_TRANSPORT_FILE_MAX_FILEPATH_LEN, dest_address_in, dest_port_in);


    if ( NDDS_Transport_Log1Enabled(me) ) {
         printf("%s: Send resource %p (file =\"%s\") created for ",
                 METHOD_NAME, sendResourceStruct, sendResourceStruct->_fileName);
         NDDS_Transport_FILE_print_address(dest_address_in, dest_port_in, 2);
     }

    *sendresource_out = sendResourceStruct;
    return 1; 
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief Try to share a SendResource for sending messages to a
         destination. 
*/
static 
RTI_INT32 NDDS_Transport_FILE_share_sendresource_srEA(
    NDDS_Transport_Plugin               *self, 
    const NDDS_Transport_SendResource_t *sendresource_in,
    const NDDS_Transport_Address_t      *dest_address_in, 
    const NDDS_Transport_Port_t          dest_port_in, 
    RTI_INT32                            transport_priority_in )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_share_sendresource_srEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;

    struct NDDS_Transport_SendResource_FILE *sendResourceStruct =
            (struct NDDS_Transport_SendResource_FILE *)*sendresource_in;

    /* multicast not supported */
    if ( NDDS_Transport_Address_is_multicast(dest_address_in) ) {
	return 0;
    }

    /* This transport can only share a NDDS_Transport_SendResource_t if the destination
     * address and port number match
     */
    if  (  ( sendResourceStruct->_portNum != dest_port_in )
            || (!NDDS_Transport_Address_is_equal(&sendResourceStruct->_address, dest_address_in)) ) {

        if ( NDDS_Transport_Log2Enabled(me) ) {
             printf("%s: Send resource (%p) not reused for:\n", METHOD_NAME, sendResourceStruct);
             NDDS_Transport_FILE_print_address(dest_address_in, dest_port_in, me->_property.trace_level);
        }

        return 0;
    }

    if ( NDDS_Transport_Log1Enabled(me) ) {
         printf("%s: Send resource (%p) reused for \n", METHOD_NAME, sendResourceStruct);
         NDDS_Transport_FILE_print_address(dest_address_in, dest_port_in, me->_property.trace_level);
    }

    /* OK. Reuse */
    return 1;
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief Unshares a sendresource.
*/
static 
RTI_INT32 NDDS_Transport_FILE_unshare_sendresource_srEA(
    NDDS_Transport_Plugin               *self,
    const NDDS_Transport_SendResource_t *sendresource_in,
    const NDDS_Transport_Address_t      *dest_address_in, 
    const NDDS_Transport_Port_t          dest_port_in, 
    RTI_INT32                            transport_priority_in )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_unshare_sendresource_srEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;

    /* If we had done something special to share a resource on a prior call
     * to NDDS_Transport_FILE_share_sendresource_srEA() we would undo that
     * here. Since we did not this function is a NOOP
     */
    if ( NDDS_Transport_Log1Enabled(me) ) {
        struct NDDS_Transport_SendResource_FILE *sendResourceStruct =
                (struct NDDS_Transport_SendResource_FILE *)*sendresource_in;

        printf("%s: Send resource (%p) unshared for: \n", METHOD_NAME, sendResourceStruct);
        NDDS_Transport_FILE_print_address(dest_address_in, dest_port_in, me->_property.trace_level);
    }

   /* no op */
    return 1;
}


/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Destroys send or receive resource.
*/
static 
void NDDS_Transport_FILE_destroy_sendresource_srEA(
    NDDS_Transport_Plugin     *self,
    const NDDS_Transport_SendResource_t *resource_in )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_destroy_sendresource_srEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;
    struct NDDS_Transport_SendResource_FILE *sendResourceStruct
        = (struct NDDS_Transport_SendResource_FILE *)resource_in;

    if ( NDDS_Transport_Log1Enabled(me) ) {
        printf("%s: Send resource (%p) for port %d destroyed: ",
                METHOD_NAME, sendResourceStruct, sendResourceStruct->_portNum);
        NDDS_Transport_FILE_print_address(&sendResourceStruct->_address, sendResourceStruct->_portNum,
                me->_property.trace_level);
    }

    free(sendResourceStruct);
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Destroys send or receive resource.
*/
static
void NDDS_Transport_FILE_destroy_recvresource_rrEA(
    NDDS_Transport_Plugin     *self,
    const NDDS_Transport_RecvResource_t *resource_in )
{
    const char *METHOD_NAME="NDDS_Transport_FILE_destroy_recvresource_rrEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;
    struct NDDS_Transport_RecvResource_FILE *recvResourceStruct =
            (struct NDDS_Transport_RecvResource_FILE *)resource_in;

    if ( NDDS_Transport_Log1Enabled(me) ) {
        printf("%s: Receive resource (%p) for port %d destroyed\n",
                METHOD_NAME, recvResourceStruct, recvResourceStruct->_portNum);
    }

    fclose(recvResourceStruct->_file);
    free(recvResourceStruct);
}

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief Returns the string class name of the transport.
*/
const char *NDDS_Transport_FILE_get_class_name_cEA(
    NDDS_Transport_Plugin     *self ) 
{
    const char *METHOD_NAME="NDDS_Transport_FILE_get_class_name_cEA";
    const struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;
    NDDS_Transport_Log1print(me, NDDS_TRANSPORT_FILE_CLASS_NAME);

    return NDDS_TRANSPORT_FILE_CLASS_NAME;
}


/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief Converts a Transport-Plugin-specific address string to an
         IPv6 address. 
*/
RTI_INT32 NDDS_Transport_FILE_string_to_address_cEA(
    NDDS_Transport_Plugin     *self,
    NDDS_Transport_Address_t  *address_out,
    const char                *address_in ) 
{
    const char *METHOD_NAME = "NDDS_Transport_FILE_string_to_address_cEA";
    struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;
    NDDS_Transport_Log1print(me, "");

    if ( address_out == NULL || address_in == NULL) {
	printf("%s: precondition error\n", METHOD_NAME);
	return 0;
    }
    
    if ( NDDS_Transport_Log1Enabled(me) ) {
        printf("%s: Conversion of addresses not supported. Requested \"%s\"\n", METHOD_NAME, address_in);
        /* NDDS_Transport_FILE_print_address(address_out, -1, 2); */
    }

   /* Does not support string to IPv6 address conversion */
    return 0;
}


/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Returns a list of network interfaces represented by this
           transport-plugin instance. 

   The FILE NIF is represented by the address of this transport plugin
   instance.
   
   The FileTransport IPv6 address address is made globally unique
   by concatenating:
   
   (
       4-bytes: <network address configured by end user>
       4 bytes: <RTPS Host Id>, // or IPv4 address
       4-bytes: Process Id  // Note: could also be <RTPS App Id>
       4-bytes: <plugin_address truncated to 4 bytes>
   ) 

   So the FILE address uses only the last 12 bytes (96 bits) of the
   IPv6 address space.
*/
RTI_INT32 NDDS_Transport_FILE_get_receive_interfaces_cEA(
    NDDS_Transport_Plugin      *self,
    RTI_INT32                  *found_more_than_provided_for_out,
    RTI_INT32                  *interface_reported_count_out,
    NDDS_Transport_Interface_t  interface_array_inout[],
    RTI_INT32                   interface_array_size_in )
{

    const char *METHOD_NAME = "NDDS_Transport_FILE_get_receive_interfaces_cEA";
    struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *) self;

    if ( self == NULL || interface_array_size_in < 0 ||
	 interface_array_inout == NULL || found_more_than_provided_for_out == NULL
	 || interface_reported_count_out == NULL ) {

        NDDS_Transport_Log0printf("%s: precondition error\n", METHOD_NAME);
	return 0;
    }
    
    if (interface_array_size_in < 1) {
	*found_more_than_provided_for_out = 1;
	goto success;
    }
    *found_more_than_provided_for_out = 0;
    
    /* --- output the interface --- */        
    *interface_reported_count_out = 1;
    interface_array_inout[0].transport_classid = NDDS_TRANSPORT_CLASSID_FILE;

    /* Zero bytes */
    memset(&interface_array_inout[0].address.network_ordered_value, 0, NDDS_TRANSPORT_ADDRESS_LENGTH);

    /* Set the meaningful bytes of the address */
    {
        struct in_addr ipAddr;
        unsigned int ipAddrNetworkOrder;

        inet_aton(me->_property.address, &ipAddr);
        ipAddrNetworkOrder = ipAddr.s_addr;
        interface_array_inout[0].address.network_ordered_value[15] = (ipAddrNetworkOrder      )% 256;
        interface_array_inout[0].address.network_ordered_value[14] = (ipAddrNetworkOrder >>  8)% 256;
        interface_array_inout[0].address.network_ordered_value[13] = (ipAddrNetworkOrder >> 16)% 256;
        interface_array_inout[0].address.network_ordered_value[12] = (ipAddrNetworkOrder >> 24)% 256;
    }


 success:  
    if ( NDDS_Transport_Log1Enabled(me) ) {
	int i;
	printf("%s: num receive interfaces: %d\n", METHOD_NAME, *interface_reported_count_out);
	for (i=0; i< *interface_reported_count_out; ++i ) {
	    char ifaddr[256];
	    NDDS_Transport_Address_to_string(&interface_array_inout[i].address,
					     ifaddr, 256);
	    printf("    interface[%d] = \"%s\" (%s)\n", i, me->_property.address, ifaddr);
	}
    }

    return 1;
}  

/* ----------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
    @brief Register a callback to be notified of the changes in the
           availability of interfaces supported by the plugin.

    Doesn't do anything with a listener.
 */
static
RTI_INT32 NDDS_Transport_FILE_register_listener_cEA(
    NDDS_Transport_Plugin   *self,
    NDDS_Transport_Listener *listener )
{
    const char *METHOD_NAME = "NDDS_Transport_FILE_register_listener_cEA";
	 
    if (self == NULL) {
	printf("%s: NULL transport\n", METHOD_NAME);
	return 0;
    }
        
    /* Transport is static */
    return 1;
}                           

/* ---------------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief (Destructor) Called to destroy the Transport-Plugin. 

  @pre No RecvResources or SendResources exist for this transport

  @post Transport has been deleted. It should not be used after this call.
  
  Deletes the memory allocated when NDDS_Transport_FILE_new()
  was called. Does not take care of deleting any RecvResources or
  SendResources associated with the transport. It is the
  responsibility NDDS to delete all SendResources and RecvResources
  associated with the transport prior to deleting the transport
  itself.

 */
static 
void NDDS_Transport_FILE_delete_cEA(
    NDDS_Transport_Plugin *self,
    void                  *reserved )
{
    const char *METHOD_NAME = "NDDS_Transport_FILE_delete_cEA";
    struct NDDS_Transport_FILE *me = (struct NDDS_Transport_FILE *)self;

    if (me == NULL) { 
	printf("%s: NULL transport\n", METHOD_NAME);
	return; 
    }

    if (me->_buffer != NULL) { 
        free(me->_buffer);
    }

    if ( NDDS_Transport_Log1Enabled(me) ) {
         printf("%s: Transport  (%p) for address %s destroyed\n",
                 METHOD_NAME, me, me->_property.address);
    }

    free(me);
}



/* ---------------------------------------------------------------------- */
/*i \ingroup NDDS_Transport_FILE
  @brief (Constructor) Create a intra-transport-plugin instance.
 */
NDDS_Transport_Plugin* NDDS_Transport_FILE_newPlugin(
			const struct NDDS_Transport_FILE_Property_t *property_in)
{
    const char *METHOD_NAME = "NDDS_Transport_FILE_newI";
    struct NDDS_Transport_FILE *me = NULL;

    const struct NDDS_Transport_FILE_Property_t defaultProp
	= NDDS_TRANSPORT_FILE_PROPERTY_DEFAULT;

    int bufferSize = 0;
    int pid = 0;

    /* allocate instance */
    me = (struct NDDS_Transport_FILE *)calloc(1, sizeof(*me));
    if (me == NULL) {
	printf("%s: malloc(%d) failed\n", METHOD_NAME, (int)sizeof(*me));
	goto failed;
    }

    /* Set the property */
    if (property_in == NULL) {
	me->_property = defaultProp;
    } else {
	me->_property = *property_in;
    }
    me->parent.property = (struct NDDS_Transport_Property_t *)&me->_property;

    if ( !NDDS_Transport_FILE_Property_verify(&me->_property) ) {
        printf("%s: %s\n", METHOD_NAME, "Invalid transport properties.");
	goto failed;
    }

    pid = RTIOsapiProcess_getId();
    {
        char str[256];
        sprintf(str, "pid: %d bound to address: \"%s\"",  pid, me->_property.address);
        NDDS_Transport_Log1print(me, str);
    }

    /* Ensure existence of directoryRoot that will contain the files associated with receive resource  */
    if ( !NDDS_Transport_FILE_ensure_receive_directory_exists(me) ) {
        goto failed;
    }
    
    /* create receive buffer */

    bufferSize = me->_property.receive_buffer_size + NDDS_Transport_FILE_messageHeaderLength();

    /* The extra 8 is to compensate for the worst case alignments that are required
     * when serializing to the buffer */
    me->_buffer = (char *)calloc(1, bufferSize + 8);
    if (me->_buffer == NULL) {
        printf("%s: RTIOsapiHeap_allocateBuffer semaphore error\n", METHOD_NAME);
        goto failed;
    }


    me->parent.send =        NDDS_Transport_FILE_send;
    me->parent.receive_rEA = NDDS_Transport_FILE_receive_rEA;
    me->parent.return_loaned_buffer_rEA = 
                        NDDS_Transport_FILE_return_loaned_buffer_rEA;

    me->parent.unblock_receive_rrEA  = 
                        NDDS_Transport_FILE_unblock_receive_rrEA;
    me->parent.create_recvresource_rrEA   =
                        NDDS_Transport_FILE_create_recvresource_rrEA;
    me->parent.destroy_recvresource_rrEA  =
                        NDDS_Transport_FILE_destroy_recvresource_rrEA;
    me->parent.share_recvresource_rrEA   =
                        NDDS_Transport_FILE_share_recvresource_rrEA;
    me->parent.unshare_recvresource_rrEA =
                        NDDS_Transport_FILE_unshare_recvresource_rrEA;

    me->parent.create_sendresource_srEA = 
                        NDDS_Transport_FILE_create_sendresource_srEA;
    me->parent.destroy_sendresource_srEA =
                        NDDS_Transport_FILE_destroy_sendresource_srEA;
    me->parent.share_sendresource_srEA = 
                        NDDS_Transport_FILE_share_sendresource_srEA;
    me->parent.unshare_sendresource_srEA =
                        NDDS_Transport_FILE_unshare_sendresource_srEA;

    me->parent.get_class_name_cEA =
                        NDDS_Transport_FILE_get_class_name_cEA;
    me->parent.string_to_address_cEA =
                        NDDS_Transport_FILE_string_to_address_cEA;
    me->parent.get_receive_interfaces_cEA = 
                        NDDS_Transport_FILE_get_receive_interfaces_cEA;
    me->parent.register_listener_cEA = 
                        NDDS_Transport_FILE_register_listener_cEA;

    me->parent.delete_cEA = NDDS_Transport_FILE_delete_cEA;
    
    return (NDDS_Transport_Plugin*) me;

 failed:

    NDDS_Transport_FILE_delete_cEA((NDDS_Transport_Plugin*)me, NULL);

    return NULL;
}

NDDS_Transport_Plugin *NDDS_Transport_FILE_create(
    NDDS_Transport_Address_t *default_network_address_out, 
    const struct DDS_PropertyQosPolicy *property_in)
{
    struct NDDS_Transport_FILE_Property_t fifoProperty
	= NDDS_TRANSPORT_FILE_PROPERTY_DEFAULT;

    const struct DDS_Property_t *p;

    p = DDS_PropertyQosPolicyHelper_lookup_property((struct DDS_PropertyQosPolicy *)property_in, "address");
    if ( p != NULL ) {
        strncpy(fifoProperty.address, p->value, NDDS_TRANSPORT_FILE_MAX_ADDR_LEN-1);
        fifoProperty.address[NDDS_TRANSPORT_FILE_MAX_ADDR_LEN-1] = '\0';
    } else {
        char defaultAddress[] = NDDS_TRANSPORT_FILE_ADDRESS_DEFAULT;
        strcpy(fifoProperty.address, defaultAddress);
    }

    p = DDS_PropertyQosPolicyHelper_lookup_property((struct DDS_PropertyQosPolicy *)property_in, "trace_level");
    if ( p != NULL ) {
        fifoProperty.trace_level = strtoul(p->value, NULL, 10);
    } else {
        fifoProperty.trace_level = 0;
    }

    p = DDS_PropertyQosPolicyHelper_lookup_property((struct DDS_PropertyQosPolicy *)property_in, "directory");
    if ( p != NULL ) {
        strncpy(fifoProperty.directoryRoot, p->value, NDDS_TRANSPORT_FILE_MAX_FILEPATH_LEN-1);
        fifoProperty.directoryRoot[NDDS_TRANSPORT_FILE_MAX_FILEPATH_LEN-1] = '\0';
    } else {
        char defaultDirectoryRoot[] = NDDS_TRANSPORT_FILE_DIRECTORY_ROOT_DEFAULT;
        strcpy(fifoProperty.directoryRoot, defaultDirectoryRoot);
    }


    NDDS_Transport_Plugin *plugin = NDDS_Transport_FILE_newPlugin(&fifoProperty);
    return plugin;
}


/* end of $Id: FILE.c,v 1.20 2008/10/22 19:16:46 jim Exp $ */


