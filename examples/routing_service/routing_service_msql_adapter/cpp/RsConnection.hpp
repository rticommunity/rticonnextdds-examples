/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef RS_CONNECTION_HPP_
#define RS_CONNECTION_HPP_

#include "RsInputDiscoveryStreamReader.hpp"
#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/Connection.hpp>

/*
 * A Connection is responsible for tracking all the stream information that is
 * provided by the underlying input and output stream discovery StreamReaders.
 * The Connection gets notified about new or disposed streams and propagates
 * this information downstream to the Routes and AutoRoutes, which will process
 * and generate events accordingly.
 */
class RsConnection : public rti::routing::adapter::Connection {
public:
    RsConnection(
            rti::routing::adapter::StreamReaderListener
                    *inputStreamDiscoveryListener,
            rti::routing::adapter::StreamReaderListener
                    *outputStreamDiscoveryListener,
            const rti::routing::PropertySet &properties);

    virtual ~RsConnection() = default;

    auto create_stream_writer(
            rti::routing::adapter::Session *session,
            const rti::routing::StreamInfo &streamInfo,
            const rti::routing::PropertySet &properties)
            -> rti::routing::adapter::StreamWriter *;

    void delete_stream_writer(
            rti::routing::adapter::StreamWriter *stream_writer);

    auto create_stream_reader(
            rti::routing::adapter::Session *session,
            const rti::routing::StreamInfo &streamInfo,
            const rti::routing::PropertySet &properties,
            rti::routing::adapter::StreamReaderListener *listener)
            -> rti::routing::adapter::StreamReader *;

    void delete_stream_reader(
            rti::routing::adapter::StreamReader *streamReader);

    auto RsConnection::input_stream_discovery_reader()
            -> rti::routing::adapter::DiscoveryStreamReader *;

    void RsConnection::dispose_discovery_stream(
            const rti::routing::StreamInfo &streamInfo);

private:
    RsInputDiscoveryStreamReader inputDiscoveryStreamReader_;
};

#endif  /* RS_CONNECTION_HPP_ */
