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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "ndds/ndds_c.h"
#include "routingservice/routingservice_infrastructure.h"
#include "recordingservice/recordingservice_storagereader.h"

#include "HelloMsg.h"
#include "FileStorageReader.h"

#define NANOSECS_PER_SEC 1000000000

#ifdef _WIN32
#define SCNi64 "I64i"
#define SCNu64 "I64u"
#else
#include <inttypes.h>
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

struct FileRecord {
    char fileName[FILENAME_MAX];
    FILE *file;
};

struct FileStorageStreamReader {
    struct FileRecord file_record;
    int64_t current_timestamp;
    int current_valid_data;
    DDS_Long current_data_id;
    char current_data_msg[256];
    struct DDS_DynamicDataSeq taken_data;
    struct DDS_SampleInfoSeq taken_info;
    DDS_TypeCode *type_code;
    struct RTI_RecordingServiceStorageStreamReader as_stream_reader;
};

struct FileStorageStreamInfoReader {
    struct FileRecord file_record;
    int32_t domain_id;
    struct RTI_RoutingServiceStreamInfo *stream_info;
    int stream_info_taken;
    struct FileRecord info_file_record;
    struct RTI_RecordingServiceStorageStreamInfoReader base_discovery_stream_reader;
};

#define FileStorageReader_FILE_NAME_MAX 1024

struct FileStorageReader {
    char file_name[FileStorageReader_FILE_NAME_MAX];
    struct RTI_RecordingServiceStorageReader as_storage_reader;
};

#define FILENAME_PROPERTY_NAME "example.c_pluggable_storage.filename"

#pragma warning(disable : 4996)

/******************************************************************************/

/**
 * Replay will periodically ask for any newly discovered data streams.
 * This function receives a time limit parameter. It should return any stream
 * info event not having been taken yet and within the given time limit
 * (associated time of the event should be less or equal to the time limit).
 * In our example we only store one topic and type, described in the provided
 * IDL file (HelloMsg.idl). We simulate we discover that stream in the very
 * first call to this function. Every other call to this function will return
 * an empty count of taken elements.
 */
void FileStorageStreamInfoReader_read(
        void *stream_reader_data,
        struct RTI_RoutingServiceStreamInfo ***stream_info_array,
        int *count,
        const struct RTI_RecordingServiceSelectorState *selector)
{
    struct FileStorageStreamInfoReader *stream_reader =
            (struct FileStorageStreamInfoReader *) stream_reader_data;

    RTI_UNUSED_PARAMETER(selector);

    if (!stream_reader->stream_info_taken) {
        *stream_info_array = &stream_reader->stream_info;
        *count = 1;
        stream_reader->stream_info_taken = TRUE;
    } else {
        *count = 0;
    }
}

void FileStorageStreamInfoReader_return_loan(
        void *stream_reader_data,
        struct RTI_RoutingServiceStreamInfo **stream_info_array,
        const int count)
{
    RTI_UNUSED_PARAMETER(stream_reader_data);
    RTI_UNUSED_PARAMETER(stream_info_array);
    RTI_UNUSED_PARAMETER(count);
}

/**
 * Replay and Converter need to know the initial and final timestamps of the
 * recording being read.
 * In the case of our example, this information is stored in the associated
 * '.info' file.
 */
long long FileStorageStreamInfoReader_get_service_start_time(
        void *stream_reader_data)
{
    struct FileStorageStreamInfoReader *stream_reader =
            (struct FileStorageStreamInfoReader *) stream_reader_data;
    int64_t timestamp = 0;

    if (fscanf(stream_reader->info_file_record.file,
            "Start timestamp: %"SCNi64"\n",
            &timestamp) != 1) {
        printf("Failed to read start timestamp from info file\n");
    }
    return timestamp;
}

/**
 * Replay and Converter need to know the initial and final timestamps of the
 * recording being read.
 * In the case of our example, this information is stored in the associated
 * '.info' file.
 */
long long FileStorageStreamInfoReader_get_service_stop_time(
        void *stream_reader_data)
{
    struct FileStorageStreamInfoReader *stream_reader =
            (struct FileStorageStreamInfoReader *) stream_reader_data;
    int64_t timestamp = 0;

    if (fscanf(stream_reader->info_file_record.file,
            "End timestamp: %"SCNi64"\n",
            &timestamp) != 1) {
        printf("Failed to read end timestamp from info file\n");
    }
    return timestamp;
}

void FileStorageStreamInfoReader_reset(void *stream_reader_data)
{
    struct FileStorageStreamInfoReader *stream_reader =
            (struct FileStorageStreamInfoReader *) stream_reader_data;

    stream_reader->stream_info_taken = FALSE;
}

int FileStorageStreamInfoReader_finished(void *stream_reader_data)
{
    struct FileStorageStreamInfoReader *stream_reader =
            (struct FileStorageStreamInfoReader *) stream_reader_data;
    return stream_reader->stream_info_taken;
}

/* Discovery */
/* Discovery information: Implementing these methods will allow the Replay
 * Service to scan the discovery information from storage, and create the
 * user data streams
 */
int FileStorageStreamInfoReader_initialize(
        struct FileStorageStreamInfoReader *stream_reader,
        const char *fileName)
{
    if (fileName == NULL) {
        printf("%s: null file passed as a parameter\n", RTI_FUNCTION_NAME);
        return FALSE;
    }

    RTI_RecordingServiceStorageStreamInfoReader_initialize(
            &stream_reader->base_discovery_stream_reader);
    stream_reader->base_discovery_stream_reader.read =
            FileStorageStreamInfoReader_read;
    stream_reader->base_discovery_stream_reader.return_loan =
            FileStorageStreamInfoReader_return_loan;
    stream_reader->base_discovery_stream_reader.get_service_start_time =
            FileStorageStreamInfoReader_get_service_start_time;
    stream_reader->base_discovery_stream_reader.get_service_stop_time =
            FileStorageStreamInfoReader_get_service_stop_time;
    stream_reader->base_discovery_stream_reader.reset =
            FileStorageStreamInfoReader_reset;
    stream_reader->base_discovery_stream_reader.finished =
            FileStorageStreamInfoReader_finished;
    stream_reader->base_discovery_stream_reader.stream_reader_data =
            stream_reader;

    stream_reader->file_record.file = fopen(fileName, "r");
    if (stream_reader->file_record.file == NULL) {
        perror("Failed to open file");
        return FALSE;
    }
    strcpy(stream_reader->file_record.fileName, fileName);
    strcpy(stream_reader->info_file_record.fileName, fileName);
    strcat(stream_reader->info_file_record.fileName, ".info");
    stream_reader->info_file_record.file = fopen(
            stream_reader->info_file_record.fileName, "r");
    if (stream_reader->info_file_record.file == NULL) {
        perror("Failed to open info file");
        return FALSE;
    }

    stream_reader->domain_id = 0;
    stream_reader->stream_info = RTI_RoutingServiceStreamInfo_new_discovered(
            "Example_C_Storage",
            "HelloMsg",
            RTI_ROUTING_SERVICE_TYPE_REPRESENTATION_DYNAMIC_TYPE,
            HelloMsg_get_typecode());
    if (stream_reader->stream_info == NULL) {
        printf("Failed to create StreamInfo object for stream\n");
        return FALSE;
    }
    stream_reader->stream_info->partition.element_array = NULL;
    stream_reader->stream_info->partition.element_count = 0;
    stream_reader->stream_info->partition.element_count_max = 0;
    stream_reader->stream_info_taken = FALSE;
    return TRUE;
}

/*
 * --- User-data StreamReader -------------------------------------------------------
 */

/**
 * The example implementation of a stream reader caches the current values read
 * from the data file. This function reads the next data and info values from
 * the opened file.
 */
int FileStorageStreamReader_readSample(
        struct FileStorageStreamReader *stream_reader)
{
    uint64_t sampleNr = 0;

    if (feof(stream_reader->file_record.file)) {
        return FALSE;
    }
    if (fscanf(
            stream_reader->file_record.file,
            "Sample number: %"SCNu64"\n",
            &sampleNr) != 1) {
        printf("Failed to read sample number from file\n");
        return FALSE;
    }
    if (fscanf(stream_reader->file_record.file,
            "Reception timestamp: %"SCNi64"\n",
            &stream_reader->current_timestamp) != 1) {
        printf("Failed to read current timestamp from file\n");
        return FALSE;
    }
    if (fscanf(stream_reader->file_record.file,
            "Valid data: %u\n",
            &stream_reader->current_valid_data) != 1) {
        printf("Failed to read current valid data flag from file\n");
        return FALSE;
    }
    if (stream_reader->current_valid_data) {
        if (fscanf(stream_reader->file_record.file,
                "    Data.id: %i\n",
                &stream_reader->current_data_id) != 1) {
            printf("Failed to read current data.id field from file\n");
            return FALSE;
        }
        if (fscanf(stream_reader->file_record.file,
                "    Data.msg: %256[^\n]\n",
                stream_reader->current_data_msg) != 1) {
            printf("Failed to read current data.msg field from file\n");
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * When the next sample cached fulfills the condition to be taken (its timestamp
 * is within the provided limit) this method adds it to the sequence used to
 * store the current taken samples.
 */
int FileStorageStreamReader_addSampleToData(
        struct FileStorageStreamReader *stream_reader)
{
    DDS_Long current_length = DDS_DynamicDataSeq_get_length(
            &stream_reader->taken_data);
    DDS_Long target_length = current_length + 1;
    DDS_Long current_maximum = DDS_DynamicDataSeq_get_maximum(
            &stream_reader->taken_data);
    DDS_DynamicData *current_data = NULL;
    struct DDS_SampleInfo *current_info = NULL;
    DDS_ReturnCode_t ret_code = DDS_RETCODE_OK;

    /* Every time the targeted length exceeds the current sequence capacity
     * (maximum), we double the capacity */
    if (current_maximum < target_length) {
        if (!DDS_DynamicDataSeq_set_maximum(
                &stream_reader->taken_data,
                2 * current_maximum)) {
            printf("Failed to set new capacity (%d) for data sequence\n",
                    2 * current_maximum);
            return FALSE;
        }
        if (!DDS_SampleInfoSeq_set_maximum(
                &stream_reader->taken_info,
                2 * current_maximum)) {
            printf("Failed to set new capacity (%d) for info sequence\n",
                    2 * current_maximum);
            return FALSE;
        }
    }
    /* Extend the sequences' length, we know that the current maximum will allow
     * us to hold the length */
    DDS_DynamicDataSeq_set_length(&stream_reader->taken_data, target_length);
    DDS_SampleInfoSeq_set_length(&stream_reader->taken_info, target_length);
    /* Get the current dynamic data object */
    current_data = DDS_DynamicDataSeq_get_reference(
            &stream_reader->taken_data,
            current_length);
    DDS_DynamicData_finalize(current_data);
    DDS_DynamicData_initialize(
            current_data,
            stream_reader->type_code,
            &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    /* Get the current sample info object */
    current_info = DDS_SampleInfoSeq_get_reference(
            &stream_reader->taken_info,
            current_length);
    current_info->reception_timestamp.sec =
            (DDS_Long) (stream_reader->current_timestamp
                    / (int64_t) NANOSECS_PER_SEC);
    current_info->reception_timestamp.nanosec =
            (DDS_Long) (stream_reader->current_timestamp
                    % (int64_t) NANOSECS_PER_SEC);
    current_info->valid_data = (stream_reader->current_valid_data ?
            DDS_BOOLEAN_TRUE :
            DDS_BOOLEAN_FALSE);
    if (stream_reader->current_valid_data) {
        /* Set the current data ID */
        ret_code = DDS_DynamicData_set_long(
                current_data,
                "id",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
                stream_reader->current_data_id);
        if (ret_code != DDS_RETCODE_OK) {
            /* Not a show-stopper */
            printf("Failed to set data.id member value\n");
        }
        ret_code = DDS_DynamicData_set_string(
                current_data,
                "msg",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
                stream_reader->current_data_msg);
        if (ret_code != DDS_RETCODE_OK) {
            /* Not a show-stopper */
            printf("Failed to set data.msg member value\n");
        }
    }
    /* Clear the cached values until we read the next sample */
    stream_reader->current_timestamp = INT64_MAX;
    stream_reader->current_valid_data = FALSE;
    stream_reader->current_data_id = 0;
    stream_reader->current_data_msg[0] = '\0';
    return TRUE;
}

/**
 * Replay or Converter will call this stream reader function when asking for the
 * next batch of data samples that should be provided to the output connection.
 * The function receives a pointer to a collection of samples and a pointer to
 * a collection of their respective sample information objects - both in Routing
 * Service types, this is, untyped. More interestingly, the function receives a
 * timestamp limit parameter. The function should provide any data not taken
 * yet for which the associated timestamp is less or equal to this timestamp
 * limit. This is especially important for Replay, because respecting time
 * separation between samples requires this functionality to be implemented
 * correctly.
 * Replay and Converter expect to receive Dynamic Data samples for the data
 * objects and DDS_SampleInfo objects for the associated sample information
 * objects.
 */
void FileStorageStreamReader_read(
        void *stream_reader_data,
        RTI_RoutingServiceSample **out_samples,
        RTI_RoutingServiceSampleInfo **out_sample_infos,
        int *count,
        const struct RTI_RecordingServiceSelectorState *selector)
{
    struct FileStorageStreamReader *stream_reader =
            (struct FileStorageStreamReader *) stream_reader_data;
    int i = 0;
    long long timestamp_limit =
            (long long) selector->time_range_end.sec * NANOSECS_PER_SEC
            + selector->time_range_end.nanosec;

    if (stream_reader->current_timestamp > timestamp_limit) {
        *count = 0;
        return;
    }
    /* Add the currently read sample and sample info values to the taken data
     * and info collections (sequences) */
    do {
        FileStorageStreamReader_addSampleToData(stream_reader);
        FileStorageStreamReader_readSample(stream_reader);
    } while (stream_reader->current_timestamp <= timestamp_limit);
    /* The number of taken samples is the current length of the data sequence */
    *count = (int) DDS_DynamicDataSeq_get_length(&stream_reader->taken_data);

    *out_samples = malloc(*count * sizeof(RTI_RoutingServiceSample));
    if (*out_samples == NULL) {
        printf("Failed to allocate sample array\n");
        *count = 0;
        return;
    }
    for (i = 0; i < *count; i++) {
        (*out_samples)[i] = DDS_DynamicDataSeq_get_reference(
                &stream_reader->taken_data,
                i);
    }

    *out_sample_infos = malloc(*count * sizeof(RTI_RoutingServiceSampleInfo));
    if (*out_sample_infos == NULL) {
        printf("Failed to allocate sample info array\n");
        *count = 0;
        return;
    }
    for (i = 0; i < *count; i++) {
        (*out_sample_infos)[i] = DDS_SampleInfoSeq_get_reference(
                &stream_reader->taken_info,
                i);
    }
}

void FileStorageStreamReader_return_loan(
            void *stream_reader_data,
            RTI_RoutingServiceSample *samples,
            RTI_RoutingServiceSampleInfo *sample_infos,
            int count)
{
    struct FileStorageStreamReader *stream_reader =
            (struct FileStorageStreamReader *) stream_reader_data;

    RTI_UNUSED_PARAMETER(count);

    DDS_DynamicDataSeq_set_length(&stream_reader->taken_data, 0);
    DDS_SampleInfoSeq_set_length(&stream_reader->taken_info, 0);

    free(samples);
    free(sample_infos);
}

int FileStorageStreamReader_finished(void *stream_reader_data)
{
    struct FileStorageStreamReader *stream_reader =
            (struct FileStorageStreamReader *) stream_reader_data;

    if (feof(stream_reader->file_record.file)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void FileStorageStreamReader_reset(void *stream_reader_data)
{
    struct FileStorageStreamReader *stream_reader =
            (struct FileStorageStreamReader *) stream_reader_data;

    fseek(stream_reader->file_record.file, 0, SEEK_SET);
}

int FileStorageStreamReader_initialize(
        struct FileStorageStreamReader *stream_reader,
        const char *file_name,
        const struct RTI_RoutingServiceStreamInfo *stream_info,
        int domain_id)
{
    RTI_UNUSED_PARAMETER(domain_id);

    if (stream_info->stream_name == NULL ) {
        return FALSE;
    }

    strcpy(stream_reader->file_record.fileName, file_name);
    stream_reader->file_record.file = fopen(
            stream_reader->file_record.fileName,
            "r");
    if (stream_reader->file_record.file == NULL) {
        return FALSE;
    }

    /* Initialize the data and info holders (sequences)
     * Reserve space for at least 1 element, initially */
    DDS_DynamicDataSeq_initialize(&stream_reader->taken_data);
    DDS_SampleInfoSeq_initialize(&stream_reader->taken_info);
    if (!DDS_DynamicDataSeq_set_maximum(
            &stream_reader->taken_data,
            1)) {
        printf("Failed to set new capacity (1) for data sequence\n");
        return FALSE;
    }
    if (!DDS_SampleInfoSeq_set_maximum(
            &stream_reader->taken_info,
            1)) {
        printf("Failed to set new capacity (1) for info sequence\n");
        return FALSE;
    }
    stream_reader->type_code =
            (DDS_TypeCode *) stream_info->type_info.type_representation;
    /* Bootstrap the take loop: read the first sample */
    if (!FileStorageStreamReader_readSample(stream_reader)) {
        printf("Failed to get first sample from file, maybe EOF was reached\n");
    }

    RTI_RecordingServiceStorageStreamReader_initialize(
            &stream_reader->as_stream_reader);
    stream_reader->as_stream_reader.read = FileStorageStreamReader_read;
    stream_reader->as_stream_reader.return_loan =
            FileStorageStreamReader_return_loan;
    stream_reader->as_stream_reader.finished =
            FileStorageStreamReader_finished;
    stream_reader->as_stream_reader.reset =
            FileStorageStreamReader_reset;
    stream_reader->as_stream_reader.stream_reader_data = stream_reader;

    return TRUE;
}

/**
 * Delete an instance of a data stream reader created by this plugin.
 * Close any file handles opened and free any allocated resources and memory.
 */
void FileStorageReader_delete_stream_reader(
        void *storage_reader_data,
        struct RTI_RecordingServiceStorageStreamReader *stream_reader)
{
    struct FileStorageStreamReader *file_stream_reader =
            (struct FileStorageStreamReader *)
                stream_reader->stream_reader_data;

    RTI_UNUSED_PARAMETER(storage_reader_data);

    if (fclose(file_stream_reader->file_record.file) != 0) {
        perror("Error closing replay C plugin file: ");
    } else {
        file_stream_reader->file_record.file = NULL;
    }
    DDS_DynamicDataSeq_finalize(&file_stream_reader->taken_data);
    DDS_SampleInfoSeq_finalize(&file_stream_reader->taken_info);
    free(file_stream_reader);
}

/**
 * Create a data stream reader. For each discovered stream that matches the set
 * of interest defined in the configuration, Replay or Converter will ask us to
 * create a reader for that stream (a stream reader).
 * The start and stop timestamp parameters define the time range for which the
 * application is asking for data. For simplicity, this example doesn't do
 * anything with these parameters. But the correct way to implement this API
 * would be to not read any data recorded before the given start time or after
 * the given end time.
 */
struct RTI_RecordingServiceStorageStreamReader *
FileStorageReader_create_stream_reader(
        void *storage_reader_data,
        const struct RTI_RoutingServiceStreamInfo *stream_info,
        const struct RTI_RoutingServiceProperties *properties)
{
    struct FileStorageReader *storage_reader =
            (struct FileStorageReader *) storage_reader_data;
    struct FileStorageStreamReader *stream_reader = NULL;
    int domain_id = 0;
    /*
     * Look up the 'rti.recording_service.domain_id' property in the property
     * set. Attempt to parse the value as an integer.
     */
    const char *domain_id_str = RTI_RoutingServiceProperties_lookup_property(
            properties,
            RTI_RECORDING_SERVICE_DOMAIN_ID_PROPERTY_NAME);
    if (domain_id_str == NULL) {
        printf("%s: could not find %s property in property set\n",
                RTI_FUNCTION_NAME,
                RTI_RECORDING_SERVICE_DOMAIN_ID_PROPERTY_NAME);
    }
    errno = 0;
    domain_id = strtol(domain_id_str, NULL, 10);
    if (errno == ERANGE) {
        printf("Failed to parse domain ID from string: '%s'. Range error.\n",
                domain_id_str);
    }
    stream_reader = (struct FileStorageStreamReader *)
            malloc(sizeof(struct FileStorageStreamReader));
    if (stream_reader == NULL) {
        printf("Failed to allocate FileStorageStreamReader instance\n");
        return NULL;
    }
    if (!FileStorageStreamReader_initialize(
            stream_reader,
            storage_reader->file_name,
            stream_info,
            domain_id)) {
        printf("%s: !init %s\n",
                RTI_FUNCTION_NAME,
                "FileStorageStreamReader");
        FileStorageReader_delete_stream_reader(
                storage_reader_data,
                &stream_reader->as_stream_reader);
        return NULL;
    }
    return &stream_reader->as_stream_reader;
}

/**
 * Delete an instance of a discovery stream reader created by this plugin. Close
 * any opened files and return the allocated memory and resources.
 */
void FileStorageReader_delete_stream_info_reader(
        void *storage_reader_data,
        struct RTI_RecordingServiceStorageStreamInfoReader * stream_reader)
{
    struct FileStorageStreamInfoReader *example_stream_reader =
            (struct FileStorageStreamInfoReader *)
                    stream_reader->stream_reader_data;

    RTI_UNUSED_PARAMETER(storage_reader_data);

    RTI_RoutingServiceStreamInfo_delete(example_stream_reader->stream_info);

    if (example_stream_reader != NULL) {
        if (example_stream_reader->file_record.file != NULL) {
            if (fclose(example_stream_reader->file_record.file) != 0) {
                perror("Error closing replay C plugin file:");
            } else {
                example_stream_reader->file_record.file = NULL;
            }
        }
        if (example_stream_reader->info_file_record.file != NULL) {
            if (fclose(example_stream_reader->info_file_record.file) != 0) {
                perror("Error closing replay C plugin info file:");
            } else {
                example_stream_reader->info_file_record.file = NULL;
            }
        }
    } else {
        printf("Error: invalid DB discovery stream reader instance\n");
    }
    free(example_stream_reader);
}

/**
 * Create a discovery stream reader. This function is called by Replay or
 * Converter when the connection between input and output is being created. The
 * discovery stream reader is in charge or traversing the storage looking for
 * stored data streams to be accessed.
 * A set of properties is passed. It contains built-in properties like the
 * start and stop timestamps, in the form of strings (64-bit integers as text).
 * The start and stop timestamp parameters define the time range for which the
 * application is asking for data. To simplify the example, we have not made any
 * use of these parameters, but the correct way to implement a discovery
 * stream reader would be to use them to constrain the streams found to that
 * specific time range (streams found before the start time or after the end
 * time should be ignored - this is, not discovered).
 */
struct RTI_RecordingServiceStorageStreamInfoReader *
FileStorageReader_create_stream_info_reader(
        void *storage_reader_data,
        const struct RTI_RoutingServiceProperties *properties)
{
    struct FileStorageReader *storage_reader =
            (struct FileStorageReader *) storage_reader_data;
    struct FileStorageStreamInfoReader *stream_reader = NULL;

    RTI_UNUSED_PARAMETER(properties);

    stream_reader = (struct FileStorageStreamInfoReader *)
            malloc(sizeof(struct FileStorageStreamInfoReader));
    if (stream_reader == NULL) {
        printf("Failed to allocate FileStorageStreamInfoReader instance\n");
        return NULL;
    }
    if (!FileStorageStreamInfoReader_initialize(
            stream_reader,
            storage_reader->file_name)) {
        printf("Failed to initialize FileStorageStreamInfoReader instance\n");
        FileStorageReader_delete_stream_info_reader(
                storage_reader_data,
                &stream_reader->base_discovery_stream_reader);
        return NULL;
    }
    return &stream_reader->base_discovery_stream_reader;
}

/**
 * Free the resources allocated by this plugin instance.
 */
void FileStorageReader_delete_instance(
        struct RTI_RecordingServiceStorageReader *storage_reader)
{
    if (storage_reader == NULL) {
        return;
    }
    free(storage_reader->storage_reader_data);
}

/**
 * This function creates an instance of the StorageReader plugin to be used by
 * Replay or Converter to read the data, as was stored by the storage part of
 * this example (see FileStorageWriter.c).
 * The plugin provides methods to create two types of reader objects, called
 * StreamReaders: the discovery stream readers and the data stream readers. This
 * function is in charge of setting up the plugin functions to create and delete
 * each type of stream reader (FileStorageDiscoveryStreamReader type for
 * the discovery stream reader, FileStorageStreamReader for the data
 * stream reader).
 * This plugin needs the file name to be passed to Replay or Converter in the
 * <property> XML tag of the storage configuration. The name of the property
 * is defined in the FILENAME_PROPERTY_NAME constant above.
 */
struct RTI_RecordingServiceStorageReader * FileStorageReader_create(
        const struct RTI_RoutingServiceProperties *properties)
{
    struct FileStorageReader *storage_reader = NULL;
    const char *file_name = NULL;

    storage_reader = malloc(sizeof(struct FileStorageReader));
    if (storage_reader == NULL) {
        printf("Failed to allocate FileStorageReader instance\n");
        return NULL;
    }
    /* Look up the file name property in the properties. These are the
     * properties defined in the XML configuration */
    file_name = RTI_RoutingServiceProperties_lookup_property(
            properties,
            FILENAME_PROPERTY_NAME);
    if (file_name == NULL) {
        printf("Failed to find property with name=%s\n",
                FILENAME_PROPERTY_NAME);
        free(storage_reader);
        return NULL;
    }
    if (strlen(file_name) >= FileStorageReader_FILE_NAME_MAX) {
        printf("File name too long (%s)\n", file_name);
        free(storage_reader);
        return NULL;
    }
    strcpy(storage_reader->file_name, file_name);

    RTI_RecordingServiceStorageReader_initialize(
            &storage_reader->as_storage_reader);

    storage_reader->as_storage_reader.create_stream_info_reader =
            FileStorageReader_create_stream_info_reader;
    storage_reader->as_storage_reader.delete_stream_info_reader =
            FileStorageReader_delete_stream_info_reader;
    storage_reader->as_storage_reader.create_stream_reader =
            FileStorageReader_create_stream_reader;
    storage_reader->as_storage_reader.delete_stream_reader =
            FileStorageReader_delete_stream_reader;
    storage_reader->as_storage_reader.delete_instance =
            FileStorageReader_delete_instance;
    storage_reader->as_storage_reader.storage_reader_data = storage_reader;

    return &storage_reader->as_storage_reader;
}
