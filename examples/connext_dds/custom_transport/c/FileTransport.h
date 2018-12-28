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

#include "transport/transport_interface.h"

/* ================================================================= */
/*               Intra Process Transport-Plugin                      */
/* ================================================================= */
#define NDDS_TRANSPORT_CLASSID_FILE            (20)
#define NDDS_TRANSPORT_FILE_CLASS_NAME         "FileTransport"

/* Indicates the number of bits that are significant to the FIFO
   transport. In other words the size of the FileTransport addresses.

   We make the FileTrasport use 4-byte addresses like IPv4.
   This means that we leave the remaining 96 bits configurable for
   a "network" address
*/
#define NDDS_TRANSPORT_FILE_ADDRESS_BIT_COUNT  (32)
#define NDDS_TRANSPORT_FILE_PROPERTIES_BITMAP_DEFAULT \
              ( NDDS_TRANSPORT_PROPERTY_BIT_BUFFER_ALWAYS_LOANED )

#define NDDS_TRANSPORT_FILE_GATHER_SEND_BUFFER_COUNT_MAX_DEFAULT (1024)
#define NDDS_TRANSPORT_FILE_MESSAGE_SIZE_MAX_DEFAULT (9216)
#define NDDS_TRANSPORT_FILE_RECEIVED_MESSAGE_COUNT_MAX_DEFAULT  (10)
#define NDDS_TRANSPORT_FILE_TRACE_LEVEL_DEFAULT (1)

#define NDDS_TRANSPORT_FILE_RECEIVE_BUFFER_SIZE_DEFAULT  \
        (NDDS_TRANSPORT_FILE_RECEIVED_MESSAGE_COUNT_MAX_DEFAULT * \
         NDDS_TRANSPORT_FILE_MESSAGE_SIZE_MAX_DEFAULT)

#define NDDS_TRANSPORT_FILE_MAX_ADDR_LEN (32)

#define NDDS_TRANSPORT_FILE_ADDRESS_DEFAULT \
        {'1','.','1','.','1','.','1','\0'}

#define NDDS_TRANSPORT_FILE_DIRECTORY_ROOT_DEFAULT \
        {'/','t','m','p','/','d','d','s','/','F','i','l','e','T','r','a','n','s','p','o','r','t','\0'}

#define NDDS_TRANSPORT_FILE_MAX_FILEPATH_LEN (128)

struct NDDS_Transport_FILE_Property_t {
    /*e @brief Generic properties of all Transport Plugins. 
    */
    struct NDDS_Transport_Property_t parent;

    RTI_INT32 received_message_count_max;
    RTI_INT32 receive_buffer_size;
    RTI_INT32 trace_level;

    char address[NDDS_TRANSPORT_FILE_MAX_ADDR_LEN];
    char directoryRoot[NDDS_TRANSPORT_FILE_MAX_FILEPATH_LEN];
};

#define NDDS_TRANSPORT_FILE_PROPERTY_DEFAULT { \
    { NDDS_TRANSPORT_CLASSID_FILE, \
      NDDS_TRANSPORT_FILE_ADDRESS_BIT_COUNT, \
      NDDS_TRANSPORT_FILE_PROPERTIES_BITMAP_DEFAULT, \
      NDDS_TRANSPORT_FILE_GATHER_SEND_BUFFER_COUNT_MAX_DEFAULT, \
      NDDS_TRANSPORT_FILE_MESSAGE_SIZE_MAX_DEFAULT, \
      NULL, 0, /* allow_interfaces_list */ \
      NULL, 0, /* deny_interfaces_list */ \
      NULL, 0, /* allow_multicast_interfaces_list */ \
      NULL, 0, /* deny_multicast_interfaces_list */ \
    }, \
    NDDS_TRANSPORT_FILE_RECEIVED_MESSAGE_COUNT_MAX_DEFAULT, \
    NDDS_TRANSPORT_FILE_RECEIVE_BUFFER_SIZE_DEFAULT,    \
    NDDS_TRANSPORT_FILE_TRACE_LEVEL_DEFAULT, \
    NDDS_TRANSPORT_FILE_ADDRESS_DEFAULT, \
    NDDS_TRANSPORT_FILE_DIRECTORY_ROOT_DEFAULT \
}

NDDS_Transport_Plugin* NDDS_Transport_FILE_newPlugin(
    const struct NDDS_Transport_FILE_Property_t *property_in);

struct DDS_PropertyQosPolicy;

NDDS_Transport_Plugin *NDDS_Transport_FILE_create(
    NDDS_Transport_Address_t *default_network_address_out, 
    const struct DDS_PropertyQosPolicy *property_in);
