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
}

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
}

void FileConnection::delete_stream_writer(StreamWriter *writer)
{
    delete writer;
}

DiscoveryStreamReader *FileConnection::output_stream_discovery_reader()
{
    return nullptr;
}

DiscoveryStreamReader *FileConnection::input_stream_discovery_reader()
{
    return &input_discovery_reader_;
}

void FileConnection::dispose_discovery_stream(
        const rti::routing::StreamInfo &stream_info)
{
    input_discovery_reader_.dispose(stream_info);
}
