/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileAdapter.hpp"
#include "FileConnection.hpp"

using namespace rti::community::examples;
using namespace rti::routing;
using namespace rti::routing::adapter;

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
};

void FileAdapter::delete_connection(Connection *connection)
{
    /**
     * Perform cleanup pertaining to the connection object here. 
     * In this example there is nothing to be done.
     */
}

rti::config::LibraryVersion FileAdapter::get_version() const
{
    return rti::config::LibraryVersion(1, 0, 0, 'r');
};

RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DEF(FileAdapter)
