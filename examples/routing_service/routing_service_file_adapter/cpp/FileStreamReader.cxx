/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include <thread>
#include <sstream>

#include "FileStreamReader.hpp"

using namespace rti::community::examples;
using namespace dds::core::xtypes;

void FileStreamReader::ProcessThread() {
    std::cout << "Process thread started" << std::endl;
    stop_thread_ = false;

    while (!stop_thread_) {
        std::cout << "Process" << std::endl;

        std::getline(inputfile_, buffer_);
        // Here we notify routing service, that there is data available
        // on the StreamReader, triggering a call to take().
        reader_listener_->on_data_available(this);

        //TODO: Configure sleep period
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

FileStreamReader::FileStreamReader(
        const StreamInfo &info,
        const PropertySet &property,
        StreamReaderListener *listener) :
                filereader_thread_(&FileStreamReader::ProcessThread, this)
{
    reader_listener_ = listener;
    adapter_type_ = static_cast<DynamicType *>(info.type_info().type_representation());

    // TODO: Parametrize file name from property
    inputfile_.open("samples_in.data");
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
    std::cout << "Calling take()" << std::endl;

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
    for(int i=0; i < samples.size(); ++i) {
        delete samples[i];
        delete infos[i];
    }
    samples.clear();
    infos.clear();
}

void *FileStreamReader::create_content_query(void *, const dds::topic::Filter &)
{
    // TODO: Implement me
    return NULL;
}

void FileStreamReader::delete_content_query(void *)
{
    // TODO: Implement me
}