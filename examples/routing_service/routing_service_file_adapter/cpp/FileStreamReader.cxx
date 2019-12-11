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
#include <cctype>
#include <sstream>
#include <thread>

#include "FileStreamReader.hpp"
#include <rti/core/Exception.hpp>

using namespace dds::core::xtypes;
using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::community::examples;

const std::string FileStreamReader::INPUT_FILE_PROPERTY_NAME =
        "example.adapter.input_file";
const std::string FileStreamReader::SAMPLE_PERIOD_PROPERTY_NAME =
        "example.adapter.sample_period_sec";

bool FileStreamReader::check_csv_file_line_format(const std::string &line)
{
    return (!line.empty()) && (std::count(line.begin(), line.end(), ',') == 3);
}

bool FileStreamReader::is_digit(const std::string &value)
{
    return std::find_if(
                   value.begin(),
                   value.end(),
                   [](unsigned char c) { return !std::isdigit(c); })
            == value.end();
}

void FileStreamReader::file_reading_thread()
{
    while (!stop_thread_) {
        if (input_file_stream_.is_open()) {
            {
                /**
                 * Essential to protect against concurrent data access to
                 * buffer_ from the take() methods running on a different
                 * Routing Service thread.
                 */
                std::lock_guard<std::mutex> guard(buffer_mutex_);
                std::getline(input_file_stream_, buffer_);
            }

            /**
             * Here we notify Routing Service, that there is data available
             * on the StreamReader, triggering a call to take().
             */
            if (!input_file_stream_.eof()) {
                reader_listener_->on_data_available(this);
            } else {
                stop_thread_ = true;
            }
        }

        std::this_thread::sleep_for(sampling_period_);
    }
    std::cout << "Reached end of stream for file: " << input_file_name_
              << std::endl;
    file_connection_->dispose_discovery_stream(stream_info_);
}

FileStreamReader::FileStreamReader(
        FileConnection *connection,
        const StreamInfo &info,
        const PropertySet &properties,
        StreamReaderListener *listener)
        : sampling_period_(1),
          stop_thread_(false),
          stream_info_(info.stream_name(), info.type_info().type_name())
{
    file_connection_ = connection;
    reader_listener_ = listener;
    adapter_type_ =
            static_cast<DynamicType *>(info.type_info().type_representation());

    // Parse the properties provided in the xml configuration file
    for (const auto &property : properties) {
        if (property.first == INPUT_FILE_PROPERTY_NAME) {
            input_file_name_ = property.second;
            input_file_stream_.open(property.second);
        } else if (property.first == SAMPLE_PERIOD_PROPERTY_NAME) {
            sampling_period_ = std::chrono::seconds(std::stoi(property.second));
        }
    }

    if (input_file_name_.empty()) {
        throw dds::core::IllegalOperationError(
                "Error property not found: " + INPUT_FILE_PROPERTY_NAME);
    } else if (!input_file_stream_.is_open()) {
        throw dds::core::IllegalOperationError(
                "Error opening input file: " + input_file_name_);
    } else {
        std::cout << "Input file name: " << input_file_name_ << std::endl;
    }

    filereader_thread_ =
            std::thread(&FileStreamReader::file_reading_thread, this);
}

void FileStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    /**
     * This protection is required since take() executes on a different
     * Routing Service thread.
     */
    std::lock_guard<std::mutex> guard(buffer_mutex_);

    if (!check_csv_file_line_format(buffer_)) {
        std::cout << "Incorrect format for line: " << buffer_ << std::endl;
        return;
    }

    std::istringstream s(buffer_);
    std::string color;
    std::string x;
    std::string y;
    std::string shapesize;

    // Reading input from the CSV file
    std::getline(s, color, ',');
    std::getline(s, x, ',');
    std::getline(s, y, ',');
    std::getline(s, shapesize, ',');

    if (!(is_digit(x) && is_digit(y) && is_digit(shapesize))) {
        std::cout << "Incorrect values found at line: " << buffer_ << std::endl;
        return;
    }

    /**
     * Note that we read one line at a time from the CSV file in the
     * function file_reading_thread()
     */
    samples.resize(1);
    infos.resize(1);

    std::unique_ptr<DynamicData> sample(new DynamicData(*adapter_type_));

    /**
     * This is the hardcoded type information about ShapeType.
     * You are advised to change this as per your type definition
     */
    sample->value("color", color);
    sample->value("x", std::stoi(x));
    sample->value("y", std::stoi(y));
    sample->value("shapesize", std::stoi(shapesize));

    samples[0] = sample.release();

    return;
}

void FileStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    take(samples, infos);
}

void FileStreamReader::return_loan(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    for (int i = 0; i < samples.size(); ++i) {
        delete samples[i];
        delete infos[i];
    }
    samples.clear();
    infos.clear();
}

void FileStreamReader::shutdown_file_reader_thread()
{
    stop_thread_ = true;
    filereader_thread_.join();
}

FileStreamReader::~FileStreamReader()
{
    input_file_stream_.close();
}
