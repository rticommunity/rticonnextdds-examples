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
#ifndef RTI_RECORDER_UTILS_STORAGE_WRITER_HPP_
#define RTI_RECORDER_UTILS_STORAGE_WRITER_HPP_

#include <fstream>

#include "rti/recording/storage/StorageWriter.hpp"
#include "rti/recording/storage/StorageStreamWriter.hpp"
#include "rti/recording/storage/StorageDiscoveryStreamWriter.hpp"

#include "PrintFormatCsv.hpp"

namespace rti { namespace recorder { namespace utils {


/*
 * @brief Convenience macro to forward-declare the C-style function that will be
 * called by RTI Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_WRITER_CREATE_DECL(UtilsStorageWriter);

/*
 * @brief Definition of the support output formats
 */
struct OutputFormatKind_def {
    enum type {
        CSV_FORMAT,
        XML_FORMAT /* unsupported */
    };
};

typedef dds::core::safe_enum<OutputFormatKind_def> OutputFormatKind;

class UtilsStorageProperty {
public:
    UtilsStorageProperty();

    OutputFormatKind output_format_kind() const;
    void output_format_kind(const OutputFormatKind&);

    std::string output_dir_path() const;
    void output_dir_path(const std::string&);

    std::string output_file_basename() const;
    void output_file_basename(const std::string&);

    bool merge_output_files() const;
    void merge_output_files(bool);

private:
    OutputFormatKind output_format_kind_;
    std::string output_dir_path_;
    std::string output_file_basename_;
    bool merge_output_files_;
};


/*
 * This class acts as a factory for Stream Writer objects, that store data
 * samples in a text file, transforming them from dynamic data representation
 * into text.
 * This storage writer creates three files: 1) a data file with the text
 * samples; 2) a publication file, containing a subset of the information in the
 * DCPSPublication built-in discovery topic samples; and 3) an info file, that
 * only contains the starting and ending points in time where there are data
 * samples.
 */
class UtilsStorageWriter : public rti::recording::storage::StorageWriter {
public:
    typedef std::map<std::string, std::ofstream> OutputFileSet;
    typedef OutputFileSet::value_type FileSetEntry;

    UtilsStorageWriter(const rti::routing::PropertySet& properties);
    virtual ~UtilsStorageWriter();

    static const UtilsStorageProperty& PROPERTY_DEFAULT();
    static const std::string& PROPERTY_NAMESPACE();
    static const std::string& OUTPUT_DIR_PROPERTY_NAME();
    static const std::string& OUTPUT_FILE_BASENAME_PROPERTY_NAME();
    static const std::string& OUTPUT_FORMAT_PROPERTY_NAME();
    static const std::string& OUTPUT_MERGE_PROPERTY_NAME();   
    static const std::string& LOGGING_VERBOSITY_PROPERTY_NAME();
    static const std::string& CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME();
    static const std::string& CSV_ENUM_AS_STRING_PROPERTY_NAME();

    static const std::string& CSV_FILE_EXTENSION();
    static const std::string& OUTPUT_FILE_BASENAME_DEFAULT();

    /*
     * Recording Service will call this method to create a Stream Writer object
     * associated with a user-data topic that has been discovered.
     * The property set passed as a parameter contains information about the
     * stream not provided by the stream info object. For example, Recording
     * Service will add the DDS domain ID as a property to this set.
     */
    rti::recording::storage::StorageStreamWriter * create_stream_writer(
            const rti::routing::StreamInfo& stream_info,
            const rti::routing::PropertySet& properties);

    /*
     * Recording Service will call this method to delete a previously created
     * Stream Writer (no matter if it was created with the create_stream_writer()
     * or create_discovery_stream_writer() method).
     */
    void delete_stream_writer(
            rti::recording::storage::StorageStreamWriter *writer);

private:
    void merge_output_file(FileSetEntry& file_entry);
    
    UtilsStorageProperty property_;
    // Collection of output files, one for each stream
    OutputFileSet output_files_;
    // The final file if merging is enabled
    std::ofstream output_merged_file_;
    // Property per output kind
    PrintFormatCsvProperty csv_property_;
};

/**
 * This class implements the provided DynamicData specialization of a
 * StorageStreamWriter to transform dynamic data objects into a text
 * representation that it stores into a file. 
 */
class CsvStreamWriter :
        public rti::recording::storage::DynamicDataStorageStreamWriter {
public:
    CsvStreamWriter(
            const PrintFormatCsvProperty& property,
            const rti::routing::StreamInfo& stream_info,
            UtilsStorageWriter::FileSetEntry& output_file_entry);

    virtual ~CsvStreamWriter();

    void store(
            const std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
            const std::vector<dds::sub::SampleInfo *>& info_seq);

    UtilsStorageWriter::FileSetEntry& file_entry();

private:
    // PrintFormat implementation used to convert data samples
    PrintFormatCsv print_format_csv_;
    uint64_t stored_sample_count_;
    UtilsStorageWriter::FileSetEntry& output_file_entry_;
    // a buffer to represent a single CSV sample
    std::string data_as_csv_;
};

} } } 

#endif

