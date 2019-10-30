/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileConnection.hpp"
#include "FileStreamReader.hpp"
#include "FileStreamWriter.hpp"

using namespace rti::community::examples;
using namespace rti::routing;
using namespace rti::routing::adapter;

FileConnection::FileConnection(
        StreamReaderListener *input_stream_discovery_listener,
        StreamReaderListener *output_stream_discovery_listener,
        const PropertySet &properties) :
        input_discovery_reader_(properties, input_stream_discovery_listener)
{
};

StreamReader *FileConnection::create_stream_reader(
        Session *session,
        const StreamInfo &info,
        const PropertySet &properties,
        StreamReaderListener *listener)
{
    return new FileStreamReader(this, info, properties, listener);
};

void FileConnection::delete_stream_reader(StreamReader *reader)
{
    delete reader;
}

StreamWriter *FileConnection::create_stream_writer(
        Session *session,
        const StreamInfo &info,
        const PropertySet &properties)
{
    return new FileStreamWriter(properties);
};

void FileConnection::delete_stream_writer(StreamWriter *writer)
{
    delete writer;
}

DiscoveryStreamReader *FileConnection::output_stream_discovery_reader() 
{
    return NULL;
};

DiscoveryStreamReader *FileConnection::input_stream_discovery_reader() 
{
    return &input_discovery_reader_;
};

/**
 * This function is called by the FileStreamReader to indicate that it has 
 * reached EOF and its time to dispose the route. The dispose set by the 
 * FileInputDiscoveryStreamReader starts the chain of cleanup procedure. Remember 
 * that the <creation_mode> for <output> should be ON_ROUTE_MATCH for the cleanup 
 * to be propagated to the StreamWriter as well.
 */
void FileConnection::dispose_discovery_stream(
        const rti::routing::StreamInfo &stream_info)
{
    input_discovery_reader_.dispose(stream_info);
}
