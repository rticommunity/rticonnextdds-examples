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
        const PropertySet &properties)
        : input_discovery_reader_(
                properties,
                input_stream_discovery_listener) {};

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
    FileStreamReader *file_reader = dynamic_cast<FileStreamReader *>(reader);
    file_reader->shutdown_file_reader_thread();
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
    return nullptr;
};

DiscoveryStreamReader *FileConnection::input_stream_discovery_reader()
{
    return &input_discovery_reader_;
};

void FileConnection::dispose_discovery_stream(
        const rti::routing::StreamInfo &stream_info)
{
    input_discovery_reader_.dispose(stream_info);
}
