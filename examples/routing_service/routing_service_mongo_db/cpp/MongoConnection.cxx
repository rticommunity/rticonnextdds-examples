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

#include "MongoConnection.hpp"
//#include "FileStreamReader.hpp"
#include "MongoStreamWriter.hpp"

using namespace rti::community::examples;
using namespace rti::routing;
using namespace rti::routing::adapter;



/*
 * --- MongoConnection -------------------------------------------------------
 */

MongoConnection::MongoConnection(
        StreamReaderListener *input_stream_discovery_listener,
        StreamReaderListener *output_stream_discovery_listener,
        const PropertySet &properties)
        : client_pool_(mongocxx::uri(MongoConfig::parse<MongoConfig::URI>(properties))),
          db_name_(MongoConfig::parse<MongoConfig::DB_NAME>(properties))
//        ,input_discovery_reader_(
//                properties,
//                input_stream_discovery_listener),
{
}
                
/* --- Private interface ---*/       


/* --- Adapter Interface --- */

StreamReader *MongoConnection::create_stream_reader(
        Session *session,
        const StreamInfo &info,
        const PropertySet &properties,
        StreamReaderListener *listener)       
{
    //return new FileStreamReader(this, info, properties, listener);
    return nullptr;
};

void MongoConnection::delete_stream_reader(StreamReader *reader)
{
//    FileStreamReader *file_reader = dynamic_cast<FileStreamReader *>(reader);
//    file_reader->shutdown_file_reader_thread();
    delete reader;
}

StreamWriter *MongoConnection::create_stream_writer(
        Session *session,
        const StreamInfo &stream_info,
        const PropertySet &properties)
{
    return new MongoStreamWriter(
            *this,
            stream_info,
            db_name_,
            properties);
};

void MongoConnection::delete_stream_writer(StreamWriter *writer)
{
    delete writer;
}

DiscoveryStreamReader *MongoConnection::output_stream_discovery_reader()
{
    return nullptr;
};

DiscoveryStreamReader *MongoConnection::input_stream_discovery_reader()
{
    //return &input_discovery_reader_;
    return nullptr;
};

//void MongoConnection::dispose_discovery_stream(
//        const rti::routing::StreamInfo &stream_info)
//{
//    //input_discovery_reader_.dispose(stream_info);
//}