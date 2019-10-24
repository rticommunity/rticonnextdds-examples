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

#include "FileConnection.hpp"

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
            FileConnection *connection, 
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

    ~FileStreamReader()
    {
        stop_thread_ = true;
        filereader_thread_.join();
        input_file_stream_.close();
    }

private:

    static const std::string INPUT_FILE_PROPERTY_NAME;
    static const std::string SAMPLE_PERIOD_PROPERTY_NAME;

    void ProcessThread();

    FileConnection *file_connection_;
    StreamReaderListener *reader_listener_;
    std::thread filereader_thread_;
    bool stop_thread_;
    uint16_t sampling_period_;

    std::ifstream input_file_stream_;
    std::string input_file_name_;
    std::string buffer_;
    dds::core::xtypes::DynamicType *adapter_type_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif