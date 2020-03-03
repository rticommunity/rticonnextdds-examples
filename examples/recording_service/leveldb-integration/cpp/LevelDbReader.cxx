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

#include "dds/dds.hpp"

#include <rti/recording/storage/StorageDefs.hpp>

#include "LevelDb_RecorderTypes.hpp"
#include "LevelDbReader.hpp"

#define WORKING_DIR_PROPERTY "rti.recording.examples.leveldb.working_dir"

using namespace dds::core::xtypes;

namespace rti { namespace recording { namespace examples {

/*
 * Convenience macro to define the C-style function that will be called by RTI
 * Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_READER_CREATE_DEF(LevelDbReader);

LevelDbReader::LevelDbReader(const rti::routing::PropertySet& properties) :
    StorageReader(properties)
{
    /* Get working directory from properties */
    working_dir_ = rti::recording::get_from_properties<std::string>(
            properties,
            WORKING_DIR_PROPERTY);
}

LevelDbReader::~LevelDbReader()
{
}

rti::recording::storage::StorageStreamInfoReader *
LevelDbReader::create_stream_info_reader(
        const rti::routing::PropertySet& properties)
{
    /* Get time range information from the properties */
    int64_t start_timestamp = rti::recording::get_from_properties<int64_t>(
            properties,
            rti::recording::start_timestamp_property_name());
    int64_t end_timestamp = rti::recording::get_from_properties<int64_t>(
            properties,
            rti::recording::end_timestamp_property_name());

    return new LevelDbStreamInfoReader(
            working_dir_,
            start_timestamp,
            end_timestamp);
}

void LevelDbReader::delete_stream_info_reader(
            rti::recording::storage::StorageStreamInfoReader *stream_info_reader)
{
    delete stream_info_reader;
}

rti::recording::storage::StorageStreamReader *
LevelDbReader::create_stream_reader(
        const rti::routing::StreamInfo& stream_info,
        const rti::routing::PropertySet& properties)
{
    /*
     * Make sure that there is a valid type representation the stream reader can
     * work with in the StreamInfo object
     */
    if (stream_info.type_info().type_representation_kind()
            != rti::routing::TypeRepresentationKind::DYNAMIC_TYPE) {
        throw std::runtime_error(
                "Invalid type representation kind in StreamInfo object");
    }
    if (stream_info.type_info().type_representation() == nullptr) {
        throw std::runtime_error(
                "Null pointer invalid for type representation");
    }
    /* Get time range and domain ID information from the properties */
    uint32_t domain_id = rti::recording::get_from_properties<uint32_t>(
            properties,
            rti::recording::domain_id_property_name());
    int64_t start_timestamp = rti::recording::get_from_properties<int64_t>(
            properties,
            rti::recording::start_timestamp_property_name());
    int64_t end_timestamp = rti::recording::get_from_properties<int64_t>(
            properties,
            rti::recording::end_timestamp_property_name());
    return new LevelDbStreamReader(
            working_dir_,
            stream_info,
            domain_id,
            start_timestamp,
            end_timestamp);
}

void LevelDbReader::delete_stream_reader(
        rti::recording::storage::StorageStreamReader *stream_reader)
{
    delete stream_reader;
}


using namespace dds::core::xtypes;

/*
 * Create a data stream reader. For each discovered stream that matches the set
 * of interest defined in the configuration, Replay or Converter will ask us to
 * create a reader for that stream (a stream reader).
 * The start and stop timestamp parameters define the time range for which the
 * application is asking for data.
 *
 * @pre stream_info.type_info().type_representation_kind() == DYNAMIC_TYPE
 * @pre stream_info.type_info().type_representation() != null
 */
LevelDbStreamReader::LevelDbStreamReader(
        const std::string& working_dir,
        const rti::routing::StreamInfo& stream_info,
        uint32_t domain_id,
        int64_t start_timestamp,
        int64_t end_timestamp) :
    stream_info_(stream_info),
    type_(rti::core::native_conversions::cast_from_native<DynamicType>(
             *((DDS_TypeCode *) stream_info.type_info().type_representation()))),
    start_timestamp_(start_timestamp),
    end_timestamp_(end_timestamp),
    current_timestamp_(0),
    finished_(false)
{
    /*
     * Every stream and domain ID pair are stored in their own file. The name of
     * the file is built by prepending the working directory, then adding a
     * directory separator. The name of the file is the concatenation of the
     * name of the stream, a '@' character and then the domain ID. The extension
     * of the file will be '.dat'
     */
    std::stringstream db_filename;
    db_filename << working_dir << "/"
            << stream_info.stream_name() << "@" << domain_id << ".dat";
    db_filename_ = db_filename.str();

    /* Attempt to create the LevelDB database with the derived filename */
    leveldb::DB *db = nullptr;
    leveldb::Options db_options;
    /*
     * We use a custom key comparator, as the default lexicographic comparator
     * provided by default with LevelDB won't work properly with our desired
     * order: monotonic and ascending based on reception timestamp
     */
    db_options.comparator = &key_comparator_;
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
    const StructType& key_type = rti::topic::dynamic_type<UserDataKey>::get();
    key_buffer_.resize(key_type.cdr_serialized_sample_max_size());

    /*
     * Setup the iterator. We need to advance to the next valid sample that fits
     * within the specified time range.
     */
    db_iterator_ = std::unique_ptr<leveldb::Iterator>(
            data_db_->NewIterator(leveldb::ReadOptions()));
    for (db_iterator_->SeekToFirst(); db_iterator_->Valid(); ) {
        UserDataKey current_key = slice_to_user_type<UserDataKey>(
                db_iterator_->key(),
                key_buffer_);
        current_timestamp_ = current_key.reception_timestamp();
        if (current_timestamp_ >= start_timestamp_
                && current_timestamp_ <= end_timestamp_) {
            break;
        } else {
            db_iterator_->Next();
        }
    }
    /* The DB may be empty (no sample fulfills the conditions) */
    if (!db_iterator_->Valid()) {
        finished_ = true;
    }
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

LevelDbStreamReader::~LevelDbStreamReader()
{
}

void LevelDbStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
        std::vector<dds::sub::SampleInfo *>& info_seq,
        const rti::recording::storage::SelectorState& selector)
{
    if (finished_) {
        return;
    }
    UserDataKey current_key;
    const int64_t timestamp_limit =
            to_nanosec_timestamp(selector.time_range_end());
    const int32_t max_samples = selector.max_samples();
    int32_t num_samples_read = 0;
    bool next_sample_valid = db_iterator_->Valid();
    while (next_sample_valid
            && current_timestamp_ <= timestamp_limit
            && (max_samples < 0 ? true : (num_samples_read < max_samples))) {
        /*
         * Get the current value from the LevelDB file. The value is of type
         * UserDataValue, tailored to provide the validity of the sample or
         * not. We won't attempt to load the dynamic data sample from the CDR
         * buffer if the valid_data flag is not set
         */
        UserDataValue current_value = slice_to_user_type<UserDataValue>(
                db_iterator_->value(),
                value_buffer_);
        std::shared_ptr<DynamicData> sample(
                std::make_shared<DynamicData>(type_));
        if (current_value.valid_data()) {
            rti::core::xtypes::from_cdr_buffer(
                    *(sample.get()),
                    current_value.data_blob());
        }
        loaned_samples_.resize(num_samples_read + 1);
        loaned_samples_[num_samples_read] = sample;
        /*
         * The read operation needs to return a collection of sample info
         * objects too. Replay and Converter are fine with just returning the
         * valid_data flag and the reception timestamp
         */
        std::shared_ptr<dds::sub::SampleInfo> sample_info(
                std::make_shared<dds::sub::SampleInfo>());
        DDS_SampleInfo native_sample_info = DDS_SAMPLEINFO_DEFAULT;
        dds::core::Time reception_time = timestamp_to_time(current_timestamp_);
        native_sample_info.reception_timestamp.sec =
                (DDS_Long) reception_time.sec();
        native_sample_info.reception_timestamp.nanosec =
                (DDS_UnsignedLong) reception_time.nanosec();
        native_sample_info.valid_data = current_value.valid_data()
                ? DDS_BOOLEAN_TRUE
                : DDS_BOOLEAN_FALSE;
        (*sample_info)->native(native_sample_info);
        loaned_infos_.resize(num_samples_read + 1);
        loaned_infos_[num_samples_read] = sample_info;
        num_samples_read++;
        /* Advance to next DB position */
        db_iterator_->Next();
        if (db_iterator_->Valid()) {
            current_key = slice_to_user_type<UserDataKey>(
                    db_iterator_->key(),
                    key_buffer_);
            current_timestamp_ = current_key.reception_timestamp();
            /*
             * Check if the sample is out of the stream's time range; if that is
             * the case, we should set the stream as finished
             */
            if (current_timestamp_ > end_timestamp_) {
                next_sample_valid = false;
            } else {
                next_sample_valid = true;
            }
        } else {
            next_sample_valid = false;
        }
    }
    /*
     * Fill out the collections passed as parameters by the service. We will use
     * the internal vectors of samples and sample infos to get pointers
     * to be stored in these collections
     */
    sample_seq.resize(num_samples_read);
    info_seq.resize(num_samples_read);
    for (int32_t i = 0; i < num_samples_read; i++) {
        sample_seq[i] = loaned_samples_[i].get();
        info_seq[i] = loaned_infos_[i].get();
    }
}

void LevelDbStreamReader::return_loan(
        std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
        std::vector<dds::sub::SampleInfo *>& info_seq)
{
    sample_seq.clear();
    info_seq.clear();
    loaned_samples_.clear();
    loaned_infos_.clear();
    if (!db_iterator_->Valid()) {
        finished_ = true;
    } else {
        if (current_timestamp_ > end_timestamp_) {
            finished_ = true;
        }
    }
}

bool LevelDbStreamReader::finished()
{
    return finished_;
}

void LevelDbStreamReader::reset()
{
    // TODO
}

LevelDbStreamInfoReader::LevelDbStreamInfoReader(
        const std::string& working_dir,
        int64_t start_timestamp,
        int64_t end_timestamp) :
    service_start_time_(0),
    service_end_time_(0),
    start_timestamp_(start_timestamp),
    end_timestamp_(end_timestamp),
    finished_(false)
{
    std::stringstream pub_filename;
    pub_filename << working_dir << "/DCPSPublication.dat";
    discovery_filename_ = pub_filename.str();

    /* Attempt to create the LevelDB database with the discovery filename */
    leveldb::DB *db = nullptr;
    leveldb::Options db_options;
    db_options.comparator = &key_comparator_;
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
    const StructType& key_type = rti::topic::dynamic_type<UserDataKey>::get();
    key_buffer_.resize(key_type.cdr_serialized_sample_max_size());

    db_iterator_.reset(discovery_db_->NewIterator(leveldb::ReadOptions()));
    for (db_iterator_->SeekToFirst(); db_iterator_->Valid(); ) {
        UserDataKey current_key = slice_to_user_type<UserDataKey>(
                db_iterator_->key(),
                key_buffer_);
        current_timestamp_ = current_key.reception_timestamp();
        if (current_timestamp_ >= start_timestamp_
                && current_timestamp_ <= end_timestamp_) {
            break;
        } else {
            db_iterator_->Next();
        }
    }
    /* The DB may be empty (no sample fulfills the conditions) */
    if (!db_iterator_->Valid()) {
        finished_ = true;
    }

    std::stringstream metadata_filename;
    metadata_filename << working_dir << "/metadata.dat";
    /* Attempt to create the LevelDB database with the derived filename */
    db_status = leveldb::DB::Open(
            leveldb::Options(),
            metadata_filename.str(),
            &db);
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

    std::stringstream timestamp_stream;
    std::string timestamp_str;

    db_status = metadata_db_->Get(
            leveldb::ReadOptions(),
            start_time_key_name(),
            &timestamp_str);
    if (!db_status.ok()) {
        std::stringstream log_stream;
        log_stream << "Failed to read start time to metadata DB" << std::endl;
        log_stream << "    LevelDB error = " << db_status.ToString()
                << std::endl;
        throw std::runtime_error(log_stream.str());
    }
    timestamp_stream.str(timestamp_str);
    timestamp_stream >> service_start_time_;

    db_status = metadata_db_->Get(
            leveldb::ReadOptions(),
            end_time_key_name(),
            &timestamp_str);
    if (!db_status.ok()) {
        std::stringstream log_stream;
        log_stream << "Failed to read start time to metadata DB" << std::endl;
        log_stream << "    LevelDB error = " << db_status.ToString()
                << std::endl;
        throw std::runtime_error(log_stream.str());
    }
    timestamp_stream.str(timestamp_str);
    timestamp_stream >> service_end_time_;
}

LevelDbStreamInfoReader::~LevelDbStreamInfoReader()
{
}

void LevelDbStreamInfoReader::read(
        std::vector<rti::routing::StreamInfo *>& sample_seq,
        const rti::recording::storage::SelectorState& selector)
{
    if (finished_) {
        return;
    }
    /*
     * We need a type object factory to deserialize the serialized type-object.
     * We wrap it inside a smart pointer so we don't have to worry about
     * cleanup.
     */
    std::shared_ptr<DDS_TypeObjectFactory> type_factory(
            DDS_TypeObjectFactory_new(),
            DDS_TypeObjectFactory_delete);
    UserDataKey current_key;
    const int64_t timestamp_limit =
            to_nanosec_timestamp(selector.time_range_end());
    const int32_t max_samples = selector.max_samples();
    int32_t num_samples_read = 0;
    bool next_sample_valid = db_iterator_->Valid();
    while (next_sample_valid
            && current_timestamp_ <= timestamp_limit
            && (max_samples < 0 ? true : (num_samples_read < max_samples))) {
        ReducedDCPSPublication reduced_sample =
                slice_to_user_type<ReducedDCPSPublication>(
                        db_iterator_->value(),
                        value_buffer_);
        std::shared_ptr<rti::routing::StreamInfo> stream_info =
                std::make_shared<rti::routing::StreamInfo>("", "");
        if (reduced_sample.valid_data()) {
            stream_info->stream_name(reduced_sample.topic_name());
            stream_info->type_info().type_name(reduced_sample.type_name());
            stream_info->type_info().type_representation_kind(
                    rti::routing::TypeRepresentationKind::DYNAMIC_TYPE);

            DDS_TypeObject *type_obj =
                    DDS_TypeObjectFactory_create_typeobject_from_serialize_buffer(
                            type_factory.get(),
                            (const char *) reduced_sample.type().data(),
                            reduced_sample.type().size());
            if (type_obj == nullptr) {
                std::stringstream log_stream;
                log_stream << "Failed to create type-object from serialized "
                        "buffer in DB (will be skipped)" << std::endl;
                log_stream << "    Stream name = "
                        << reduced_sample.topic_name() << std::endl;
                std::cerr << log_stream.str();
                continue;
            }
            std::shared_ptr<DDS_TypeObject> type_ptr(
                    type_obj,
                    TypeObjectDeleter(type_factory.get()));
            DDS_TypeCode *type_code =
                    DDS_TypeObject_convert_to_typecode(type_obj);
            if (type_code == nullptr) {
                std::stringstream log_stream;
                log_stream << "Failed to create type-code from type-object "
                        "(will be skipped)" << std::endl;
                log_stream << "    Stream name = "
                        << reduced_sample.topic_name() << std::endl;
                std::cerr << log_stream.str();
                continue;
            }
            stream_info->type_info().type_representation(type_code);
            std::cout << stream_info->stream_name() << std::endl;
        }
        loaned_stream_infos_.resize(num_samples_read + 1);
        loaned_stream_infos_[num_samples_read] = stream_info;
        num_samples_read++;
        /* Advance to next DB position */
        db_iterator_->Next();
        if (db_iterator_->Valid()) {
            current_key = slice_to_user_type<UserDataKey>(
                    db_iterator_->key(),
                    key_buffer_);
            current_timestamp_ = current_key.reception_timestamp();
            if (current_timestamp_ > end_timestamp_) {
                next_sample_valid = false;
            } else {
                next_sample_valid = true;
            }
        } else {
            next_sample_valid = false;
        }
    }
    sample_seq.resize(num_samples_read);
    for (int32_t i = 0; i < num_samples_read; i++) {
        sample_seq[i] = loaned_stream_infos_[i].get();
    }
}

void LevelDbStreamInfoReader::return_loan(
        std::vector<rti::routing::StreamInfo *>& sample_seq)
{
    /*
     * We converted the type-object into a type-code to pass it to the Recording
     * Service engine, in the read() operation. This means that we have to
     * return any memory that this operation may have allocated.
     */
    for (size_t i = 0; i < sample_seq.size(); i++) {
        if (sample_seq[i]->type_info().type_representation() != nullptr) {
            DDS_TypeCode *type_code = (DDS_TypeCode *)
                    sample_seq[i]->type_info().type_representation();
            RTICdrTypeCode_destroyTypeCode((RTICdrTypeCode *) type_code);
        }
    }
    sample_seq.clear();
    loaned_stream_infos_.clear();
    if (db_iterator_->Valid()) {
        if (current_timestamp_ > end_timestamp_) {
            finished_ = true;
        }
    } else {
        finished_ = true;
    }
}

/*
 * Replay and Converter need to know the initial and final timestamps of the
 * recording being read.
 */
int64_t LevelDbStreamInfoReader::service_start_time()
{
    return service_start_time_;
}

/*
 * Replay and Converter need to know the initial and final timestamps of the
 * recording being read.
 */
int64_t LevelDbStreamInfoReader::service_stop_time()
{
    return service_end_time_;
}

bool LevelDbStreamInfoReader::finished()
{
    return finished_;
}

void LevelDbStreamInfoReader::reset()
{
    // TODO
}

} } } // namespace rti::recording::examples

