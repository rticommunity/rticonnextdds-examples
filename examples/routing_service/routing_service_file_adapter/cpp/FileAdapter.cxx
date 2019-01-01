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

Connection *FileAdapter::create_connection(
        rti::routing::adapter::detail::StreamReaderListener *,
        rti::routing::adapter::detail::StreamReaderListener *,
        const PropertySet &)
{
    return new FileConnection();
};

rti::config::LibraryVersion FileAdapter::get_version() const
{
    return rti::config::LibraryVersion(6, 0, 0, 'r');
};

RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DEF(FileAdapter)
