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

#include "RsAdapterPlugin.hpp"
#include "RsConnection.hpp"
#include "RsLog.hpp"

#define RTI_RS_LOG_ARGS "RsAdapterPlugin"

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::routing::adapter::detail;

RsAdapterPlugin::RsAdapterPlugin(const PropertySet &properties)
{
    /* set adapter properties here */
}

RsAdapterPlugin::~RsAdapterPlugin()
{
    /* remove connections if any remain */
    std::for_each(
            connections_.begin(),
            connections_.end(),
            [](Connection *connection) { delete connection; });
    connections_.clear();
}

/*
 * Creates a Connection.
 * Connection objects are created when the domain routes that contain them are
 * enabled.
 *
 * Returns - new Connection if successful. Cannot return nullptr.
 * Exceptions - std::exception
 */
auto RsAdapterPlugin::create_connection(
        StreamReaderListener *input_stream_discovery_listener,
        StreamReaderListener *output_stream_discovery_listener,
        const PropertySet &properties) -> Connection *
{
    RTI_RS_LOG_FN(create_connection);

    auto *connection = new RsConnection(
            input_stream_discovery_listener,
            output_stream_discovery_listener,
            properties);
    if (connection != nullptr) {
        connections_.push_back(connection);
    } else {
        throw dds::core::Error("Error creating connection");
    }

    return connection;
}

void RsAdapterPlugin::delete_connection(Connection *connection)
{
    RTI_RS_LOG_FN(delete_connection)

    connections_.remove_if([](Connection *connection) {
        delete connection;
        return true;
    });
}

/* definition of entry point class */
RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DEF(RsAdapterPlugin)
