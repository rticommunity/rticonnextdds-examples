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

#include "RsConnection.hpp"
#include "RsLog.hpp"
#include "RsStreamReader.hpp"
#include "RsStreamWriter.hpp"

#define RTI_RS_LOG_ARGS "RsConnection"

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::routing::adapter::detail;

RsConnection::RsConnection(
        StreamReaderListener *input_stream_discovery_listener,
        StreamReaderListener *output_stream_discovery_listener,
        const PropertySet &properties)
        : input_discovery_stream_reader_(
                properties,
                input_stream_discovery_listener)
{
}

/*
 * This method is called for each Route's output when the associated 'creation
 * mode' condition is met. The operation is called on the output Connection as a
 * specified through the 'connection' attribute in the <output> tag.
 *
 * Returns - New StreamWriter if successful.
 */
auto RsConnection::create_stream_writer(
        Session *session,
        const StreamInfo &stream_info,
        const PropertySet &properties) -> StreamWriter *
{
    RTI_RS_LOG_FN(create_stream_writer);

    return new RsStreamWriter(stream_info, properties);
}

void RsConnection::delete_stream_writer(StreamWriter *stream_writer)
{
    RTI_RS_LOG_FN(delete_stream_writer);

    delete stream_writer;
}

/*
 * This method is called for each Route's input when the associated 'creation
 * mode' condition is met. The operation is called on the input Connection as a
 * specified through the 'connection' attribute in the <input> tag.
 *
 * Returns - New StreamReader if successful.
 */
auto RsConnection::create_stream_reader(
        Session *session,
        const StreamInfo &stream_info,
        const PropertySet &properties,
        StreamReaderListener *listener) -> StreamReader *
{
    RTI_RS_LOG_FN(create_stream_reader);

    return new RsStreamReader(this, stream_info, properties, listener);
}

void RsConnection::delete_stream_reader(StreamReader *stream_reader)
{
    RTI_RS_LOG_FN(delete_stream_reader);

    delete stream_reader;
}

auto RsConnection::input_stream_discovery_reader() -> DiscoveryStreamReader *
{
    RTI_RS_LOG_FN(input_stream_discovery_reader);

    return &input_discovery_stream_reader_;
};

void RsConnection::dispose_discovery_stream(const StreamInfo &stream_info)
{
    RTI_RS_LOG_FN(dispose_discovery_stream);

    input_discovery_stream_reader_.dispose(stream_info);
}
