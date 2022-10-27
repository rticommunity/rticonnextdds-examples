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

#ifndef FILECONNECTION_HPP
#define FILECONNECTION_HPP

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/Connection.hpp>

#include "FileInputDiscoveryStreamReader.hpp"

namespace rti { namespace community { namespace examples {

class FileConnection : public rti::routing::adapter::Connection {
public:
    FileConnection(
            rti::routing::adapter::StreamReaderListener
                    *input_stream_discovery_listener,
            rti::routing::adapter::StreamReaderListener
                    *output_stream_discovery_listener,
            const rti::routing::PropertySet &properties);

    rti::routing::adapter::StreamReader *create_stream_reader(
            rti::routing::adapter::Session *session,
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &properties,
            rti::routing::adapter::StreamReaderListener *listener) final;

    void delete_stream_reader(
            rti::routing::adapter::StreamReader *reader) final;

    rti::routing::adapter::StreamWriter *create_stream_writer(
            rti::routing::adapter::Session *session,
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &properties) final;

    void delete_stream_writer(
            rti::routing::adapter::StreamWriter *writer) final;

    rti::routing::adapter::DiscoveryStreamReader *
            input_stream_discovery_reader() final;

    rti::routing::adapter::DiscoveryStreamReader *
            output_stream_discovery_reader() final;

    /**
     * @brief This function is called by the FileStreamReader to indicate that
     * it has reached EOF and its time to dispose the route. The dispose set by
     * the FileInputDiscoveryStreamReader starts the chain of cleanup procedure.
     * Remember that the <creation_mode> for <output> should be ON_ROUTE_MATCH
     * for the cleanup to be propagated to the StreamWriter as well.
     *
     * @param stream_info \b in. Reference to a StreamInfo object which should
     * be used when creating a new StreamInfo sample with disposed set to true
     */
    void dispose_discovery_stream(
            const rti::routing::StreamInfo &stream_info);

private:
    FileInputDiscoveryStreamReader input_discovery_reader_;
};

}}}  // namespace rti::community::examples

#endif
