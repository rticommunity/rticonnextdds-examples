/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include <sstream>
#include <thread>

#include "FileStreamReader.hpp"

using namespace rti::community::examples;
using namespace dds::core::xtypes;

const std::string FileStreamReader::INPUT_FILE_PROPERTY_NAME = "example.adapter.input_file";
const std::string FileStreamReader::SAMPLE_PERIOD_PROPERTY_NAME = "example.adapter.sample_period_sec";

void FileStreamReader::ProcessThread()
{
    stop_thread_ = false;

    while (!stop_thread_) {
        if (inputfile_.is_open()) {
            std::getline(inputfile_, buffer_);

            // Here we notify routing service, that there is data available
            // on the StreamReader, triggering a call to take().
            reader_listener_->on_data_available(this);
        }

        std::this_thread::sleep_for(std::chrono::seconds(sampling_period_));
    }
}

FileStreamReader::FileStreamReader(
        const StreamInfo &info,
        const PropertySet &property,
        StreamReaderListener *listener)
        : filereader_thread_(),
        sampling_period_(1)
{
    reader_listener_ = listener;
    adapter_type_ = static_cast<DynamicType *>(
            info.type_info().type_representation());

    // Parse the properties provided in the xml configuration file
    for (const auto &it : property) {
        if (it.first == INPUT_FILE_PROPERTY_NAME) {
            inputfile_.open(it.second);
        } else if (it.first == SAMPLE_PERIOD_PROPERTY_NAME) {
            sampling_period_ = std::stoi(it.second);
        }
    }

    if (!inputfile_.is_open()) {
        throw std::logic_error("Input file not provided or unable to open");
    }

    filereader_thread_ = std::thread(&FileStreamReader::ProcessThread, this);
}

void FileStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    take(samples, infos);
    return;
}


void FileStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    take(samples, infos);
    return;
}

void FileStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    std::istringstream s(buffer_);
    std::string id;
    std::string message;
    std::getline(s, id, ',');
    std::getline(s, message, ',');

    samples.resize(1);
    infos.resize(1);

    DynamicData *sample = new DynamicData(*adapter_type_);
    sample->value("id", std::stoi(id));
    sample->value("message", message);

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
