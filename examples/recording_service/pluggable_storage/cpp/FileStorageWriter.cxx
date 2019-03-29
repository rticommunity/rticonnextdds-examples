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

#include "FileStorageWriter.hpp"

#define FILESTORAGEWRITER_INDENT_LEVEL (4)
#define NANOSECS_PER_SEC 1000000000ll

namespace cpp_example {

/*
 * Convenience macro to define the C-style function that will be called by RTI
 * Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_WRITER_CREATE_DEF(FileStorageWriter);

/*
 * In the xml configuration, under the property tag for the storage plugin, a
 * collection of name/value pairs can be passed. In this case, this example
 * chooses to define a property to name the filename to use.
 */
FileStorageWriter::FileStorageWriter(
        const rti::routing::PropertySet& properties) :
    StorageWriter(properties)
{
    rti::routing::PropertySet::const_iterator found =
            properties.find("example.cpp_pluggable_storage.filename");
    if (found == properties.end()) {
        throw std::runtime_error("Failed to get file name from properties");
    }
    std::string data_filename_ = found->second;
    data_file_.open(data_filename_.c_str(), std::ios::out);
    if (!data_file_.good()) {
        throw std::runtime_error("Failed to open file to store data samples");
    }
    std::string pub_filename_ = data_filename_ + ".pub";
    pub_file_.open(pub_filename_.c_str(), std::ios::out);
    if (!pub_file_.good()) {
        throw std::runtime_error(
                "Failed to open file to store publication data samples");
    }
    std::string info_filename_ = data_filename_ + ".info";
    info_file_.open(info_filename_.c_str(), std::ios::out);
    if (!info_file_.good()) {
        throw std::runtime_error("Failed to open file to store metadata");
    }

    // populate the info file with start timestamp
    /* Obtain current time */
    int64_t current_time = (int64_t) time(NULL);
    if (current_time == -1) {
        throw std::runtime_error("Failed to obtain the current time");
    }
    /* Time was returned in seconds. Transform to nanoseconds */
    current_time *= NANOSECS_PER_SEC;
    info_file_ << "Start timestamp: " << current_time << std::endl;
    if (info_file_.fail()) {
        throw std::runtime_error("Failed to write start timestamp");
    }
}

FileStorageWriter::~FileStorageWriter()
{
    if (info_file_.good()) {
        /* Obtain current time */
        int64_t current_time = (int64_t) time(NULL);
        if (current_time == -1) {
            // can't throw in a destructor
            std::cerr << "Failed to obtain the current time";
        }
        /* Time was returned in seconds. Transform to nanoseconds */
        current_time *= NANOSECS_PER_SEC;
        info_file_ << "End timestamp: " << current_time << std::endl;
    }
}

rti::recording::storage::StorageStreamWriter *
FileStorageWriter::create_stream_writer(
        const rti::routing::StreamInfo& stream_info,
        const rti::routing::PropertySet&)
{
    return new FileStreamWriter(data_file_, stream_info.stream_name());
}

rti::recording::storage::PublicationStorageWriter *
FileStorageWriter::create_publication_writer()
{
    return new PubDiscoveryFileStreamWriter(pub_file_);
}

void FileStorageWriter::delete_stream_writer(
        rti::recording::storage::StorageStreamWriter *writer)
{
    delete writer;
}

FileStreamWriter::FileStreamWriter(
        std::ofstream& data_file,
        const std::string& stream_name) :
    stored_sample_count_(0),
    data_file_(data_file),
    stream_name_(stream_name)
{
}

FileStreamWriter::~FileStreamWriter()
{
}

/*
 * This function is called by Recorder whenever there are samples available for
 * one of the streams previously discovered and accepted (see the
 * FileStorageWriter_create_stream_writer() function below). Recorder provides
 * the samples and their associated information objects in Routing Service
 * format, this is, untyped format.
 * In our case we know that, except for the built-in DDS discovery topics which
 * are received in their own format - and that we're not really storing -, that
 * the format of the data we're receiving is DDS Dynamic Data. This will always
 * be the format received for types recorded from DDS.
 * The function traverses the collection of samples and stores the data in a
 * textual format.
 */
void FileStreamWriter::store(
        const std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
        const std::vector<dds::sub::SampleInfo *>& info_seq)
{
    using namespace dds::core::xtypes;
    using namespace rti::core::xtypes;
    using namespace dds::sub;

    const int32_t count = sample_seq.size();
    for (int32_t i = 0; i < count; ++i) {
        const SampleInfo& sample_info = *(info_seq[i]);
        // we first first print the sample's metadata
        int64_t timestamp =
                (int64_t) sample_info->reception_timestamp().sec()
                * NANOSECS_PER_SEC;
        timestamp += sample_info->reception_timestamp().nanosec();

        data_file_ << "Sample number: " << stored_sample_count_ << std::endl;
        data_file_ << "Reception timestamp: " << timestamp << std::endl;
        data_file_ << "Valid data: " << sample_info->valid() << std::endl;

        // now print sample data
        if (sample_info->valid()) {
            data_file_ << "    Data.id: "
                    << sample_seq[i]->value<int32_t>("id") << std::endl;
            // Get and store the sample's msg field
            data_file_ << "    Data.msg: "
                    << sample_seq[i]->value<std::string>("msg").c_str()
                    << std::endl;
        }
        stored_sample_count_++;
    }
}

PubDiscoveryFileStreamWriter::PubDiscoveryFileStreamWriter(
        std::ofstream& pub_file) :
    pub_file_(pub_file),
    stored_sample_count_(0)
{
}

PubDiscoveryFileStreamWriter::~PubDiscoveryFileStreamWriter()
{
}

void PubDiscoveryFileStreamWriter::store(
        const std::vector<dds::topic::PublicationBuiltinTopicData *>& sample_seq,
        const std::vector<dds::sub::SampleInfo *>& info_seq)
{
    using namespace dds::sub;

    const int32_t count = sample_seq.size();
    for (int32_t i = 0; i < count; ++i) {
        const SampleInfo& sample_info = *(info_seq[i]);
        // we first first print the sample's metadata
        int64_t timestamp =
                (int64_t) sample_info->reception_timestamp().sec()
                * NANOSECS_PER_SEC;
        timestamp += sample_info->reception_timestamp().nanosec();

        pub_file_ << "Sample number: " << stored_sample_count_ << std::endl;
        pub_file_ << "Reception timestamp: " << timestamp << std::endl;
        pub_file_ << "Valid data: " << sample_info->valid() << std::endl;
        // now print sample data
        if (sample_info->valid()) {
            pub_file_ << "Topic name: "
                    << sample_seq[i]->topic_name() << std::endl;
            pub_file_ << "Type name: "
                    << sample_seq[i]->type_name() << std::endl;
        }
        stored_sample_count_++;
    }
}

} // namespace cpp_example

