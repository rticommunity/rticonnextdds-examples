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

#include "MongoAdapter.hpp"
#include "MongoConnection.hpp"

using namespace rti::community::examples;
using namespace rti::routing;
using namespace rti::routing::adapter;


MongoAdapter::MongoAdapter(PropertySet&)
    :instance_{}
{
}

Connection *MongoAdapter::create_connection(
        rti::routing::adapter::detail::StreamReaderListener*,
        rti::routing::adapter::detail::StreamReaderListener*,
        const PropertySet &properties)
{
    return new MongoConnection(properties);
}

void MongoAdapter::delete_connection(Connection *connection)
{
    delete connection;
}

rti::config::LibraryVersion MongoAdapter::get_version() const
{
    return { 1, 0, 0, 'r' };
};

RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DEF(MongoAdapter)
