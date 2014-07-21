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
/*
 * DataStructures.h
 */

#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_
#include "directory_reading.h"

#define MAX_PAYLOAD_SIZE 1024


/* ========================================================================= */
/*                                                                           */
/* Data types                                                                */
/*                                                                           */
/* ========================================================================= */
#include "ndds/ndds_c.h"
#include "routingservice/routingservice_adapter.h"

struct RTI_RoutingServiceFileAdapterPlugin {
    struct RTI_RoutingServiceAdapterPlugin _base;
};
/*****************************************************************************/
struct RTI_RoutingServiceFileConnection {

    /* the path of the directory to scan */
    char path[256];
    /*
     * thread identifier and running parameter, is_running_enabled is 1 when the
     * thread starts, when it turns to 0 the thread stops
     */

    pthread_t tid;
    int is_running_enabled;
    int sleep_period;
    /*pointer to input and output discovery reader*/
    struct RTI_RoutingServiceFileStreamReader * input_discovery_reader;
    /*input and output discovery listener (not pointer)*/
    struct RTI_RoutingServiceStreamReaderListener  input_discovery_listener;
    /*
     *this field is used to distinguish the connection,
     *this basically if it is the connection 1 or 2
     */
    int is_input;

};

/*****************************************************************************/

struct RTI_RoutingServiceFileStreamWriter {
    /* streamInfo associated with the stream reader */
    const struct RTI_RoutingServiceStreamInfo * info;
    /* file where we are writing */
    FILE * file;
    /* properties flush for writing on the file */
    int flush_enabled;
};

/*****************************************************************************/

struct RTI_RoutingServiceFileStreamReader {
    /*
     * listener associated to the stream reader, got as a parameter in the
     * function create_stream_reader
     */
    struct RTI_RoutingServiceStreamReaderListener listener;
    /* streamInfo associated with the stream reader */
    const struct RTI_RoutingServiceStreamInfo * info;
    /* typecode associated with the stream */
    struct DDS_TypeCode * type_code;
    /*
     * parameters for the execution of the thread that periodically
     * notify on_data_available
     */
    pthread_t tid;
    int is_running_enabled;
    /*parameters for opening the file and reading it */
    struct DDS_Duration_t read_period;
    int samples_per_read;
    /*file we are reading */
    FILE * file;
    /*connection which the stream reader belongs to */
    struct RTI_RoutingServiceFileConnection *connection;
    /*the array of filenames present in the source directory*/
    char** discovery_data;
    /*counter for discovery_data array, indicate the last entry that has been read*/
    int discovery_data_counter_read;
    /* counter for discovery data array, indicate the
     * total number of entry (file names) inside the array*/
    int discovery_data_counter;
    /*
     * The discovery_daata array, is populated with the name of all files
     * present inside the directory monitored, then every new file discovered
     * gets on the top of the array, so, if there is something new, we realize
     * it because there is something "after" the discovery data index, as it
     * represent the last position of the array that we have read to build
     * new stream.
     */
};



#endif /* DATASTRUCTURES_H_ */
