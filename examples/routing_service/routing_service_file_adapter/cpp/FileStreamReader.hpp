/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#ifndef FILESTREAMREADER_HPP
#define FILESTREAMREADER_HPP

#include <fstream>
#include <iostream>
#include <thread>

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamReader.hpp>

namespace rti {
namespace community {
namespace examples {

using namespace rti::routing;
using namespace rti::routing::adapter;

class FileStreamReader : public DynamicDataStreamReader {
public:
    FileStreamReader(
            const StreamInfo &info,
            const PropertySet &,
            StreamReaderListener *listener);

    void read(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &);

    void read(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &,
            const SelectorState &selector_state);

    void take(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &);

    void take(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &,
            const SelectorState &selector_state);

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &);

    void *create_content_query(void *, const dds::topic::Filter &);

    void delete_content_query(void *);


    ~FileStreamReader()
    {
        stop_thread_ = true;
        filereader_thread_.join();
        inputfile_.close();
    }

private:

    static const std::string INPUT_FILE_PROPERTY_NAME;

    void ProcessThread();

    StreamReaderListener *reader_listener_;
    std::thread filereader_thread_;
    bool stop_thread_;

    std::ifstream inputfile_;
    std::string buffer_;
    dds::core::xtypes::DynamicType *adapter_type_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif