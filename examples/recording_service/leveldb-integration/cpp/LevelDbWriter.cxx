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

#include <algorithm>
#include <memory>

#include <dds/core/types.hpp>
#include <rti/core/constants.hpp>

#include <leveldb/env.h>
#include <leveldb/write_batch.h>

#include <rti/recording/storage/StorageDefs.hpp>

#include "LevelDbWriter.hpp"
#include "LevelDb_RecorderTypes.hpp"


namespace rti { namespace recording { namespace examples {

/*
 * Convenience macro to define the C-style function that will be called by RTI
 * Recording Service to create the main class.
 */
RTI_RECORDING_STORAGE_WRITER_CREATE_DEF(LevelDbWriter);

/**
 * In the XML configuration, under the property tag for the storage plugin, a
 * collection of name/value pairs can be passed. This example expects the
 * working directory to be passed as a property here (see WORKING_DIR_PROPERTY
 * above).
 */
LevelDbWriter::LevelDbWriter(const rti::routing::PropertySet &properties)
        : StorageWriter(properties)
{
    /* Obtain current time so we can store it as the start time */
    int64_t current_time = (int64_t) time(nullptr);
    if (current_time == -1) {
        throw std::runtime_error("Failed to obtain the current time");
    }
    /* Get working directory from properties */
    working_dir_ = rti::recording::get_from_properties<std::string>(
            properties,
            working_dir_property_name());
    /*
     * The enable auto-dir property allows for Recorder to create a
     * sub-directory inside the specified directory to use as the storage
     * location.
     */
    const bool enable_auto_dir = rti::recording::get_from_properties<bool>(
            properties,
            enable_auto_dir_property_name());
    if (enable_auto_dir) {
        leveldb::Env *env = leveldb::Env::Default();
        std::stringstream current_time_stream;
        current_time_stream << "/" << current_time;
        working_dir_ += current_time_stream.str();
        leveldb::Status status = env->CreateDir(working_dir_);
        if (!status.ok()) {
            std::stringstream log_stream;
            log_stream << "Failed to create auto working directory:"
                       << std::endl;
            log_stream << "    Directory = " << working_dir_ << std::endl;
            log_stream << "    LevelDB error = " << status.ToString()
                       << std::endl;
            throw std::runtime_error(log_stream.str());
        }
    }

    /*
     * Create the metadata file that will be used to store the start and stop
     * times of the plugin. They will represent the start and stop times of the
     * service closely enough
     */
    std::stringstream metadata_filename;
    metadata_filename << working_dir_ << "/metadata.dat";
    /* DB options: create the file if not found; rest is fine to keep default */
    leveldb::Options db_options;
    db_options.create_if_missing = true;
    /*
     * For the best record/replay/convert experience, we do not allow to append
     * data to an existing database. The following setting will preempt creation
     * of the database if it already existed. You should choose a different
     * working directory each time you run the tool
     */
    db_options.error_if_exists = true;
    leveldb::DB *db = nullptr;
    leveldb::Status db_status =
            leveldb::DB::Open(db_options, metadata_filename.str(), &db);
    if (!db_status.ok() || db == nullptr) {
        std::stringstream log_stream;
        log_stream << "Failed to open metadata DB file" << std::endl;
        log_stream << "    File name = " << metadata_filename.str()
                   << std::endl;
        log_stream << "    LevelDB error = " << db_status.ToString()
                   << std::endl;
        throw std::runtime_error(log_stream.str());
    }
    metadata_db_.reset(db);

    /* Time was returned in seconds. Transform to nanoseconds */
    current_time *= (int64_t) rti::core::nanosec_per_sec;
    std::stringstream current_time_stream;
    current_time_stream << current_time;
    db_status = metadata_db_->Put(
            leveldb::WriteOptions(),
            start_time_key_name(),
            current_time_stream.str());
    if (!db_status.ok()) {
        std::stringstream log_stream;
        log_stream << "Failed to write start time to metadata DB" << std::endl;
        log_stream << "    LevelDB error = " << db_status.ToString()
                   << std::endl;
        throw std::runtime_error(log_stream.str());
    }
}

LevelDbWriter::~LevelDbWriter()
{
    /* Obtain current time so we can store it as the service's stop time */
    int64_t current_time = (int64_t) time(nullptr);
    if (current_time == -1) {
        std::cerr << "Failed to obtain the current time" << std::endl;
    }
    /* Time was returned in seconds. Transform to nanoseconds */
    current_time *= (int64_t) rti::core::nanosec_per_sec;
    std::stringstream current_time_stream;
    current_time_stream << current_time;
    leveldb::Status db_status = metadata_db_->Put(
            leveldb::WriteOptions(),
            end_time_key_name(),
            current_time_stream.str());
    if (!db_status.ok()) {
        std::stringstream log_stream;
        log_stream << "Failed to write end time to metadata DB" << std::endl;
        log_stream << "    LevelDB error = " << db_status.ToString()
                   << std::endl;
        /* Do not throw in a C++ destructor */
        std::cerr << log_stream.str();
    }
}

rti::recording::storage::StorageStreamWriter *LevelDbWriter::
        create_stream_writer(
                const rti::routing::StreamInfo &stream_info,
                const rti::routing::PropertySet &properties)
{
    /*
     * Recorder will pass the Domain ID information as a property. Get its value
     * and pass it to the stream writer constructor.
     * Note: we provide the utility below to get a value of a certain type from
     * a set of properties */
    uint32_t domain_id = rti::recording::get_from_properties<uint32_t>(
            properties,
            rti::recording::domain_id_property_name());
    return new LevelDbStreamWriter(working_dir_, stream_info, domain_id);
}

rti::recording::storage::PublicationStorageWriter *LevelDbWriter::
        create_publication_writer()
{
    return new PubDiscoveryLevelDbWriter(working_dir_);
}

void LevelDbWriter::delete_stream_writer(
        rti::recording::storage::StorageStreamWriter *writer)
{
    delete writer;
}

LevelDbStreamWriter::LevelDbStreamWriter(
        const std::string &working_dir,
        const rti::routing::StreamInfo &stream_info,
        uint32_t domain_id)
        : stream_info_(stream_info), domain_id_(domain_id)
{
    using namespace dds::core::xtypes;

    /*
     * Every stream and domain ID pair are stored in their own DB. The name of
     * the DB is built by prepending the working directory, then adding a
     * directory separator. The name of the file is the concatenation of the
     * name of the stream, a '@' character and then the domain ID. The extension
     * of the file will be '.dat'
     */
    std::stringstream db_filename;
    db_filename << working_dir << "/" << stream_info.stream_name() << "@"
                << domain_id << ".dat";
    db_filename_ = db_filename.str();
    /* DB options: create the file if not found; rest is fine to keep default */
    leveldb::Options db_options;
    db_options.create_if_missing = true;
    /*
     * For the best record/replay/convert experience, we do not allow to append
     * data to an existing database. The following setting will preempt creation
     * of the database if it already existed. You should choose a different
     * working directory each time you run the tool
     */
    db_options.error_if_exists = true;
    /* Order samples in monotonic ascending reception timestamp order */
    db_options.comparator = &key_comparator_;
    db_options.compression = leveldb::kNoCompression;
    /* Attempt to create the LevelDB database with the derived filename */
    leveldb::DB *db = nullptr;
    leveldb::Status db_status =
            leveldb::DB::Open(db_options, db_filename_, &db);
    if (!db_status.ok() || db == nullptr) {
        std::stringstream log_stream;
        log_stream << "Failed to open user-data DB file" << std::endl;
        log_stream << "    File name = " << db_filename_ << std::endl;
        log_stream << "    LevelDB error = " << db_status.ToString()
                   << std::endl;
        throw std::runtime_error(log_stream.str());
    }
    data_db_.reset(db);
    /*
     * Pre-allocate key buffer, used to serialize the UserDataKey type to be
     * used as a key
     */
    const StructType &key_type = rti::topic::dynamic_type<UserDataKey>::get();
    key_buffer_.resize(key_type.cdr_serialized_sample_max_size());
    /*
     * Pre-allocate an intelligent size for the CDR buffer to be used by the
     * store operations. Get the maximum sample size from the type information.
     * We protect the buffer size if the type is unbounded.
     */
    DDS_TypeCode *type = static_cast<DDS_TypeCode *>(
            stream_info.type_info().type_representation());
    DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
    if (DDS_TypeCode_is_unbounded(type, DDS_BOOLEAN_FALSE, &ex)
        && ex != DDS_NO_EXCEPTION_CODE) {
        value_buffer_.resize(8192);
    } else {
        DDS_UnsignedLong size = DDS_TypeCode_get_cdr_serialized_sample_max_size(
                type,
                DDS_AUTO_DATA_REPRESENTATION,
                &ex);
        if (ex != DDS_NO_EXCEPTION_CODE) {
            std::stringstream log_stream;
            log_stream << "Could not get serialized sample max size for stream"
                       << std::endl;
            log_stream << "    Stream name = " << stream_info.stream_name()
                       << std::endl;
            throw std::runtime_error(log_stream.str());
        }
        value_buffer_.resize((std::vector<char>::size_type) size);
    }
}

LevelDbStreamWriter::~LevelDbStreamWriter()
{
}

void LevelDbStreamWriter::store(
        const std::vector<dds::core::xtypes::DynamicData *> &sample_seq,
        const std::vector<dds::sub::SampleInfo *> &info_seq)
{
    using namespace dds::core::xtypes;
    using namespace rti::core::xtypes;
    using namespace dds::sub;

    leveldb::WriteBatch transaction;

    const int32_t count = sample_seq.size();
    for (int32_t i = 0; i < count; ++i) {
        /*
         * The key of a sample has to be unique, and is composed of the
         * following:
         * - the reception timestamp
         * - the original writer virtual GUID
         * - the original virtual sequence number
         */
        const SampleInfo &sample_info = *(info_seq[i]);
        int64_t reception_timestamp = sample_info->reception_timestamp().sec();
        reception_timestamp *= rti::core::nanosec_per_sec;
        reception_timestamp += sample_info->reception_timestamp().nanosec();
        dds::core::array<uint8_t, GUID_LENGTH> guid;
        rti::core::Guid original_pub_v_guid(
                sample_info.delegate().original_publication_virtual_guid());
        memcpy(guid.data(), original_pub_v_guid.native().value, GUID_LENGTH);
        rti::core::SequenceNumber seq_nr(
                sample_info.delegate()
                        .original_publication_virtual_sequence_number());
        UserDataKey key(
                SequenceNumber(seq_nr.high(), seq_nr.low()),
                reception_timestamp,
                guid);
        dds::topic::topic_type_support<UserDataKey>::to_cdr_buffer(
                key_buffer_,
                key);
        leveldb::Slice key_slice(key_buffer_.data(), key_buffer_.size());
        /*
         * The user-data sample is stored along with valid data flag. In fact,
         * if the sample is not valid we do not serialize the sample to a CDR
         * buffer at all
         */
        UserDataValue value;
        value.valid_data(sample_info.valid());
        if (sample_info.valid()) {
            const dds::core::xtypes::DynamicData &sample = *(sample_seq[i]);
            rti::core::xtypes::to_cdr_buffer(value.data_blob(), sample);
        }
        dds::topic::topic_type_support<UserDataValue>::to_cdr_buffer(
                value_buffer_,
                value);
        leveldb::Slice data_slice(value_buffer_.data(), value_buffer_.size());

        transaction.Put(key_slice, data_slice);
    }
    leveldb::Status status =
            data_db_->Write(leveldb::WriteOptions(), &transaction);
    if (!status.ok()) {
        std::stringstream log_stream;
        log_stream << "Error writing batch operations to LevelDB:" << std::endl;
        log_stream << "    Stream name = " << stream_info_.stream_name()
                   << std::endl;
        log_stream << "    File name = " << db_filename_ << std::endl;
        log_stream << "    LevelDB error = " << status.ToString() << std::endl;
        log_stream << "Samples could not be stored (" << count
                   << (count == 1 ? " sample)" : "samples)") << std::endl;
        std::cerr << log_stream.str();
    }
}

PubDiscoveryLevelDbWriter::PubDiscoveryLevelDbWriter(
        const std::string &working_dir)
{
    using namespace dds::core::xtypes;

    /*
     * The discovery information (just the DCPSPublication topic) will be
     * stored in its own LevelDB database. The name of the database will be
     * formed by appending 'DCPSPublication.dat' to the user-provided working
     * directory
     */
    std::stringstream pub_filename;
    pub_filename << working_dir << "/DCPSPublication.dat";
    discovery_filename_ = pub_filename.str();
    /* DB options: create the file if not found; rest is fine to keep default */
    leveldb::Options db_options;
    db_options.create_if_missing = true;
    /*
     * For the best record/replay/convert experience, we do not allow to append
     * data to an existing database. The following setting will preempt creation
     * of the database if it already existed. You should choose a different
     * working directory each time you run the tool
     */
    db_options.error_if_exists = true;
    /* Order samples in monotonic ascending reception timestamp order */
    db_options.comparator = &key_comparator_;
    leveldb::DB *db = nullptr;
    leveldb::Status db_status =
            leveldb::DB::Open(db_options, discovery_filename_, &db);
    if (!db_status.ok() || db == nullptr) {
        std::stringstream log_stream;
        log_stream << "Failed to open discovery (DCPSPublication) DB file"
                   << std::endl;
        log_stream << "    File name = " << discovery_filename_ << std::endl;
        log_stream << "    LevelDB error = " << db_status.ToString()
                   << std::endl;
        throw std::runtime_error(log_stream.str());
    }
    discovery_db_.reset(db);
    /*
     * Pre-allocate key buffer, used to serialize the UserDataKey type to be
     * used as a key
     */
    const StructType &key_type = rti::topic::dynamic_type<UserDataKey>::get();
    key_buffer_.resize(key_type.cdr_serialized_sample_max_size());
}

PubDiscoveryLevelDbWriter::~PubDiscoveryLevelDbWriter()
{
}

void PubDiscoveryLevelDbWriter::store(
        const std::vector<dds::topic::PublicationBuiltinTopicData *>
                &sample_seq,
        const std::vector<dds::sub::SampleInfo *> &info_seq)
{
    using namespace dds::sub;

    leveldb::WriteBatch transaction;

    const int32_t count = sample_seq.size();
    /* No samples to store, fast-exit */
    if (count == 0) {
        return;
    }
    for (int32_t i = 0; i < count; ++i) {
        const SampleInfo &sample_info = *(info_seq[i]);
        /*
         * The key of a sample has to be unique and is hence composed of the
         * following:
         * - the reception timestamp
         * - the original writer virtual GUID
         * - the original virtual sequence number
         */
        int64_t reception_timestamp = sample_info->reception_timestamp().sec();
        reception_timestamp *= rti::core::nanosec_per_sec;
        reception_timestamp += sample_info->reception_timestamp().nanosec();
        dds::core::array<uint8_t, GUID_LENGTH> guid;
        rti::core::Guid original_pub_v_guid(
                sample_info.delegate().original_publication_virtual_guid());
        memcpy(guid.data(), original_pub_v_guid.native().value, GUID_LENGTH);
        rti::core::SequenceNumber seq_nr(
                sample_info.delegate()
                        .original_publication_virtual_sequence_number());
        UserDataKey key(
                SequenceNumber(seq_nr.high(), seq_nr.low()),
                reception_timestamp,
                guid);
        dds::topic::topic_type_support<UserDataKey>::to_cdr_buffer(
                key_buffer_,
                key);
        leveldb::Slice key_slice(key_buffer_.data(), key_buffer_.size());

        /*
         * We don't store all the information in the DCPSPublication data
         * sample, for example and practical purposes (the less fields we store,
         * the more efficient we are)
         */
        dds::topic::PublicationBuiltinTopicData &sample = *(sample_seq[i]);
        ReducedDCPSPublication reduced_sample;
        reduced_sample.valid_data(sample_info.valid());
        if (sample_info.valid()) {
            reduced_sample.topic_name(sample.topic_name());
            reduced_sample.type_name(sample.type_name());
            const dds::core::optional<dds::core::xtypes::DynamicType> type =
                    sample->type();
            if (type.is_set()) {
                const dds::core::xtypes::DynamicType &dynamic_type = type.get();
                const DDS_TypeCode &native_type = dynamic_type.native();
                DDS_TypeObject *type_object =
                        DDS_TypeObject_create_from_typecode(&native_type);
                if (type_object == nullptr) {
                    std::stringstream log_msg;
                    log_msg << "Failed to create type-object from type-code. "
                               "DCPSPublication sample cannot be stored"
                            << std::endl;
                    log_msg << "    Topic name = " << sample.topic_name()
                            << std::endl;
                    log_msg << "    Type name  = " << sample.type_name()
                            << std::endl;
                    /*
                     * Do not throw here. Not being able to correctly store the
                     * sample is bad but not final (let's store the rest of the
                     * samples if possible)
                     */
                    std::cerr << log_msg.str();
                    continue;
                }
                std::shared_ptr<DDS_TypeObject> type_object_ptr(
                        type_object,
                        DDS_TypeObject_delete);
                uint32_t type_object_buffer_len =
                        DDS_TypeObject_get_serialized_size(type_object);
                std::vector<uint8_t> typeobject_buffer(type_object_buffer_len);
                if (DDS_TypeObject_serialize(
                            type_object,
                            (char *) &(typeobject_buffer[0]),
                            &type_object_buffer_len)
                    != DDS_RETCODE_OK) {
                    std::stringstream log_msg;
                    log_msg << "Failed to serialize type-object. "
                               "DCPSPublication sample cannot be stored"
                            << std::endl;
                    log_msg << "    Topic name = " << sample.topic_name()
                            << std::endl;
                    log_msg << "    Type name  = " << sample.type_name()
                            << std::endl;
                    /*
                     * Do not throw here. Not being able to correctly store the
                     * sample is bad but not final (let's store the rest of the
                     * samples if possible)
                     */
                    std::cerr << log_msg.str();
                    continue;
                }
                reduced_sample.type(typeobject_buffer);
            }
        }
        dds::topic::topic_type_support<ReducedDCPSPublication>::to_cdr_buffer(
                value_buffer_,
                reduced_sample);
        leveldb::Slice data_slice(value_buffer_.data(), value_buffer_.size());
        transaction.Put(key_slice, data_slice);
    }
    leveldb::Status status =
            discovery_db_->Write(leveldb::WriteOptions(), &transaction);
    if (!status.ok()) {
        std::stringstream log_stream;
        log_stream << "Error writing batch operations to LevelDB:" << std::endl;
        log_stream << "    File name = " << discovery_filename_ << std::endl;
        log_stream << "    LevelDB error = " << status.ToString() << std::endl;
        log_stream << "DCPSPublication samples could not be stored (" << count
                   << (count == 1 ? " sample)" : "samples)") << std::endl;
        std::cerr << log_stream.str();
    }
}

}}}  // namespace rti::recording::examples
