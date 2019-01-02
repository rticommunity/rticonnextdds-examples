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

StreamReader *FileConnection::create_stream_reader(
        Session *session,
        const StreamInfo &info,
        const PropertySet &properties,
        StreamReaderListener *listener)
{
    FileStreamReader * fsr = NULL;

    try {
        fsr = new FileStreamReader(info, properties, listener);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return NULL;
    }

    return fsr;
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
