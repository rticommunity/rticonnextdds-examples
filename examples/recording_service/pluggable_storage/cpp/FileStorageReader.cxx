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

#include "FileStorageReader.hpp"

#include "dds/dds.hpp"

#define NANOSECS_PER_SEC 1000000000


namespace cpp_example {

/*
 * Convenience macro to define the C-style function that will be called by RTI
 * Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_READER_CREATE_DEF(FileStorageReader);

FileStorageReader::FileStorageReader(
        const rti::routing::PropertySet& properties)
    : StorageReader(properties)
{
    rti::routing::PropertySet::const_iterator found =
		properties.find("example.cpp_pluggable_storage.filename");
    if (found == properties.end()) {
        throw std::runtime_error("Failed to get file name from properties");
    }
    file_name_ = std::string(found->second);

    info_file_.open((file_name_ + ".info").c_str(),
            std::ios::in | std::ios::binary);
    if (!info_file_.good()) {
        throw std::runtime_error("Failed to open metadata file");
    }
    data_file_.open(file_name_.c_str(), std::ios::in | std::ios::binary);
    if (!data_file_.good()) {
        throw std::runtime_error("Failed to open data file");
    }
}

/*
 * In the xml configuration, under the property tag for the storage plugin, a
 * collection of name/value pairs can be passed. In this case, this example
 * chooses to define a property to name the filename to use.
 */
FileStorageReader::~FileStorageReader()
{
}

rti::recording::storage::StorageStreamInfoReader *
FileStorageReader::create_stream_info_reader(
        const rti::routing::PropertySet&)
{
    return new FileStorageStreamInfoReader(&info_file_);
}

void FileStorageReader::delete_stream_info_reader(
            rti::recording::storage::StorageStreamInfoReader *stream_info_reader)
{
    delete stream_info_reader;
}

rti::recording::storage::StorageStreamReader * FileStorageReader::create_stream_reader(
        const rti::routing::StreamInfo&,
        const rti::routing::PropertySet&)
{
    return new FileStorageStreamReader(&data_file_);
}

void FileStorageReader::delete_stream_reader(
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
 * application is asking for data. For simplicity, this example doesn't do
 * anything with these parameters. But the correct way to implement this API
 * would be to not read any data recorded before the given start time or after
 * the given end time.
 */
FileStorageStreamReader::FileStorageStreamReader(std::ifstream *data_file) :
        data_file_(data_file),
        type_("HelloMsg")
{
    type_.add_member(Member("id", primitive_type<int32_t>()).key(true));
    type_.add_member(Member("msg", StringType(256)));
    /* read-ahead, EOF is not an error */
    if (!read_sample()) {
        std::cout << "info: no first sample, storage file seems to be empty"
                << std::endl;
    }
}

FileStorageStreamReader::~FileStorageStreamReader()
{
}

bool FileStorageStreamReader::read_sample()
{
    // read sample into cache
    std::string prefix;
    uint64_t sample_nr = 0;
    *data_file_ >> prefix;

    // we won't accept partial data, but it's no error to find the end of data
    if (data_file_->eof()) {
        return false;
    }

    if (data_file_->fail()) {
        throw std::runtime_error(
                "Failed to read prefix for sample number from file");
    }
    if (prefix != std::string("Sample")) {
        throw std::runtime_error(
                "Failed to match prefix for sample number from file");
    }
    *data_file_ >> prefix;
    if (data_file_->fail()) {
        throw std::runtime_error(
                "Failed to read prefix for sample number from file");
    }
    if (prefix != std::string("number:")) {
        throw std::runtime_error(
                "Failed to match prefix for sample number from file");
    }
    *data_file_ >> sample_nr;
    if (data_file_->fail()) {
        throw std::runtime_error("Failed to read sample number from file");
    }
    *data_file_ >> prefix;
    if (data_file_->fail()) {
        throw std::runtime_error(
                "Failed to read prefix for reception timestamp from file");
    }
    if (prefix != std::string("Reception")) {
        throw std::runtime_error(
                "Failed to match prefix for reception timestamp from file");
    }
    *data_file_ >> prefix;
    if (data_file_->fail()) {
        throw std::runtime_error(
                "Failed to read prefix for reception timestamp from file");
    }
    if (prefix != std::string("timestamp:")) {
        throw std::runtime_error(
                "Failed to match prefix for reception timestamp from file");
    }
    *data_file_ >> current_timestamp_;
    if (data_file_->fail()) {
        throw std::runtime_error("Failed to read current timestamp from file");
    }
    *data_file_ >> prefix;
    if (data_file_->fail()) {
        throw std::runtime_error(
                "Failed to read prefix for valid data from file");
    }
    if (prefix != std::string("Valid")) {
        throw std::runtime_error(
                "Failed to match prefix for valid data from file");
    }
    *data_file_ >> prefix;
    if (data_file_->fail()) {
        throw std::runtime_error(
                "Failed to read prefix for valid data from file");
    }
    if (prefix != std::string("data:")) {
        throw std::runtime_error(
                "Failed to match prefix for valid data from file");
    }
    *data_file_ >> current_valid_data_;
    if (data_file_->fail()) {
        throw std::runtime_error(
                "Failed to read current valid data flag from file");
    }
    if (current_valid_data_) {
        *data_file_ >> prefix;
        if (data_file_->fail()) {
            throw std::runtime_error(
                    "Failed to read prefix for Data.id from file");
        }
        if (prefix != std::string("Data.id:")) {
            throw std::runtime_error(
                    "Failed to match prefix for Data.id from file");
        }
        *data_file_ >> current_data_id_;
        if (data_file_->fail()) {
            throw std::runtime_error(
                    "Failed to read current data.id field from file");
        }
        *data_file_ >> prefix;
        if (data_file_->fail()) {
            throw std::runtime_error(
                    "Failed to read prefix for Data.msg from file");
        }
        if (prefix != std::string("Data.msg:")) {
            throw std::runtime_error(
                    "Failed to match prefix for Data.msg from file");
        }
        std::getline(*data_file_, current_data_msg_);
        if (data_file_->fail()) {
            throw std::runtime_error(
                    "Failed to read current data.msg field from file");
        }
        std::getline(*data_file_, prefix);
        if (prefix != std::string("")) {
            throw std::runtime_error(
                    "Failed to terminator for sample from file");
        }
    }
    return true;
}

/* in this call, the timestamp argument provides the end-timestamp that the
 * Replay Service or Converter is querying. The plugin should provide only
 * stream information that has not already been taken, and has a timestamp less
 * than or equal to the timestamp_limit.  */
void FileStorageStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
        std::vector<dds::sub::SampleInfo *>& info_seq,
        const rti::recording::storage::SelectorState& selector)
{
    int64_t timestamp_limit = selector.timestamp_range_end();
    /* Add the currently read sample and sample info values to the taken data
     * and info collections (sequences), as long as their timestamp does not
     * exceed the timestamp_limit provided */
    if (current_timestamp_ > timestamp_limit) {
        return;
    }
    if (finished()) {
        return;
    }
    while (current_timestamp_ <= timestamp_limit) {
        using namespace dds::core::xtypes;

        DynamicData *read_data = new DynamicData(type_);
        read_data->value("id", current_data_id_);
        read_data->value("msg", current_data_msg_);
        sample_seq.push_back(read_data);

        DDS_SampleInfo read_sampleInfo = DDS_SAMPLEINFO_DEFAULT;
        read_sampleInfo.valid_data = current_valid_data_ ?
                DDS_BOOLEAN_TRUE : DDS_BOOLEAN_FALSE;
        read_sampleInfo.reception_timestamp.sec =
                (DDS_Long) (current_timestamp_ / (int64_t) NANOSECS_PER_SEC);
        read_sampleInfo.reception_timestamp.nanosec =
                current_timestamp_ % (int64_t) NANOSECS_PER_SEC;
        dds::sub::SampleInfo *cpp_sample_info = new dds::sub::SampleInfo;
        (*cpp_sample_info)->native(read_sampleInfo);
        info_seq.push_back(cpp_sample_info);

        /*
         * When the next sample cached fulfills the condition to be taken (its
         * timestamp is within the provided limit) this method adds it to the
         * sequence used to store the current taken samples.
         */

        /* read ahead next sample, until EOF */
        if (!read_sample()) {
            break;
        }
    }
}

void FileStorageStreamReader::return_loan(
        std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
        std::vector<dds::sub::SampleInfo *>& info_seq)
{
    for (size_t i = 0; i < sample_seq.size(); i++) {
        delete sample_seq[i];
        delete info_seq[i];
    }
    sample_seq.clear();
    info_seq.clear();
}

bool FileStorageStreamReader::finished()
{
    if (data_file_->eof()) {
        return true;
    } else {
        return false;
    }
}

void FileStorageStreamReader::reset()
{
    data_file_->clear();
    data_file_->seekg(0);
    /* read-ahead, EOF is not an error */
    if (!read_sample()) {
        std::cout << "info: no first sample, storage file seems to be empty"
                << std::endl;
    }
}

FileStorageStreamInfoReader::FileStorageStreamInfoReader(
        std::ifstream *info_file) :
    info_file_(info_file),
    stream_info_taken_(false),
    example_stream_info_("Example_Cpp_Storage", "HelloMsg"),
    example_type_("HelloMsg")
{
    using namespace dds::core::xtypes;

    example_stream_info_.type_info().type_representation_kind(
            rti::routing::TypeRepresentationKind::DYNAMIC_TYPE);
    // Manually add members of the type
    example_type_.add_member(
            dds::core::xtypes::Member("id", primitive_type<int32_t>())
                    .key(true));
    example_type_.add_member(Member("msg", StringType(256)));
    example_stream_info_.type_info().type_representation(
            &example_type_.native());
}

FileStorageStreamInfoReader::~FileStorageStreamInfoReader()
{
}

/*
 * Replay (or Converter) will periodically ask for any newly discovered data
 * streams.
 * This function receives a time limit parameter. It should return any
 * discovery  event not having been taken yet and within the given time limit
 * (associated time of the event should be less or equal to the time limit).
 * In our example we only store one topic and type, described in the provided
 * IDL file (HelloMsg.idl). We simulate we discover that stream in the very
 * first call to this function. Every other call to this function will return
 * an empty count of taken elements.
 */
void FileStorageStreamInfoReader::read(
        std::vector<rti::routing::StreamInfo *>& sample_seq,
        const rti::recording::storage::SelectorState&)
{
    /*
     * In this call, we would walk the stored data and discover what streams
     * are there with timestamps that do not exceed the given timestamp_limit.
     *
     * This example does not read the type code for the saved stream from the
     * file; instead, it defines the type code programmatically here. In a real
     * implementation, it could be read from the database if it was written
     * when store() got called.
     *
     * For this example, we will just assume there is a single stream (because
     * we know this from the writer side of the example plugin), so we populate
     * here a single sample and associated sample_info into the vectors passed
     * by reference directly instead of reading them from a file, to signal the
     * creation of the stream we want to replay, but after the first call,
     * we'll set a flag so we return early and dont "discover" any more streams
     * after the first call.
     */
    if (stream_info_taken_) {
        return;
    }

    // here we populate a stream info corresponding to the recorded topic
    // we will need to free this in this discovery stream reader's return_loan
    sample_seq.push_back(&example_stream_info_);

    // in this example, we simulate a single stream so from this point on dont
    // yield results in this discovery reader.
    stream_info_taken_ = true;
}

void FileStorageStreamInfoReader::return_loan(
            std::vector<rti::routing::StreamInfo *>& sample_seq)
{
    sample_seq.clear();
}

/*
 * Replay and Converter need to know the initial and final timestamps of the
 * recording being read.
 * In the case of our example, this information is stored in the associated
 * '.info' file.
 */
int64_t FileStorageStreamInfoReader::service_start_time()
{
    std::string line;
    int64_t timestamp = 0;

    std::getline(*info_file_, line, ':');
    std::getline(*info_file_, line);
    std::stringstream stream(line);
    stream >> timestamp;

    return timestamp;
}

/*
 * Replay and Converter need to know the initial and final timestamps of the
 * recording being read.
 * In the case of our example, this information is stored in the associated
 * '.info' file.
 */
int64_t FileStorageStreamInfoReader::service_stop_time()
{
    std::string line;
    int64_t timestamp = 0;

    std::getline(*info_file_, line, ':');
    std::getline(*info_file_, line);
    std::stringstream stream(line);
    stream >> timestamp;
    return timestamp;
}

bool FileStorageStreamInfoReader::finished()
{
    return stream_info_taken_;
}

void FileStorageStreamInfoReader::reset()
{
    stream_info_taken_ = false;
}

} // namespace cpp_example
