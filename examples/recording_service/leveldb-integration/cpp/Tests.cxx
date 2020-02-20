/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <dds_c/dds_c_string.h>

#include <rti/core/NativeValueType.hpp>
#include <dds/sub/SampleInfo.hpp>
#include <dds/core/xtypes/DynamicType.hpp>
#include <dds/core/xtypes/DynamicData.hpp>

#include <rti/recording/storage/StorageDefs.hpp>

#include "leveldb/env.h"

#include "LevelDb_RecorderTypes.hpp"
#include "LevelDbReader.hpp"
#include "LevelDbWriter.hpp"

/**
 * This test application creates a storage writer and a storage reader of the
 * LevelDB implementations.
 * The writing part will create a publication stream writer, and will store the
 * mocked discovery of a topic named "Topic1", with type name "Type1" and
 * associated with the built-in KeyedString type-code. The test will also create
 * a stream writer, associated with that topic and type information, and store
 * one sample of the type.
 * The reading part of the test will execute after that, creating a stream info
 * reader, and successfully reading the stored stream info object ("Topic1",
 * "Type1" and KeyedString type-code). After that it will create a stream reader
 * and attempt to read the stored sample, comparing the retrieved sample with
 * the one we originally stored.
 *
 * Overall, the test mimics the behaviour of a storage (Recorder) cycle and then
 * a reading (Replay, Converter) cycle.
 */
int main(int argc, char **argv)
{
    using namespace rti::core::native_conversions;
    using namespace dds::core::xtypes;

    try {
        /*
         * We'll make use of the Env utility in the LevelDB API to create the
         * test directory
         */
        leveldb::Env *env = leveldb::Env::Default();

        /* Obtain current time */
        int64_t current_time = (int64_t) time(nullptr);
        if (current_time == -1) {
            std::cerr << "Failed to obtain the current time" << std::endl;
            return EXIT_FAILURE;
        }
        /* Time was returned in seconds. Transform to millis */
        current_time *= (int64_t) rti::core::nanosec_per_millisec;
        /*
         * The test creates a new directory in the current working directory
         * based on the current time in milliseconds. Set up the working_dir
         * property that the LevelDB plugin expects to create the database
         * structure.
         */
        rti::routing::PropertySet properties;
        std::stringstream dir_name_builder;
        dir_name_builder << "test_" << current_time;
        properties["rti.recording.examples.leveldb.working_dir"] =
                dir_name_builder.str();
        env->CreateDir(dir_name_builder.str());

        /*
         * Writing tests. Create a LevelDbWriter instance, from which we'll create
         * a publication writer and a stream writer for a mock topic
         */
        std::unique_ptr<rti::recording::examples::LevelDbWriter> leveldb_writer(
                std::make_unique<rti::recording::examples::LevelDbWriter>(
                        properties));

        rti::recording::storage::PublicationStorageWriter *pub_writer =
                leveldb_writer->create_publication_writer();

        DDS_TypeCodeFactory *tc_factory = DDS_TypeCodeFactory_get_instance();
        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

        const std::string topic_name("Topic1");
        const std::string type_name("Type1");
        DDS_PublicationBuiltinTopicData native_pub_data;
        assert(DDS_PublicationBuiltinTopicData_initialize(&native_pub_data));
        /*
         * The lifecycle of the native publication data object, which needs
         * finalization to release memory, will be automated by RAII behaviour
         * encapsulated in a shared pointer
         */
        std::shared_ptr<DDS_PublicationBuiltinTopicData> pub_data_raii_holder(
                &native_pub_data,
                DDS_PublicationBuiltinTopicData_finalize);
        strncpy(
                native_pub_data.topic_name,
                topic_name.c_str(),
                MIG_RTPS_PATHNAME_LEN_MAX);
        strncpy(
                native_pub_data.type_name,
                type_name.c_str(),
                MIG_RTPS_PATHNAME_LEN_MAX);
        /*
         * We use one of the built-in topics in DDS (keyed string) for this
         * test
         */
        native_pub_data.type_code = DDS_TypeCodeFactory_clone_tc(
                tc_factory,
                (const DDS_TypeCode *) DDS_KeyedString_get_typecode(),
                &ex);
        assert(native_pub_data.topic_name != nullptr);
        assert(native_pub_data.type_name != nullptr);
        assert(native_pub_data.type_code != nullptr);
        assert(ex == DDS_NO_EXCEPTION_CODE);

        std::vector<dds::topic::PublicationBuiltinTopicData *> samples(1);
        samples[0] = reinterpret_cast<dds::topic::PublicationBuiltinTopicData *>(
                &native_pub_data);

        DDS_SampleInfo native_info = DDS_SAMPLEINFO_DEFAULT;
        native_info.reception_timestamp.sec =
                (current_time / rti::core::millisec_per_sec) + 1;
        native_info.reception_timestamp.nanosec = 0;
        native_info.valid_data = DDS_BOOLEAN_TRUE;
        dds::sub::SampleInfo sample_info;
        sample_info.delegate().native(native_info);
        std::vector<dds::sub::SampleInfo *> sample_infos(1);
        sample_infos[0] = &sample_info;
        /* Call the store operation on the publication writer object.  */
        pub_writer->store(samples, sample_infos);

        rti::routing::StreamInfo stream_info(topic_name, type_name);
        stream_info.type_info().type_representation(native_pub_data.type_code);
        stream_info.type_info().type_representation_kind(
                rti::routing::TypeRepresentationKind::DYNAMIC_TYPE);

        rti::routing::PropertySet stream_properties;
        stream_properties[rti::recording::domain_id_property_name()] = "0";
        rti::recording::examples::LevelDbStreamWriter *stream_writer =
                static_cast<rti::recording::examples::LevelDbStreamWriter *>(
                        leveldb_writer->create_stream_writer(
                                stream_info,
                                stream_properties));

        /* Store a data sample */
        DynamicType type =
                cast_from_native<DynamicType>(*(native_pub_data.type_code));
        DynamicData data_sample(type);
        const std::string sample_key = "SampleKey1";
        data_sample.value("key", sample_key);
        std::vector<DynamicData *> data_samples(1);
        data_samples[0] = &data_sample;
        stream_writer->store(data_samples, sample_infos);

        /* Delete the two stream writers created by the storage writer */
        leveldb_writer->delete_stream_writer(stream_writer);
        leveldb_writer->delete_stream_writer(pub_writer);

        /*
         * By resetting the smart pointer we delete the storage writer object,
         * releasing any database locks
         */
        leveldb_writer.reset();

        /*
         * Reading tests. Create a LevelDbReader instance, and create a stream info
         * reader and a stream reader from it, that should be capable of reading
         * from the infrastructure created from the writing side above.
         */
        std::unique_ptr<rti::recording::examples::LevelDbReader> leveldb_reader(
                std::make_unique<rti::recording::examples::LevelDbReader>(
                        properties));

        /* Add time range (end and start timestamps) to property set */
        stream_properties[rti::recording::start_timestamp_property_name()] =
                "1000000000";
        stream_properties[rti::recording::end_timestamp_property_name()] =
                "1682025910000000000";

        rti::recording::storage::StorageStreamInfoReader *stream_info_reader =
                leveldb_reader->create_stream_info_reader(stream_properties);
        /*
         * Read from the stream info reader. It should read one stream info
         * object into the vector passed in to the read call
         */
        std::vector<rti::routing::StreamInfo *> read_stream_infos;
        rti::recording::storage::SelectorState selector;
        selector.max_samples(1024);
        stream_info_reader->read(read_stream_infos, selector);
        assert(read_stream_infos.size() == 1);
        assert(read_stream_infos[0]->stream_name().compare(topic_name) == 0);
        assert(read_stream_infos[0]->type_info().type_name().compare(type_name)
                == 0);
        const char *read_typecode_name =
                DDS_TypeCode_name(
                        (const DDS_TypeCode *) read_stream_infos[0]->type_info()
                                .type_representation(),
                        &ex);
        assert(ex == DDS_NO_EXCEPTION_CODE);
        assert(type.name().compare(read_typecode_name) == 0);
        assert(!read_stream_infos[0]->disposed());
        /*
         * Replay and Converter will always call the return-loan operation, so
         * the plugin can release any resources that it needed to allocate
         * during the reading operation
         */
        stream_info_reader->return_loan(read_stream_infos);
        /*
         * Because we only stored one publication object in the DB, the stream
         * info reader must be finished at this point
         */
        assert(stream_info_reader->finished());

        /*
         * Create a user-data stream reader, to read the stored KeyedString
         * sample stored in the writing side
         */
        rti::recording::examples::LevelDbStreamReader *stream_reader =
                static_cast<rti::recording::examples::LevelDbStreamReader *>(
                        leveldb_reader->create_stream_reader(
                                stream_info,
                                stream_properties));

        std::vector<DynamicData *> read_data_samples;
        std::vector<dds::sub::SampleInfo *> read_data_infos;
        /*
         * Read the data sample, then compare the string key to what we stored
         * during the writing part
         */
        stream_reader->read(read_data_samples, read_data_infos, selector);
        assert(read_data_samples.size() == 1);
        assert(read_data_infos.size() == 1);
        std::string read_data_key = read_data_samples[0]->value<std::string>("key");
        assert(read_data_key.compare(sample_key) == 0);
        assert(read_data_infos[0]->valid());
        /*
         * Replay and Converter will always call the return-loan operation to
         * release any allocated resources needed by the read operation
         */
        stream_reader->return_loan(read_data_samples, read_data_infos);
        /*
         * We only stored one single user-data sample in the DB, so at this
         * point the stream should be finished
         */
        assert(stream_reader->finished());

        /* Delete the two stream readers created by the storage reader */
        leveldb_reader->delete_stream_info_reader(stream_info_reader);
        leveldb_reader->delete_stream_reader(stream_reader);
        /*
         * Resetting the storage reader pointer will delete the underlying
         * object, releasing resources
         */
        leveldb_reader.reset();

        env->RemoveDir(dir_name_builder.str());
    } catch (const std::exception& ex) {
        std::cerr << "Test Failed!!" << std::endl;
        std::cerr << "    Exception thrown: " << ex.what() << std::endl;
    }
    std::cout << "Test Passed!!" << std::endl;
}
