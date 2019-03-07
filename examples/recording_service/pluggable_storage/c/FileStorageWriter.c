/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "ndds/ndds_c.h"
#include "recordingservice/recordingservice_storagewriter.h"

#include "FileStorageWriter.h"

#ifdef _WIN32
#define PRIu64 "I64u"
#define PRIi64 "I64i"
#else
#include <inttypes.h>
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define NANOSECS_PER_SEC 1000000000ll
#define FileStorageWriter_FILE_NAME_MAX 1024
#define FILENAME_PROPERTY_NAME "example.c_pluggable_storage.filename"

struct FileRecord {
    char file_name[FileStorageWriter_FILE_NAME_MAX];
    FILE *file;
};

struct PublicationFileStorageStreamWriter {
    struct RTI_RecordingServiceStorageStreamWriter as_storage_stream_writer;
};

struct FileStorageStreamWriter {
    FILE *file;
    uint64_t stored_sample_count;
    struct RTI_RecordingServiceStorageStreamWriter as_storage_stream_writer;
};

struct FileStorageWriter {
    struct FileRecord file;
    struct FileRecord info_file;
    struct RTI_RecordingServiceStorageWriter as_storage_writer;
};

/*
 * This stream writer store() method will be used by Recording Service to store
 * DCPSPublication samples.
 */
void FileStorageStreamWriter_store_publication(
        void *stream_writer_data,
        const DDS_PublicationBuiltinTopicData **samples,
        const RTI_RoutingServiceSampleInfo *sample_infos,
        const int count)
{
    RTI_UNUSED_PARAMETER(stream_writer_data);
    RTI_UNUSED_PARAMETER(samples);
    RTI_UNUSED_PARAMETER(sample_infos);
    RTI_UNUSED_PARAMETER(count);

    /*
     * There are specializations for each of the three built-in DDS discovery
     * topics in the API. Here we have shown how it would be like to implement
     * storage of the DCPSPublication data samples. The pattern for the other
     * types is the same.
     * Recording Service will call this method when new publication data samples
     * are received.
     */
}

/**
 * This function is called by Recorder whenever there are samples available for
 * one of the streams previously discovered and accepted (see the
 * FileStorageWriter_create_stream_writer() function below). Recorder provides
 * the samples and their associated information objects in Routing Service
 * format, this is, untyped format.
 * In our case we know that, except for the built-in DDS discovery topics which
 * are received in their own format - and that we're not really storing -, that
 * the format of the data we're receiving is DDS Dynamic Data. This will always
 * be the format received for types recorded from DDS.
 * The function traverses the collection of samples (of length defined by the
 * 'count' parameter) and stores the data in a textual format.
 */
void FileStorageStreamWriter_store(
        void *stream_writer_data,
        const RTI_RoutingServiceSample *samples,
        const RTI_RoutingServiceSampleInfo *sample_infos,
        const int count)
{
    struct FileStorageStreamWriter *stream_writer =
            (struct FileStorageStreamWriter *) stream_writer_data;
    const struct DDS_SampleInfo **sample_info_array =
            (const struct DDS_SampleInfo **) sample_infos;
    DDS_DynamicData **sampleArray = (DDS_DynamicData **) samples;
    int i = 0;

    for (; i < count; i++) {
        DDS_ReturnCode_t ret_code = DDS_RETCODE_OK;
        DDS_Long id = 0;
        char *msg = NULL;
        const struct DDS_SampleInfo *sample_info = sample_info_array[i];
        DDS_DynamicData *sample = sampleArray[i];
        DDS_Octet valid_data = sample_info->valid_data;
        int64_t timestamp =
                (int64_t) sample_info->reception_timestamp.sec
                * NANOSECS_PER_SEC;
        timestamp += sample_info->reception_timestamp.nanosec;

        fprintf(stream_writer->file,
                "Sample number: %"PRIu64"\n",
                stream_writer->stored_sample_count);
        fprintf(stream_writer->file,
                "Reception timestamp: %"PRIi64"\n",
                timestamp);
        fprintf(stream_writer->file, "Valid data: %u\n", valid_data);
        if (valid_data) {
            /* Get and store the sample's id field */
            ret_code = DDS_DynamicData_get_long(
                    sample,
                    &id,
                    "id",
                    DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
            if (ret_code != DDS_RETCODE_OK) {
                printf("Failed to find 'id' field in sample\n");
            }
            fprintf(stream_writer->file, "    Data.id: %i\n", id);
            /* Get and store the sample's msg field */
            ret_code = DDS_DynamicData_get_string(
                    sample,
                    &msg,
                    NULL,
                    "msg",
                    DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
            if (ret_code != DDS_RETCODE_OK) {
                printf("Failed to find 'msg' field in sample\n");
            }
            fprintf(stream_writer->file, "    Data.msg: %s\n", msg);
            DDS_String_free(msg);
        }
        stream_writer->stored_sample_count++;
    }
    fflush(stream_writer->file);
}

/**
 * Initialize this instance of a StreamWriter. See that we're setting ourselves
 * in the stream_writer_data field for later easy access via simple cast.
 */
int FileStorageStreamWriter_initialize(
        struct FileStorageStreamWriter *stream_writer,
        FILE *file)
{
    RTI_RecordingServiceStorageStreamWriter_initialize(
            &stream_writer->as_storage_stream_writer);

    /* init implementation */
    stream_writer->as_storage_stream_writer.store = FileStorageStreamWriter_store;
    stream_writer->as_storage_stream_writer.stream_writer_data = stream_writer;

    stream_writer->file = file;
    stream_writer->stored_sample_count = 0;

    return TRUE;
}

/******************************************************************************/

/**
 * Internal function used by this plugin called upon creation and initialization
 * of the plugin instance.This is the place where the storage space should
 * be created and/or opened.
 * In our case, we open the file that will hold the data samples we record and
 * a meta-data file (it uses the same file name but we append a '.info' at the
 * end) that we'll use to mark the start and end timestamps of the recording.
 */
int FileStorageWriter_connect(void *storage_writer_data)
{
    struct FileStorageWriter *writer =
            (struct FileStorageWriter *) storage_writer_data;
    int64_t current_time = -1;

    writer->file.file = fopen(writer->file.file_name, "w");
    if (writer->file.file == NULL) {
        printf("%s: %s\n",
                "Failed to open file for writing",
                writer->file.file_name);
        return FALSE;
    }
    strcpy(writer->info_file.file_name, writer->file.file_name);
    strcat(writer->info_file.file_name, ".info");
    writer->info_file.file = fopen(writer->info_file.file_name, "w");
    if (writer->info_file.file == NULL) {
        printf("%s: %s\n",
                "Failed to open file for writing",
                writer->info_file.file_name);
        /* Cleanup if error */
        fclose(writer->file.file);
        return FALSE;
    }
    /* Obtain current time */
    current_time = (int64_t) time(NULL);
    if (current_time == -1) {
        fprintf(stderr, "Failed to obtain the current time\n");
        /* Cleanup if error */
        fclose(writer->file.file);
        fclose(writer->info_file.file);
        return FALSE;
    }
    /* Time was returned in seconds. Transform to nanoseconds */
    current_time *= NANOSECS_PER_SEC;
    fprintf(writer->info_file.file,
            "Start timestamp: %"PRIi64"\n",
            current_time);
    return TRUE;
}

/**
 * Internal function used by our plugin to disconnect from the storage resources
 * that have been allocated (e.g. files).
 * For this example's purposes, we need to close the main data file, and then
 * we get the current timestamp from the system and store it in the meta-data
 * file. Then we also close this meta-data file.
 */
int FileStorageWriter_disconnect(void *storage_writer_data)
{
    struct FileStorageWriter *writer =
            (struct FileStorageWriter *) storage_writer_data;
    int64_t current_time = -1;

    if (fclose(writer->file.file) != 0) {
        perror("Failed to close output file");
        return FALSE;
    }
    /* Obtain current time */
    current_time = (int64_t) time(NULL);
    if (current_time == -1) {
        fprintf(stderr, "Failed to obtain the current time\n");
    }
    /* Time was returned in seconds. Transform to nanoseconds */
    current_time *= NANOSECS_PER_SEC;
    fprintf(writer->info_file.file, "End timestamp: %"PRIi64"\n", current_time);
    if (fclose(writer->info_file.file) != 0) {
        perror("Failed to close output info file");
        return FALSE;
    }
    return TRUE;
}

/**
 * This function is called by Recorder whenever a new DDS discovery stream has
 * been discovered. Recorder will ask this plugin to create a
 * StorageStreamWriter object attached to the discovery stream.
 * Recorder calls this method only for the DDS discovery streams:
 * DCPSParticipant, DCPSPublication and DCPSSubscription. Note that the type
 * received in this case is not a Dynamic Data object, but a pointer to the
 * respective specific discovery topic types (DDS_ParticipantBuiltinTopicData,
 * DDS_PublicationBuitinTopicData and DDS_SubscriptionBuiltinTopicData).
 * For our example's purpose, we don't actually need to store the discovery data
 * because we don't need any information from it later. However, also for
 * example purposes, we create a different object than the one that's created in
 * the case of the user-data creation function.
 */
struct RTI_RecordingServiceStoragePublicationWriter *
FileStorageWriter_create_publication_writer(void *storage_writer_data)
{
    struct RTI_RecordingServiceStoragePublicationWriter *stream_writer = NULL;

    RTI_UNUSED_PARAMETER(storage_writer_data);

    stream_writer = malloc(sizeof(
            struct RTI_RecordingServiceStoragePublicationWriter));
    if (stream_writer == NULL) {
        printf("Failed to allocate RTI_RecordingServiceStoragePublicationWriter "
                "instance\n");
        return NULL;
    }
    RTI_RecordingServiceStoragePublicationWriter_initialize(stream_writer);
    /* init implementation */
    stream_writer->store = FileStorageStreamWriter_store_publication;
    stream_writer->stream_writer_data = stream_writer;
    return NULL;
}

/**
 * This function is called by Recorder whenever a new user data stream has been
 * discovered. Recorder will ask this plugin to create a StorageStreamWriter
 * object attached to the stream. This function is in charge of preparing any
 * context necessary for the stream writer to store data directly. In this
 * example, the file to store the samples to is provided to the stream writer on
 * initialization.
 * Generally, this function should accept every stream and type. Stream name
 * (topic) and type name filters should be applied at configuration level. The
 * goal of the create_stream_writer() function is to prepare the storage for the
 * reception of samples of a previously undiscovered type, and to create a
 * StreamWriter that can store those samples.
 * This function receives a set of properties as a parameter. Recorder will
 * provide some built-in properties in this set, like the DDS domain ID the
 * stream was found in (as a 32-bit integer in text format).
 * For simplification purposes, in this example we just accept this type: the
 * HelloMsg topic/type defined in the example.
 */
struct RTI_RecordingServiceStorageStreamWriter *
FileStorageWriter_create_stream_writer(
        void *storage_writer_data,
        const struct RTI_RoutingServiceStreamInfo *stream_info,
        const struct RTI_RoutingServiceProperties *properties)
{
    struct FileStorageWriter *writer =
            (struct FileStorageWriter *) storage_writer_data;
    struct FileStorageStreamWriter *stream_writer = NULL;

    /*
     * We're not using any of the provided properties in this example, but if
     * you need to do so, you can use the
     * RTI_RoutingServiceProperties_lookup_property() function to obtain a
     * property's value. The built-in properties passed by Recorder can be found
     * in the storage_writer.h file
     * (e.g. RTI_RECORDING_SERVICE_DOMAIN_ID_PROPERTY_NAME for the property
     * holding the associated DDS domain ID value). In order to obtain the
     * numeric value, just parse the text integer normally (e.g. by using
     * strtol(). See the reading side example's
     * FileStorageReader_create_stream_reader() function to see the code in
     * action.
     */
    RTI_UNUSED_PARAMETER(properties);

    if (strcmp(stream_info->type_info.type_name, "HelloMsg") == 0) {
        stream_writer = malloc(sizeof(struct FileStorageStreamWriter));
        if (stream_writer == NULL) {
            printf("Failed to allocate FileStorageStreamWriter instance\n");
            return NULL;
        }
        if (!FileStorageStreamWriter_initialize(
                stream_writer,
                writer->file.file)) {
            free(stream_writer);
            printf("Failed to initialize FileStorageStreamWriter instance\n");
            return NULL;
        }
        return &stream_writer->as_storage_stream_writer;
    }
    return NULL;
}

/**
 * The plugin is also in charge of deleting the StorageStreamWriter instances it
 * has created. It's a good practice to use the stream_writer_data field in the
 * RTI_RecordingServiceStorageStreamWriter to store the specific instance we
 * created, so we can use it directly if necessary with a simple cast.
 * In the case of this example, we're setting the created StreamWriter instances
 * in this field so we can call free() on that field directly.
 */
void FileStorageWriter_delete_stream_writer(
        void *storage_writer_data,
        struct RTI_RecordingServiceStorageStreamWriter *stream_writer)
{
    RTI_UNUSED_PARAMETER(storage_writer_data);
    /* We always assign the allocated instance to the stream_writer_data holder.
     * Thus, free that directly. */
    free(stream_writer->stream_writer_data);
}

/**
 * This function initializes the StorageWriter plugin with the adequate
 * functions. These plugin functions will be used by Recorder's C-wrapping
 * class.
 * This function depends on a property passed in the Recorder XML
 * configuration's <property> tag (see the FILENAME_PROPERTY_NAME constant
 * above), and it will fail if it's not found. It has been added to the example
 * XML file provided (pluggable_storage_example.xml).
 * This function, however, is not in charge of opening that file. That's done
 * in the FileStorageWriter_connect() function defined above. The same way,
 * the file is closed in the FileStorageWriter_disconnect() function above.
 */
int FileStorageWriter_initialize(
        struct FileStorageWriter * writer,
        const struct RTI_RoutingServiceProperties *properties)
{
    const char *file_name = NULL;

    RTIOsapiMemory_zero(writer, sizeof(struct FileStorageWriter));

    RTI_RecordingServiceStorageWriter_initialize(&writer->as_storage_writer);

    /* init implementation */
    writer->as_storage_writer.create_stream_writer =
            FileStorageWriter_create_stream_writer;
    writer->as_storage_writer.create_publication_writer =
            FileStorageWriter_create_publication_writer;
    writer->as_storage_writer.delete_stream_writer =
            FileStorageWriter_delete_stream_writer;
    writer->as_storage_writer.delete_instance =
            FileStorageWriter_delete_instance;
    writer->as_storage_writer.storage_writer_data = writer;

    file_name = RTI_RoutingServiceProperties_lookup_property(
            properties,
            FILENAME_PROPERTY_NAME);
    if (file_name == NULL) {
        printf("Failed to find property with name=%s\n",
                FILENAME_PROPERTY_NAME);
        /* Cleanup on failure */
        free(writer);
        return FALSE;
    }
    if (strlen(file_name) >= FileStorageWriter_FILE_NAME_MAX) {
        printf("File name too long (%s)\n", file_name);
        /* Cleanup on failure */
        free(writer);
        return FALSE;
    }
    strcpy(writer->file.file_name, file_name);

    if (!FileStorageWriter_connect(writer)) {
        printf("Failed to connect to storage\n");
        free(writer);
        return FALSE;
    }
    return TRUE;
}

void FileStorageWriter_delete_instance(
        struct RTI_RecordingServiceStorageWriter *storageWriter)
{
    struct FileStorageWriter *writer =
            (struct FileStorageWriter *) storageWriter->storage_writer_data;
    if (!FileStorageWriter_disconnect(writer)) {
        printf("Failed to disconnect from storage\n");
    }
    free(writer);
}

/**
 * Create and initialize an instance of type FileStorageWriter. This type will
 * handle management of the database files and storage of the received samples.
 * Note: the example only works with samples of the provided HelloMsg type,
 * described in file HelloMsg.idl.
 */
struct RTI_RecordingServiceStorageWriter * FileStorageWriter_create_instance(
        const struct RTI_RoutingServiceProperties *properties)
{
    struct FileStorageWriter * writer = NULL;

    writer = malloc(sizeof(struct FileStorageWriter));
    if (writer == NULL) {
        printf("Failed to allocate FileStorageWriter instance\n");
        return NULL;
    }
    if (!FileStorageWriter_initialize(writer, properties)) {
        printf("Failed to initialize FileStorageWriter instance\n");
        FileStorageWriter_delete_instance(&writer->as_storage_writer);
        return NULL;
    }
    return &writer->as_storage_writer;
}
