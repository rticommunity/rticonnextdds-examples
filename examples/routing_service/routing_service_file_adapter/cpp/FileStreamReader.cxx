/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include <sstream>
#include <thread>

#include <rti/core/Exception.hpp>
#include "FileStreamReader.hpp"

using namespace dds::core::xtypes;
using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::community::examples;

const std::string FileStreamReader::INPUT_FILE_PROPERTY_NAME = "example.adapter.input_file";
const std::string FileStreamReader::SAMPLE_PERIOD_PROPERTY_NAME = "example.adapter.sample_period_sec";

void FileStreamReader::file_reading_thread()
{
    while (!stop_thread_) {
        if (input_file_stream_.is_open()) {
            std::getline(input_file_stream_, buffer_);

            /** 
             * Here we notify routing service, that there is data available 
             * on the StreamReader, triggering a call to take().
             */
            if (!input_file_stream_.eof()) {
                reader_listener_->on_data_available(this);
            } else {
                stop_thread_ = true;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(sampling_period_));
    }
    std::cout << "Reached end of stream for file: " << input_file_name_ << std::endl;
    file_connection_->dispose_discovery_stream(stream_info_);
}

FileStreamReader::FileStreamReader(
        FileConnection *connection, 
        const StreamInfo &info,
        const PropertySet &properties,
        StreamReaderListener *listener)
        : filereader_thread_(),
        sampling_period_(1), 
        stop_thread_(false), 
        stream_info_(info.stream_name(), info.type_info().type_name())
{
    file_connection_ = connection;
    reader_listener_ = listener;
    adapter_type_ = static_cast<DynamicType *>(
            info.type_info().type_representation());

    // Parse the properties provided in the xml configuration file
    for (const auto &property : properties) {
        if (property.first == INPUT_FILE_PROPERTY_NAME) {
            input_file_name_ = property.second;
            input_file_stream_.open(property.second);
        } else if (property.first == SAMPLE_PERIOD_PROPERTY_NAME) {
            sampling_period_ = std::stoi(property.second);
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

    filereader_thread_ = std::thread(&FileStreamReader::file_reading_thread, this);
}

void FileStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    std::istringstream s(buffer_);
    std::string color;
    std::string x;
    std::string y;
    std::string shapesize;

    // Reading input from the csv file
    std::getline(s, color, ',');
    std::getline(s, x, ',');
    std::getline(s, y, ',');
    std::getline(s, shapesize, ',');

    samples.resize(1);
    infos.resize(1);

    DynamicData *sample = new DynamicData(*adapter_type_);

    /** 
     * This is the hardcoded type information about ShapeType.
     * You are advised to change this as per your type definition
     */
    sample->value("color", color);
    sample->value("x", std::stoi(x));
    sample->value("y", std::stoi(y));
    sample->value("shapesize", std::stoi(shapesize));

    dds::sub::SampleInfo *info = new dds::sub::SampleInfo();
    (*info)->native().valid_data = DDS_BOOLEAN_TRUE;

    samples[0] = sample;
    infos[0] = info;

    return;
}

void FileStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    take(samples, infos);
    return;
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
    samples.clear();
}

FileStreamReader::~FileStreamReader()
{
    stop_thread_ = true;
    filereader_thread_.join();
    input_file_stream_.close();
}
