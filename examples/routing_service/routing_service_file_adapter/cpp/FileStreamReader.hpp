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

namespace rti { namespace community { namespace examples {

class FileStreamReader : 
        public rti::routing::adapter::DynamicDataStreamReader {
public:
    FileStreamReader(
            FileConnection *connection, 
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &,
            rti::routing::adapter::StreamReaderListener *listener);

    void take(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &);

    void take(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &,
            const rti::routing::adapter::SelectorState &selector_state);

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &);

    ~FileStreamReader();

private:

    static const std::string INPUT_FILE_PROPERTY_NAME;
    static const std::string SAMPLE_PERIOD_PROPERTY_NAME;

    void file_reading_thread();

    FileConnection *file_connection_;
    rti::routing::adapter::StreamReaderListener *reader_listener_;
    std::thread filereader_thread_;
    bool stop_thread_;
    uint16_t sampling_period_;

    std::ifstream input_file_stream_;
    std::string input_file_name_;
    std::string buffer_;

    rti::routing::StreamInfo stream_info_;
    dds::core::xtypes::DynamicType *adapter_type_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif
