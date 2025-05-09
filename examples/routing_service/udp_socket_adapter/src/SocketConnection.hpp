/*
 * (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef SOCKETCONNECTION_HPP
#define SOCKETCONNECTION_HPP

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/Connection.hpp>

 #include "SocketInputDiscoveryStreamReader.hpp"

/*
 * This class creates the RS Connection, which is an access point to our
 * example data domain (a UDP socket)
 */
class SocketConnection : public rti::routing::adapter::Connection {
public:
    SocketConnection(
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

    // This function will also stop the receiving socket thread
    void delete_stream_reader(
            rti::routing::adapter::StreamReader *reader) final;

    rti::routing::adapter::DiscoveryStreamReader *
            input_stream_discovery_reader() final;

    /**
     * @brief This function is called by the SocketStreamReader to indicate
     * that it's time to dispose the route. The dispose set by the
     * SocketInputDiscoveryStreamReader starts the chain of cleanup procedure.
     *
     * @param stream_info \b in. Reference to a StreamInfo object which should
     * be used when creating a new StreamInfo sample with disposed set to true
     */
    void dispose_discovery_stream(
            const rti::routing::StreamInfo &stream_info);

 private:
     SocketInputDiscoveryStreamReader input_discovery_reader_;
};

#endif
