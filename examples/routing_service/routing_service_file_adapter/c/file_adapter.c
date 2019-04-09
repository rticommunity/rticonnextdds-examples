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

/* ========================================================================= */
/* RTI Routing Service File Adapter                                          */
/* ========================================================================= */
/* This is a pluggable adapter that reads blocks from a file                 */
/* providing them to Routing Service as DynamicData samples                  */
/* and receives samples from Routing Service to write them in a file         */
/*                                                                           */
/* To customize to your data format, edit LineConversion.c                   */
/*                                                                           */
/* ========================================================================= */

#include <stdio.h>
#include <string.h>

#include"data_structures.h"
#include "line_conversion.h"

#include "ndds/ndds_c.h"
#include "routingservice/routingservice_adapter.h"

#include <sys/select.h>
#include <semaphore.h>
#include <pthread.h>

/* This function creates the typecode */
DDS_TypeCode * RTI_RoutingServiceFileAdapter_create_type_code()
{

    struct DDS_TypeCodeFactory * factory = NULL;
    struct DDS_TypeCode * sequence_tc = NULL; /* type code for octet */
    struct DDS_TypeCode * struct_tc = NULL; /* Top-level typecode */
    struct DDS_StructMemberSeq member_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

    /* we get the instance of the type code factory */
    factory = DDS_TypeCodeFactory_get_instance();
    if (factory == NULL) {
        fprintf(stderr, "ERROR: Unable to get type code factory singleton\n");
        goto done;
    }

    /* create a sequence for DDS_Octet, so we use a general type */
    sequence_tc = DDS_TypeCodeFactory_create_sequence_tc(
            factory,
            MAX_PAYLOAD_SIZE,
            DDS_TypeCodeFactory_get_primitive_tc(factory,
                    DDS_TK_OCTET),
                    &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "ERROR: Unable to create 'payload' sequence typecode:"
                " %d\n", ex);
        goto done;
    }

    /* create top-level typecode */
    struct_tc = DDS_TypeCodeFactory_create_struct_tc(
            factory,
            "TextLine",
            &member_seq,
            &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "Unable to create struct typecode, error = %d \n", ex);
        goto done;
    }

    DDS_TypeCode_add_member(struct_tc,
            "value",
            DDS_MEMBER_ID_INVALID,
            sequence_tc,
            DDS_TYPECODE_NONKEY_MEMBER,
            &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"Error adding member to struct typecode, error=%d\n",
                ex);
        goto done;
    }
    if (sequence_tc !=NULL) {
        DDS_TypeCodeFactory_delete_tc(factory,sequence_tc,&ex);
    }
    return struct_tc;

done:
    if (sequence_tc != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, sequence_tc, &ex);
    }
    if (struct_tc != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, struct_tc, &ex);
    }
    return NULL;
}

void RTI_RoutingServiceFileAdapter_delete_type_code(DDS_TypeCode * type_code)
{
    DDS_TypeCodeFactory * factory = NULL;
    DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

    factory = DDS_TypeCodeFactory_get_instance();
    if (factory == NULL) {
        fprintf(stdout,
                "ERROR getting instance DDS_TypeCodeFactory deleting typecode");
        return;
    } else {
        if (type_code != NULL) {
            DDS_TypeCodeFactory_delete_tc(factory,type_code,&ex);
        }
        if (ex != DDS_NO_EXCEPTION_CODE) {
            fprintf(stderr, "Unable to delete typecode\n");
            return;
        }
    }
}

/* ========================================================================= */
/*                                                                           */
/* Stream reader methods                                                     */
/*                                                                           */
/* ========================================================================= */

void * RTI_RoutingServiceFileStreamReader_run(void * threadParam)
{

    struct RTI_RoutingServiceFileStreamReader * self =
            (struct RTI_RoutingServiceFileStreamReader *) threadParam;

    /* This thread will notify periodically data availability in the file */
    while (self->is_running_enabled) {

        NDDS_Utility_sleep(&self->read_period);

        if (!feof(self->file)) {
            self->listener.on_data_available(
                    self, self->listener.listener_data);
        }
    }
    return NULL;
}

/*****************************************************************************/

/*
 * This function gets called inside the function read or by the return loan,
 * it frees the dynamic array of dynamic data, when we return the loan,
 * or when we have some errors, after allocating memory for the sample
 * list
 */
void RTI_RoutingServiceFileStreamReader_freeDynamicDataArray(
        struct DDS_DynamicData ** samples,
        int count)
{
    int i = 0;
    for (i = 0; i < count; i++) {
        if (samples[i] != NULL) {
            DDS_DynamicData_delete(samples[i]);
            samples[i] = NULL;
        }
    }
    free(samples);
    samples = NULL;

}

/*****************************************************************************/

/*
 * The read function gets called every time the routing service
 *  gets notified of data available by the function on_data_available
 *  of every stream reader's listener
 */
void RTI_RoutingServiceFileStreamReader_read(
        RTI_RoutingServiceStreamReader stream_reader,
        RTI_RoutingServiceSample ** sample_list,
        RTI_RoutingServiceSampleInfo ** info_list,
        int * count,
        RTI_RoutingServiceEnvironment * env)
{

    int i = 0, sample_counter = 0;

    struct DDS_DynamicData * sample = NULL;
    struct DDS_DynamicDataProperty_t     dynamic_data_props =
            DDS_DynamicDataProperty_t_INITIALIZER;

    struct RTI_RoutingServiceFileStreamReader * self =
            (struct RTI_RoutingServiceFileStreamReader *) stream_reader;
    /*
     * We don't provide sample info in this adapter, which
     * is an optional feature
     */
    *info_list = NULL;

    /*
     * if the function read it is called because we have discovery data,
     * the pointer or the stream reader that calls the
     */
    if ((self->connection->input_discovery_reader == self)) {
        int new_discovered_samples = 0;

        fprintf(stdout,"DiscoveryReader: called function read "
                "for input discovery\n");
        /*
         * we keep track in the checking thread of the number of file names
         * inside the discovery_data array, every new discovered file, we
         * increase this counter (discovery_data_counter), and we keep track
         * of the files we have already read, and created the relative streams,
         * with the discovery_data_counter_read, subtracting one to another, we
         * know how many new discovered files we have
         */
        new_discovered_samples = self->discovery_data_counter -
                self->discovery_data_counter_read;

        /*
         * we receive as a parameter the pointer to an array, and we need to
         * allocate memory for it, first for the array, then for the single
         * elements we put in it.
         */
        *sample_list = calloc(
                new_discovered_samples,
                sizeof(struct RTI_RoutingServiceStreamInfo *));
        if (*sample_list == NULL) {
            RTI_RoutingServiceEnvironment_set_error(
                    env, "Failure creating dynamic data sample in read "
                    "function for discovery");
            return;
        }

        /*
         * inside this loop we create the sample info structures,
         * to describe the new stream to be created
         */
        for (i = 0; i < new_discovered_samples; i++) {
            struct RTI_RoutingServiceStreamInfo *streaminfo;
            /*
             * here we create the stream info type, passing as string name
             * the name of the file taken from the discovery_data array
             * filled by the checking thread.
             */
            streaminfo = RTI_RoutingServiceStreamInfo_new_discovered(
                    self->discovery_data[self->discovery_data_counter_read],
                    "TextLine",/*typename*/
                    RTI_ROUTING_SERVICE_TYPE_REPRESENTATION_DYNAMIC_TYPE,
                    self->type_code);

            if (streaminfo == NULL) {
                continue;
            }

            (*sample_list)[*count] = streaminfo;
            /*
             * we increment the count of the sample info generated and returned
             * inside the sampli_list dynamic array.
             */
            (*count)++;
            /*
             * we increment the index as we have already read that
             * position of the array
             */
            self->discovery_data_counter_read++;
        }


    } else {

        fprintf(stdout,"StreamReader: called function read for data\n");

        *sample_list = calloc(self->samples_per_read,sizeof(DDS_DynamicData *));
        if (*sample_list == NULL) {
            RTI_RoutingServiceEnvironment_set_error(
                    env,"Failure creating dynamic data "
                            "sample list in read function");
            return;
        }

        /*
         *  Read as many times as samples_per_read
         *  (or less if we encounter the end of file)
         */
        for (i = 0; i < self->samples_per_read && !feof(self->file); i++) {

            /*
             * Create a dynamic data sample for every buffer we read. We use
             * the type we received when the stream reader was created
             */
            sample = DDS_DynamicData_new(self->type_code, &dynamic_data_props);
            if (sample == NULL) {
                RTI_RoutingServiceEnvironment_set_error(
                        env,
                        "Failure creating dynamic data sample in read function");
                RTI_RoutingServiceFileStreamReader_freeDynamicDataArray(
                        (struct DDS_DynamicData **) *sample_list, sample_counter);
                *sample_list = NULL;
                return;
            }
            /*
             * Fill the dynamic data sample fields
             * with the buffer read from the file.
             */
            if ( !RTI_RoutingServiceFileAdapter_read_sample(
                    sample, self->file,
                    env)) {
                /* No sample read */
                DDS_DynamicData_delete(sample);
                continue;
            }

            (*sample_list)[sample_counter++] = sample;
        }
        /* Set the count to the actual number of samples we have generated */
        *count = sample_counter;

    }
    /*
     * If there are no samples to read we free the memory allocated straight
     * away as the routing service wouldn't call return_loan
     */
    if (*count == 0) {
        /* If we report zero samples we have to free the array now */
        free(*sample_list);
        *sample_list = NULL;
    }


}


/*****************************************************************************/

void RTI_RoutingServiceFileStreamReader_return_loan(
        RTI_RoutingServiceStreamReader stream_reader,
        RTI_RoutingServiceSample * sample_list,
        RTI_RoutingServiceSampleInfo * info_list,
        int count,
        RTI_RoutingServiceEnvironment * env)
{
    /* Release all the memory allocated with read() */
    int i;
    struct RTI_RoutingServiceFileStreamReader *self =
            (struct RTI_RoutingServiceFileStreamReader*) stream_reader;

    fprintf(stdout,"StreamReader: called function return_loan\n");

    /*
     * In case this is called on the discovery data, we free the memory for
     * sample info otherwise we delete the dynamic data.
     */
    if (self->connection->input_discovery_reader == self) {
        for (i = 0; i < count; i++) {
            RTI_RoutingServiceStreamInfo_delete(sample_list[i]);
        }
        free(sample_list);
        sample_list = NULL;
    }
    else{
        RTI_RoutingServiceFileStreamReader_freeDynamicDataArray(
                (struct DDS_DynamicData **)sample_list, count);
    }
}

/* ========================================================================= */
/*                                                                           */
/* Stream stream_writer methods                                              */
/*                                                                           */
/* ========================================================================= */

int RTI_RoutingServiceFileStreamWriter_write(
        RTI_RoutingServiceStreamWriter stream_writer,
        const RTI_RoutingServiceSample * sample_list,
        const RTI_RoutingServiceSampleInfo * info_list,
        int count,
        RTI_RoutingServiceEnvironment * env)
{
    struct DDS_DynamicData * sample = NULL;
    int i = 0;
    int written_sample=0;
    struct RTI_RoutingServiceFileStreamWriter * self =
            (struct RTI_RoutingServiceFileStreamWriter *) stream_writer;

    fprintf(stdout,"StreamWriter: called function write\n");

    /* we explore the sample_list dynamic array we received */
    for (i = 0; i < count; i++) {
        /*we convert to dynamic data as we can read it*/
        sample = (struct DDS_DynamicData *) sample_list[i];
        /*
         * the function RTI_RoutingServiceFileAdapter_write_sample will take
         * care to write to the file with the name of the stream.
         */
        if (!RTI_RoutingServiceFileAdapter_write_sample(
                sample, self->file, env)) {
            continue;
        }
        if (self->flush_enabled) {
            fflush(self->file);
        }
        written_sample++;
    }
    return written_sample;
}

/* ========================================================================= */
/*                                                                           */
/* Connection methods                                                        */
/*                                                                           */
/* ========================================================================= */

RTI_RoutingServiceSession
RTI_RoutingServiceFileConnection_create_session(
        RTI_RoutingServiceConnection connection,
        const struct RTI_RoutingServiceProperties * properties,
        RTI_RoutingServiceEnvironment * env)
{
    pthread_attr_t thread_attribute;
    struct RTI_RoutingServiceFileConnection* file_connection =
            (struct RTI_RoutingServiceFileConnection*) connection;

    fprintf(stdout,"Connection: called function create_session\n");

    if (file_connection->is_input == 1) {
        pthread_attr_init(&thread_attribute);
        pthread_attr_setdetachstate(&thread_attribute, PTHREAD_CREATE_JOINABLE);
        if (pthread_create(
                &file_connection->tid,
                NULL,
                RTI_RoutingServiceFileAdpater_checking_thread,
                (void*)file_connection) < 0) {

            RTI_RoutingServiceEnvironment_set_error(
                    env, "Error creating thread for directory"
                    "scanning");
            return NULL;

        }
    }

    /*
     * we don't actually use session, we return the Thread identifier
     * as for us that is the concept of session. We start here the thread
     * because we need to be sure that we already have the discovery reader
     * ready, and in this case this happens, as the create session function
     * gets called after the getInputDiscoveryReader
     */
    return &file_connection->tid;
}

/*****************************************************************************/

void RTI_RoutingServiceFileConnection_delete_session(
        RTI_RoutingServiceConnection connection,
        RTI_RoutingServiceSession session,
        RTI_RoutingServiceEnvironment * env)
{
    struct RTI_RoutingServiceFileConnection *file_connection =
            (struct RTI_RoutingServiceFileConnection*) connection;
    /* we wait for the discovery thread to end */

    if (file_connection->is_input) {
        file_connection->is_running_enabled = 0;
        pthread_join(file_connection->tid,NULL);
        fprintf(stdout,"thread discovery ended\n");
    }

    fprintf(stdout,"Connection: called function delete_session\n");

    /* We don't need sessions in this example */
}

/*****************************************************************************/

void RTI_RoutingServiceFileStreamReader_delete(
        struct RTI_RoutingServiceFileStreamReader * self)
{

    if (self->file != NULL) {
        fclose(self->file);
    }

    free(self);
}

/*****************************************************************************/

RTI_RoutingServiceStreamReader 
RTI_RoutingServiceFileConnection_create_stream_reader(
        RTI_RoutingServiceConnection connection,
        RTI_RoutingServiceSession session,
        const struct RTI_RoutingServiceStreamInfo * stream_info,
        const struct RTI_RoutingServiceProperties * properties,
        const struct RTI_RoutingServiceStreamReaderListener * listener,
        RTI_RoutingServiceEnvironment * env)
{

    struct     RTI_RoutingServiceFileStreamReader * stream_reader = NULL;
    int read_period = 0;
    int samples_per_read = 0;
    int error = 0;
    const char * read_period_property = NULL;
    const char * samples_per_read_property = NULL;
    char * filename = NULL;
    FILE * file = NULL;
    pthread_attr_t thread_attribute;
    struct RTI_RoutingServiceFileConnection *file_connection =
            (struct RTI_RoutingServiceFileConnection *) connection;

    fprintf(stdout,"Connection: called function create_stream_reader\n");

    /* Get the configuration properties in <route>/<input>/<property> */
    read_period_property = RTI_RoutingServiceProperties_lookup_property(
            properties, "ReadPeriod");
    if (read_period_property == NULL) {
        read_period = 1000;
    } else {
        read_period = atoi(read_period_property);
        if (read_period <= 0) {
            RTI_RoutingServiceEnvironment_set_error(env,
                    "Error: read_period property value not valid");
            return NULL;
        }
    }

    samples_per_read_property = RTI_RoutingServiceProperties_lookup_property(
            properties, "SamplesPerRead");
    if (samples_per_read_property == NULL) {
        samples_per_read = 1;
    } else {
        samples_per_read = atoi(samples_per_read_property);
        if (samples_per_read <= 0) {
            RTI_RoutingServiceEnvironment_set_error(env,
                    "ERROR:samples_per_read property value not valid");
            return NULL;
        }
    }

    /*
     * now we create the string of the perfect size, the filename is formed by
     * path that we already have by the connection, the / as separator between
     * path and filename, and \0 as terminator. after that we create the string
     * with sprintf
     */
    filename = malloc(strlen(file_connection->path)+1
            +strlen(stream_info->stream_name)+1);
    if (filename == NULL) {
        RTI_RoutingServiceEnvironment_set_error(
                env,
                "Error allocating memory for filename in write function");
        return NULL;
    }

    sprintf(filename,"%s/%s",file_connection->path,stream_info->stream_name);

    file = fopen(filename, "r");
    if (file == NULL) {
        RTI_RoutingServiceEnvironment_set_error(
                env, "Could not open file %s for read",filename);
        free(filename);
        filename=NULL;
        return NULL;
    }
    free(filename);
    filename=NULL;

    /* Check that the type representation is DDS dynamic type code */
    if (stream_info->type_info.type_representation_kind !=
            RTI_ROUTING_SERVICE_TYPE_REPRESENTATION_DYNAMIC_TYPE) {

        RTI_RoutingServiceEnvironment_set_error(
                env, "Unsupported type format creating stream reader");
        fclose(file);
        return NULL;
    }

    /* Create the stream reader object */
    stream_reader = calloc(
            1, sizeof(struct RTI_RoutingServiceFileStreamReader));
    if (stream_reader == NULL) {
        RTI_RoutingServiceEnvironment_set_error(
                env, "Memory allocation error creating stream reader");
        fclose(file);
        return NULL;
    }

    stream_reader->connection = file_connection;
    stream_reader->samples_per_read = samples_per_read;
    stream_reader->read_period.sec =  read_period / 1000;
    stream_reader->read_period.nanosec = (read_period % 1000) * 1000000;
    stream_reader->file = file;
    stream_reader->listener = *listener;
    stream_reader->type_code =
            (struct DDS_TypeCode *) stream_info->type_info.type_representation;
    stream_reader->is_running_enabled = 1;
    stream_reader->info = stream_info;

    pthread_attr_init(&thread_attribute);
    pthread_attr_setdetachstate(&thread_attribute, PTHREAD_CREATE_JOINABLE);
    error = pthread_create(
            &stream_reader->tid,
            &thread_attribute,
            RTI_RoutingServiceFileStreamReader_run,
            (void *)stream_reader);
    pthread_attr_destroy(&thread_attribute);
    if (error) {
        RTI_RoutingServiceEnvironment_set_error(
                env, "Error creating thread for data_available notification");
        fclose(stream_reader->file);
        free(stream_reader);
        return NULL;
    }

    return (RTI_RoutingServiceStreamReader) stream_reader;
}

/*****************************************************************************/

void RTI_RoutingServiceFileConnection_delete_stream_reader(
        RTI_RoutingServiceConnection connection,
        RTI_RoutingServiceStreamReader stream_reader,
        RTI_RoutingServiceEnvironment * env)
{


    struct RTI_RoutingServiceFileStreamReader * self =
            (struct RTI_RoutingServiceFileStreamReader *) stream_reader;

    fprintf(stdout,"Connection: called function delete_stream_reader:%s\n",
            self->info->stream_name);

    self->is_running_enabled = 0;
    pthread_join(self->tid,NULL);

    RTI_RoutingServiceFileStreamReader_delete(self);

}

/*****************************************************************************/

RTI_RoutingServiceStreamWriter 
RTI_RoutingServiceFileConnection_create_stream_writer(
        RTI_RoutingServiceConnection connection,
        RTI_RoutingServiceSession session,
        const struct RTI_RoutingServiceStreamInfo * stream_info,
        const struct RTI_RoutingServiceProperties * properties,
        RTI_RoutingServiceEnvironment * env)
{

    struct RTI_RoutingServiceFileStreamWriter * stream_writer = NULL;


    const char * mode_property = NULL;
    const char * flush_property = NULL;
    char * filename = NULL;
    FILE * file = NULL;
    int flush_enabled = 0;
    int error = 0;
    struct RTI_RoutingServiceFileConnection* file_connection =
            (struct RTI_RoutingServiceFileConnection *) connection;

    fprintf(stdout,"Connection: called function create_stream_writer\n");

    mode_property = RTI_RoutingServiceProperties_lookup_property(
            properties, "WriteMode");
    if (mode_property != NULL) {
        if (strcmp(mode_property, "overwrite") &&
                strcmp(mode_property, "append") &&
                strcmp(mode_property, "keep") ) {
            RTI_RoutingServiceEnvironment_set_error(
                    env, "Invalid value for WriteMode (%s). "
                    "Allowed values: keep (default), overwrite, append",
                    mode_property);
            return NULL;
        }
    } else {
        mode_property = "keep";
    }

    /* Get the configuration properties in <route>/<output>/<property> */

    flush_property = RTI_RoutingServiceProperties_lookup_property(
            properties, "Flush");
    if (flush_property != NULL) {
        if (!strcmp(flush_property, "yes") || !strcmp(flush_property, "true") ||
                !strcmp(flush_property, "1")) {
            flush_enabled = 1;
        }
    }

    /* we prepare the string with the whole path */
    filename = malloc(strlen(file_connection->path)+1
            +strlen(stream_info->stream_name)+1);
    if (filename == NULL) {
        RTI_RoutingServiceEnvironment_set_error(env,"Error allocating memory"
                "for filename create_stream_writer");
    }
    sprintf(filename,"%s/%s",file_connection->path,stream_info->stream_name);

    /*
     * if property is keep we try to open the file in read mode, if the result
     * is not null, means that the file exists, and we cannot overwrite (keep)
     * so we don't create the stream writer
     */
    if (!strcmp(mode_property, "keep")) {
        file = fopen(filename, "r");
        if (file != NULL) {
            RTI_RoutingServiceEnvironment_set_error(
                    env, "File exists and WriteMode is keep");
            fclose(file);
            free(filename);
            return NULL;
        }
    }
    /*
     * if the first fopen returns null, means that the file doesn't exist, so
     * we can open it for writing.
     */
    file = fopen(filename, !strcmp(mode_property, "append") ? "a" : "w");
    if (file == NULL) {
        RTI_RoutingServiceEnvironment_set_error(
                env, "Could not open file for write: %s", filename);
        return NULL;
    }
    free(filename);
    stream_writer = calloc(
            1, sizeof(struct RTI_RoutingServiceFileStreamWriter));
    if (stream_writer == NULL) {
        RTI_RoutingServiceEnvironment_set_error(
                env, "Memory allocation error");
        fclose(file);
        return NULL;
    }

    stream_writer->file = file;
    stream_writer->flush_enabled = flush_enabled;
    stream_writer->info = stream_info;
    return stream_writer;
}

/*****************************************************************************/

void RTI_RoutingServiceFileConnection_delete_stream_writer(
        RTI_RoutingServiceConnection connection,
        RTI_RoutingServiceStreamWriter stream_writer,
        RTI_RoutingServiceEnvironment * env)
{

    struct RTI_RoutingServiceFileStreamWriter * self =
            (struct RTI_RoutingServiceFileStreamWriter *) stream_writer;

    fclose(self->file);
    fprintf(stdout,"Connection: called function delete_stream_writer:%s\n",
            self->info->stream_name);

    free(self);
}

/* ========================================================================= */
/*                                                                           */
/* AdapterPlugin methods                                                     */
/*                                                                           */
/* ========================================================================= */

RTI_RoutingServiceConnection
RTI_RoutingServiceFileAdapterPlugin_create_connection(
        struct RTI_RoutingServiceAdapterPlugin * adapter,
        const char * routing_service_name,
        const char * routing_service_group_name,
        const struct RTI_RoutingServiceStreamReaderListener * output_disc_listener,
        const struct RTI_RoutingServiceStreamReaderListener * input_disc_listener,
        const struct RTI_RoutingServiceTypeInfo ** registeredTypes,
        int    registeredTypeCount,
        const struct RTI_RoutingServiceProperties * properties,
        RTI_RoutingServiceEnvironment * env)
{
    const char * is_input_connection = NULL;
    const char * path = NULL;
    const char * sleep_period = NULL;
    struct RTI_RoutingServiceFileConnection * connection = NULL;

    fprintf(stdout,"FileAdapter: called function create_connection\n");

    connection = calloc(1, sizeof(struct RTI_RoutingServiceFileConnection));
    if (connection == NULL) {
        RTI_RoutingServiceEnvironment_set_error(env,"Error allocating memory"
                "for connection");
        return NULL;
    }

    sleep_period = RTI_RoutingServiceProperties_lookup_property(properties,
            "sleepPeriod");
    if (sleep_period == NULL) {
        connection->sleep_period = 5;
    }
    else {
        connection->sleep_period = atoi(sleep_period);
        if (connection->sleep_period <= 0) {
            RTI_RoutingServiceEnvironment_set_error(env,"Error creating "
                    "Connection: sleep_period value not valid");

        }
    }

    path = RTI_RoutingServiceProperties_lookup_property(
            properties,"path");
    /*
     * if the property is not found, then we are going to assign default value.
     * As the default value is different depending of the connection, so we
     * are going to assign the default value later, when we know which
     * connection this is
     */

    /* we recover properties from the xml file configuration */
    is_input_connection = RTI_RoutingServiceProperties_lookup_property(
            properties,"direction");
    /*
     * now we check if it is the connection that we use as
     * input connection or not
     */
    if ((is_input_connection != NULL) && !strcmp(is_input_connection,"input")) {
        fprintf(stdout,"Connection: This is an input connection\n");
        connection->is_input = 1;
        connection->is_running_enabled = 1;
        /*
         * we copy the listeners inside our connection,
         * not the pointers, but the contained structure
         */
        connection->input_discovery_listener = *input_disc_listener;


        if (path == NULL) {
            /* we assign the default value*/
            strcpy(connection->path,".");
        } else {
            strcpy(connection->path, path);
        }

    } else {
        /*either it is output, or we assign default as output*/
        fprintf(stdout,"Connection: This is an output connection\n");
        connection->is_input = 0;
        connection->is_running_enabled=0;
        if (path == NULL) {
            /* we assign the default value*/
            strcpy(connection->path,"./filewrite");
        } else {
            strcpy(connection->path, path);
        }
    }
    return connection;
}

/*****************************************************************************/

void RTI_RoutingServiceFileAdapterPlugin_delete_connection(
        struct RTI_RoutingServiceAdapterPlugin * adapter,
        RTI_RoutingServiceConnection connection,
        RTI_RoutingServiceEnvironment * env)
{

    struct RTI_RoutingServiceFileConnection *file_connection =
            (struct RTI_RoutingServiceFileConnection*) connection;

    fprintf(stdout,"FileAdapter: called function delete connection\n");

    if (file_connection->is_input) {
        RTI_RoutingServiceFileAdapter_delete_type_code(
                file_connection->input_discovery_reader->type_code);
    }

    /* delete input  discovery stream reader */
    if (file_connection->input_discovery_reader != NULL) {
        RTI_RoutingServiceFileStreamReader_delete(
                file_connection->input_discovery_reader);
    }

    free(file_connection);
}

/*****************************************************************************/

void RTI_RoutingServiceFileAdapterPlugin_delete(
        struct RTI_RoutingServiceAdapterPlugin * adapter,
        RTI_RoutingServiceEnvironment * env)
{
    fprintf(stdout,"RoutingService: called function delete plugin\n");
    free(adapter);
}

/*****************************************************************************/

/*
 * Discovery Functions gets called just once for every connection
 * That is why we delete them inside the
 */
RTI_RoutingServiceStreamReader
RTI_RoutingService_getInputDiscoveryReader(
        RTI_RoutingServiceConnection connection,
        RTI_RoutingServiceEnvironment *env)
{

    struct RTI_RoutingServiceFileStreamReader *stream_reader = NULL;
    struct RTI_RoutingServiceFileConnection *file_connection =
            (struct RTI_RoutingServiceFileConnection*) connection;

    fprintf(stdout,"Connection: called function getInputDiscoveryReader\n");

    if (file_connection->is_input) {
        stream_reader = calloc(1,
                sizeof(struct RTI_RoutingServiceFileStreamReader));
        if (stream_reader == NULL) {
            RTI_RoutingServiceEnvironment_set_error(
                    env, "Failure creating discovery stream_reader");
            return NULL;
        }
        /*
         * Once created the stream_reader we should delete assign him to
         * the connection, so once if we have some further problem, for
         * instance even in the creation of the typecode, the connection has
         * the reference to it to delete it
         */
        file_connection->input_discovery_reader = stream_reader;

        stream_reader->connection = file_connection;
        stream_reader->discovery_data_counter_read = 0;
        stream_reader->discovery_data_counter=0;
        stream_reader->type_code =
                RTI_RoutingServiceFileAdapter_create_type_code();
        if (stream_reader->type_code == NULL) {
            RTI_RoutingServiceEnvironment_set_error(
                    env, "Failure creating Typecode");
            free(stream_reader);
            stream_reader = NULL;
            return NULL;
        }
    }
    return (RTI_RoutingServiceStreamReader) stream_reader;
}

/* Entry point to the adapter plugin */
struct RTI_RoutingServiceAdapterPlugin * 
RTI_RoutingServiceFileAdapterPlugin_create(
        const struct RTI_RoutingServiceProperties * properties,
        RTI_RoutingServiceEnvironment * env)
{
    struct RTI_RoutingServiceFileAdapterPlugin * adapter = NULL;
    struct RTI_RoutingServiceVersion adapterVersion = {1,0,0,0};

    fprintf(stdout,"RoutingService: called function create_plugin\n");

    adapter = calloc(1, sizeof(struct RTI_RoutingServiceFileAdapterPlugin));
    if (adapter == NULL) {
        RTI_RoutingServiceEnvironment_set_error(
                env, "Memory allocation error creating plugin");
        return NULL;
    }

    RTI_RoutingServiceAdapterPlugin_initialize(&adapter->_base);
    adapter->_base.plugin_version = adapterVersion;

    /* delete plugin function */
    adapter->_base.adapter_plugin_delete =
            RTI_RoutingServiceFileAdapterPlugin_delete;

    /* connection functions */
    adapter->_base.adapter_plugin_create_connection =
            RTI_RoutingServiceFileAdapterPlugin_create_connection;
    adapter->_base.adapter_plugin_delete_connection =
            RTI_RoutingServiceFileAdapterPlugin_delete_connection;
    adapter->_base.connection_create_stream_reader =
            RTI_RoutingServiceFileConnection_create_stream_reader;
    adapter->_base.connection_delete_stream_reader =
            RTI_RoutingServiceFileConnection_delete_stream_reader;
    adapter->_base.connection_create_stream_writer =
            RTI_RoutingServiceFileConnection_create_stream_writer;
    adapter->_base.connection_delete_stream_writer =
            RTI_RoutingServiceFileConnection_delete_stream_writer;

    /* session functions */
    adapter->_base.connection_create_session =
            RTI_RoutingServiceFileConnection_create_session;
    adapter->_base.connection_delete_session =
            RTI_RoutingServiceFileConnection_delete_session;

    /* stream reader functions */
    adapter->_base.stream_reader_read =
            RTI_RoutingServiceFileStreamReader_read;
    adapter->_base.stream_reader_return_loan =
            RTI_RoutingServiceFileStreamReader_return_loan;

    /* stream writer functions */
    adapter->_base.stream_writer_write =
            RTI_RoutingServiceFileStreamWriter_write;

    /* discovery functions */
    adapter->_base.connection_get_input_stream_discovery_reader =
            RTI_RoutingService_getInputDiscoveryReader;

    return (struct RTI_RoutingServiceAdapterPlugin *) adapter;
}
#undef ROUTER_CURRENT_SUBMODULE

