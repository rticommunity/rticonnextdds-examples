/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#ifndef FILECONNECTION_HPP
#define FILECONNECTION_HPP

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/Connection.hpp>

#include "FileInputDiscoveryStreamReader.hpp"

namespace rti {
namespace community {
namespace examples {
using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::routing::adapter::detail;

class FileConnection : public Connection {
public:

    FileConnection(
            StreamReaderListener *input_stream_discovery_listener,
            StreamReaderListener *output_stream_discovery_listener,
            const PropertySet &properties) :
            input_discovery_reader_(properties)
    {
        /** 
         * Once the FileConnection is initialized, we trigger an event 
         * to notify that the streams are ready.
         */
        input_stream_discovery_listener_ = input_stream_discovery_listener;
        input_stream_discovery_listener_->on_data_available(&input_discovery_reader_);
    };

    StreamReader *create_stream_reader(
            Session *session,
            const StreamInfo &info,
            const PropertySet &properties,
            StreamReaderListener *listener);

    void delete_stream_reader(StreamReader *reader);

    StreamWriter *create_stream_writer(
            Session *session,
            const StreamInfo &info,
            const PropertySet &properties);

    void delete_stream_writer(StreamWriter *writer);

    DiscoveryStreamReader *input_stream_discovery_reader() {
        return &input_discovery_reader_;
    };

    DiscoveryStreamReader *output_stream_discovery_reader() {
        return NULL;
    };

    void dispose_discovery_streams();

private:
    FileInputDiscoveryStreamReader input_discovery_reader_;
    StreamReaderListener *input_stream_discovery_listener_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif