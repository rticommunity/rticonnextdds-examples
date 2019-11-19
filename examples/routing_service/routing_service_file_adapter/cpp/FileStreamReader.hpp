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

#ifndef FILESTREAMREADER_HPP
#define FILESTREAMREADER_HPP

#include <fstream>
#include <iostream>
#include <thread>

#include "FileConnection.hpp"

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamReader.hpp>

namespace rti { namespace community { namespace examples {

class FileStreamReader : public rti::routing::adapter::DynamicDataStreamReader {
public:
    FileStreamReader(
            FileConnection *connection,
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &,
            rti::routing::adapter::StreamReaderListener *listener);

    void
            take(std::vector<dds::core::xtypes::DynamicData *> &,
                 std::vector<dds::sub::SampleInfo *> &) final;

    void take(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &,
            const rti::routing::adapter::SelectorState &selector_state) final;

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &) final;

    void shutdown_file_reader_thread();

    bool check_csv_file_line_format(const std::string &line);

    bool is_digit(const std::string &value);

    ~FileStreamReader();

private:
    static const std::string INPUT_FILE_PROPERTY_NAME;
    static const std::string SAMPLE_PERIOD_PROPERTY_NAME;

    /**
     * @brief Function used by filereader_thread_ to read samples from the
     * CSV formatted file one line at a time. The file only contains data and
     * no meta data information.
     */
    void file_reading_thread();

    FileConnection *file_connection_;
    rti::routing::adapter::StreamReaderListener *reader_listener_;
    std::thread filereader_thread_;
    bool stop_thread_;
    std::chrono::seconds sampling_period_;

    std::ifstream input_file_stream_;
    std::string input_file_name_;
    std::string buffer_;
    std::mutex buffer_mutex_;

    rti::routing::StreamInfo stream_info_;
    dds::core::xtypes::DynamicType *adapter_type_;
};

}}}  // namespace rti::community::examples

#endif
