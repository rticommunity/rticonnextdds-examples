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

#include "UtilsStorageWriter.hpp"
#include "PrintFormatCsv.hpp"
#include "Logger.hpp"

#define NANOSECS_PER_SEC 1000000000ll

/*
 * --- UtilsStorageProperty ---------------------------------------------------
 */

namespace rti { namespace recorder { namespace utils {

UtilsStorageProperty::UtilsStorageProperty() 
    : output_dir_path_("."),
      merge_output_files_(false),
      output_format_kind_(OutputFormatKind::CSV_FORMAT)
{
}

void UtilsStorageProperty::output_dir_path(const std::string& path)
{
    output_dir_path_ = path;
}

void UtilsStorageProperty::output_file_basename(const std::string& output_file_basename)
{
    output_file_basename_ = output_file_basename;
}


void UtilsStorageProperty::output_format_kind(const OutputFormatKind& kind)
{
    output_format_kind_ = kind;
}

void UtilsStorageProperty::merge_output_files(bool merge)
{
    merge_output_files_ = merge;
}



std::string UtilsStorageProperty::output_dir_path() const
{
    return output_dir_path_;
}

std::string UtilsStorageProperty::output_file_basename() const
{
    return output_file_basename_;
}

OutputFormatKind UtilsStorageProperty::output_format_kind() const
{
    return output_format_kind_;
}

bool UtilsStorageProperty::merge_output_files() const
{
    return merge_output_files_;
}

/*
 * --- UtilsStorageWriter ---------------------------------------------------
 */

RTI_RECORDING_STORAGE_WRITER_CREATE_DEF(UtilsStorageWriter);


const dds::core::xtypes::DynamicType& dynamic_type(
        const rti::routing::StreamInfo& stream_info)
{
    return *(static_cast<dds::core::xtypes::DynamicType*>(
            stream_info.type_info().type_representation()));
}

const std::string& UtilsStorageWriter::PROPERTY_NAMESPACE()
{
    static std::string value = "rti.recording.utils_storage";
    return value;
}


const std::string& UtilsStorageWriter::OUTPUT_DIR_PROPERTY_NAME()
{
    static std::string value = PROPERTY_NAMESPACE()
            + ".output_dir_path";
    return value;
}

const std::string& UtilsStorageWriter::OUTPUT_FILE_BASENAME_PROPERTY_NAME()
{
    static std::string value = PROPERTY_NAMESPACE()
        + ".output_file_basename";
    return value;
}


const std::string& UtilsStorageWriter::OUTPUT_FORMAT_PROPERTY_NAME()
{
    static std::string value = PROPERTY_NAMESPACE()
            + ".output_format";
    return value;
}

const std::string& UtilsStorageWriter::OUTPUT_MERGE_PROPERTY_NAME()
{
    static std::string value = PROPERTY_NAMESPACE()
            + ".output_merge";
    return value;
}

const std::string& UtilsStorageWriter::LOGGING_VERBOSITY_PROPERTY_NAME()
{
    static std::string value = PROPERTY_NAMESPACE()
            + ".verbosity";
    return value;
}

const std::string& UtilsStorageWriter::CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME()
{
    static std::string value = PROPERTY_NAMESPACE()
            + ".csv.empty_member_value";
    return value;
}

const std::string& UtilsStorageWriter::CSV_ENUM_AS_STRING_PROPERTY_NAME()
{
    static std::string value = PROPERTY_NAMESPACE()
            + ".csv.enum_as_string";
    return value;
}


const std::string& UtilsStorageWriter::CSV_FILE_EXTENSION()
{
    static std::string value = ".csv";
    return value;
}

const std::string& UtilsStorageWriter::OUTPUT_FILE_BASENAME_DEFAULT()
{
    static std::string value = "csv_converted";
    return value;
}

const UtilsStorageProperty& UtilsStorageWriter::PROPERTY_DEFAULT()
{
    static UtilsStorageProperty property;
    static bool init = false;

    if (!init) {
        property.merge_output_files(true);
        property.output_dir_path(".");
        property.output_file_basename(OUTPUT_FILE_BASENAME_DEFAULT());
        property.output_format_kind(OutputFormatKind::CSV_FORMAT);
        init = true;
    }

    return property;
}


/*
 * In the xml configuration, under the property tag for the storage plugin, a
 * collection of name/value pairs can be passed. In this case, this example
 * chooses to define a property to name the filename to use.
 */
UtilsStorageWriter::UtilsStorageWriter(
        const rti::routing::PropertySet& properties) :
    StorageWriter(properties),
    property_(PROPERTY_DEFAULT()),
    csv_property_(PrintFormatCsv::PROPERTY_DEFAULT())
{
    
    // verbosity
    rti::routing::PropertySet::const_iterator found =
            properties.find(LOGGING_VERBOSITY_PROPERTY_NAME());
    if (found != properties.end()) {
        rti::config::Verbosity verbosity =
                rti::config::Verbosity::EXCEPTION;
        // convert to verbosity level
        switch (std::stoi(found->second)) {
        case 0:
            verbosity = rti::config::Verbosity::SILENT;
            break;
        case 1:
            verbosity = rti::config::Verbosity::EXCEPTION;
            break;
        case 2:
            verbosity = rti::config::Verbosity::WARNING;
            break;
        case 3:
            verbosity = rti::config::Verbosity::STATUS_LOCAL;
            break;
        case 4:
            verbosity = rti::config::Verbosity::STATUS_REMOTE;
            break;
        default:
            verbosity = rti::config::Verbosity::STATUS_ALL;
            break;
        }
        Logger::instance().verbosity(verbosity);
    }

    // output directory
    found = properties.find(OUTPUT_DIR_PROPERTY_NAME());
    if (found != properties.end()) {        
        property_.output_dir_path(found->second);        
    }

    // output file base name
    found = properties.find(OUTPUT_FILE_BASENAME_PROPERTY_NAME());
    if (found != properties.end()) {
        property_.output_file_basename(found->second);
    }

    // output format
    found = properties.find(OUTPUT_FORMAT_PROPERTY_NAME());
    if (found != properties.end()) {
        if (found->second != "CSV") {
            throw dds::core::UnsupportedError(
                    "unsupported output format=" + found->second);
        }
    }

    // merge output files
    found = properties.find(OUTPUT_MERGE_PROPERTY_NAME());
    if (found != properties.end()) {
        if (found->second == "true") {
            property_.merge_output_files(true);            
        } else if (found->second == "false") {
            property_.merge_output_files(false);
        } else {
            throw dds::core::Error(
                    "invalid value for property with name="
                    + OUTPUT_MERGE_PROPERTY_NAME());
        }
    }
    if (property_.merge_output_files()) {
        // build output file name
        std::string output_file_name =
                property_.output_dir_path()
                + "/"
                + property_.output_file_basename()
                + CSV_FILE_EXTENSION();
        output_merged_file_.open(output_file_name);
        if (!output_merged_file_.good()) {
            throw dds::core::Error("failed to open output merged file="
                    + output_file_name);
        }
    }

    // CSV-specific properties
    // representation for the empty member
    found = properties.find(CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME());
    if (found != properties.end()) {
        csv_property_.empty_member_value_representation(found->second);
    }
    
    // print enumerations as strings
    found = properties.find(CSV_ENUM_AS_STRING_PROPERTY_NAME());
    if (found != properties.end()) {
        if (found->second == "true") {
            csv_property_.enum_as_string(true);            
        } else if (found->second == "false") {
            csv_property_.enum_as_string(false);
        } else  {
            throw dds::core::Error(
                    "invalid value for property with name="
                    + CSV_ENUM_AS_STRING_PROPERTY_NAME());
        }
    }

    /* Log summary of configuration */
    if (Logger::instance().verbosity().underlying() 
            >= rti::config::Verbosity::STATUS_LOCAL) {
        size_t namespace_length = PROPERTY_NAMESPACE().length() + 1;
        std::ostringstream summary;

        summary << "Utils Storage plug-in configuration:" << "\n";
        
        summary << "\t" << OUTPUT_DIR_PROPERTY_NAME().substr(namespace_length)
                << "="
                << property_.output_dir_path()
                << "\n";

        summary << "\t" << OUTPUT_FILE_BASENAME_PROPERTY_NAME().substr(namespace_length)
                << "="
                << property_.output_file_basename()
                << "\n";

        summary << "\t" << OUTPUT_MERGE_PROPERTY_NAME().substr(namespace_length)
                << "="
                << (property_.merge_output_files() ? "true" : "false")
                << "\n";

        summary << "\t" << LOGGING_VERBOSITY_PROPERTY_NAME().substr(namespace_length)
                << "="
                << Logger::instance().verbosity()
                << "\n";

        summary << "\t" << CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME().substr(namespace_length)
                << "="
                << csv_property_.empty_member_value_representation()
                << "\n";

        summary << "\t" << CSV_ENUM_AS_STRING_PROPERTY_NAME().substr(namespace_length)
                << "="
                << (csv_property_.enum_as_string() ? "true" : "false");
        
        RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::STATUS_LOCAL,
                    summary.str().c_str());        
    }    
}

UtilsStorageWriter::~UtilsStorageWriter()
{
    if (property_.merge_output_files()) {
        try {
            RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::STATUS_LOCAL,
                    "UtilsStorageWriter: delete output files after merge");
            for (auto it = output_files_.begin();
                 it != output_files_.end();
                 ++it) {
                std::remove(it->first.c_str());
            }
        } catch (const std::exception& ex) {
            RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::EXCEPTION,
                    ex.what());
        } catch (...) {
            RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::EXCEPTION,
                    "unexpected exception occurred while deleting output files");
        }
    }
}

rti::recording::storage::StorageStreamWriter *
UtilsStorageWriter::create_stream_writer(
        const rti::routing::StreamInfo& stream_info,
        const rti::routing::PropertySet&)
{
    std::string output_file_path =
            property_.output_dir_path()
            + "/"
            + property_.output_file_basename()
            + "-"
            + stream_info.stream_name()
            + CSV_FILE_EXTENSION();
    std::ofstream output_file;
    output_file.open(output_file_path.c_str(), std::ios::out);
    if (!output_file.good()) {
        throw std::runtime_error(
                "Failed to open file="
                + output_file_path
                + " to store data samples for stream with name="
                + stream_info.stream_name());
    }
    // Write table header
    output_file << "Topic name: " << stream_info.stream_name() << std::endl;
    // add to collection
    output_files_.insert(std::make_pair(
            output_file_path,
            std::move(output_file)));

    RTI_RECORDER_UTILS_LOG_MESSAGE(
            rti::config::Verbosity::STATUS_LOCAL,
            ("UtilsStorageWriter: create StreamWriter for file=" + output_file_path).c_str());

    switch(property_.output_format_kind().underlying()) {

    case OutputFormatKind::CSV_FORMAT:
    {
        return new CsvStreamWriter(
            csv_property_,
            stream_info,
            *(output_files_.rbegin()));
    }
        break;

    default:
        throw dds::core::UnsupportedError("unsupported output format kind");
    };
    
}

void UtilsStorageWriter::delete_stream_writer(
        rti::recording::storage::StorageStreamWriter *writer)
{
    CsvStreamWriter *stream_writer = static_cast<CsvStreamWriter*>(writer);
    if (property_.merge_output_files()) {
        try {
            RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::STATUS_LOCAL,
                    ("UtilsStorageWriter: merge file=" + stream_writer->file_entry().first).c_str());
            stream_writer->file_entry().second.flush();
            merge_output_file(stream_writer->file_entry());
        } catch (const std::exception& ex) {
            RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Vferbosity::EXCEPTION,
                    ex.what());
        } catch (...) {
            std::string message =
                    "unexpected exception occurred while merging file with name="
                    + stream_writer->file_entry().first;
            RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::EXCEPTION,
                    message);
        }
    }
    
    delete writer;
}

void UtilsStorageWriter::merge_output_file(
        FileSetEntry& file_entry)
{
    std::ifstream input_file(file_entry.first);
    std::string line;
    while (std::getline(input_file, line)) {
        output_merged_file_ << line << std::endl;
    }
}


/*
 * --- CsvStreamWriter --------------------------------------------------------
 */

CsvStreamWriter::CsvStreamWriter(
            const PrintFormatCsvProperty& property,
            const rti::routing::StreamInfo& stream_info,
            UtilsStorageWriter::FileSetEntry& output_file_entry) :
    output_file_entry_(output_file_entry),
    print_format_csv_(
            property,
            dynamic_type(stream_info),
            output_file_entry.second),
    stored_sample_count_(0)
{
}

CsvStreamWriter::~CsvStreamWriter()
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
void CsvStreamWriter::store(
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

        output_file_entry_.second << timestamp;
        // now print sample data
        if (sample_info->valid()) {
            DDS_UnsignedLong data_as_csv_length = 0;
            
            // compute required size
            DDS_ReturnCode_t native_retcode = DDS_DynamicDataFormatter_to_string_w_format(
                    &sample_seq[i]->native(),
                    NULL,
                    &data_as_csv_length,
                    print_format_csv_.native());
            rti::core::check_return_code(
                    native_retcode,
                    "failed to compute CSV data required length");
            data_as_csv_.resize(data_as_csv_length);
            native_retcode = DDS_DynamicDataFormatter_to_string_w_format(
                    &sample_seq[i]->native(),
                    &data_as_csv_[0],
                    &data_as_csv_length,
                    print_format_csv_.native());
            rti::core::check_return_code(
                    native_retcode,
                    "failed convert to DynamicData to CSV");
            // add formatted sample content to file
            output_file_entry_.second << data_as_csv_;
            // end of row
            output_file_entry_ .second << std::endl;
        }
        stored_sample_count_++;
    }
}

UtilsStorageWriter::FileSetEntry& CsvStreamWriter::file_entry()
{
    return output_file_entry_;
}

} } } 


