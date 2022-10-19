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

#include "FileAdapter.hpp"
#include "FileConnection.hpp"

using namespace rti::community::examples;
using namespace rti::routing;
using namespace rti::routing::adapter;

FileAdapter::FileAdapter(PropertySet &properties)
{
}

Connection *FileAdapter::create_connection(
        rti::routing::adapter::detail::StreamReaderListener
                *input_stream_discovery_listener,
        rti::routing::adapter::detail::StreamReaderListener
                *output_stream_discovery_listener,
        const PropertySet &properties)
{
    FileConnection *fc = new FileConnection(
            input_stream_discovery_listener,
            output_stream_discovery_listener,
            properties);
    return fc;
}

void FileAdapter::delete_connection(Connection *connection)
{
    /**
     * Perform cleanup pertaining to the connection object here.
     */
    delete connection;
}

rti::config::LibraryVersion FileAdapter::get_version() const
{
    return { 1, 0, 0, 'r' };
}

RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DEF(FileAdapter)
