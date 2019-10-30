/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileInputDiscoveryStreamReader.hpp"

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::community::examples;

const std::string FileInputDiscoveryStreamReader::SQUARE_FILE_NAME = "Input_Square.csv";
const std::string FileInputDiscoveryStreamReader::CIRCLE_FILE_NAME = "Input_Circle.csv";
const std::string FileInputDiscoveryStreamReader::TRIANGLE_FILE_NAME = "Input_Triangle.csv";

bool FileInputDiscoveryStreamReader::fexists(const std::string filename)
{
    std::ifstream input_file;
    input_file.open(filename);
    return input_file.is_open();
}

FileInputDiscoveryStreamReader::FileInputDiscoveryStreamReader( 
        const PropertySet &, 
        StreamReaderListener *input_stream_discovery_listener)
        : data_samples_mutex_(), 
        data_samples_()
{
    input_stream_discovery_listener_ = input_stream_discovery_listener;

    /**
     * In our example, we provide statically the stream information available. 
     * We do not have a mechanism demonstrating how to perform discovery after 
     * startup. However, as an idea you can have a thread monitoring the file 
     * system and updating the list of StreamInfo samples and calling 
     * input_stream_discovery_listener_->on_data_available(this); to notify that 
     * new files have been discovered.
     */
    if (fexists(SQUARE_FILE_NAME)) {
        this->data_samples_.push_back(new StreamInfo("Square", "ShapeType"));
    }

    if (fexists(CIRCLE_FILE_NAME)) {
        this->data_samples_.push_back(new StreamInfo("Circle", "ShapeType"));
    }

    if (fexists(TRIANGLE_FILE_NAME)) {
        this->data_samples_.push_back(new StreamInfo("Triangle", "ShapeType"));
    }

    /** 
     * Once the FileInputDiscoveryStreamReader is initialized, we trigger an 
     * event to notify that the streams are ready.
     */
    input_stream_discovery_listener_->on_data_available(this);
}

void FileInputDiscoveryStreamReader::dispose(
        const rti::routing::StreamInfo &stream_info)
{
    /**
     * This guard is essential since the take() and return_loan() operations triggered 
     * by calling on_data_available() execute on a different thread. The take() and 
     * return_loan() operations also need to access the data_samples_ list.
     */
    std::lock_guard<std::mutex> guard(data_samples_mutex_);

    rti::routing::StreamInfo *stream_info_disposed = new StreamInfo(
            stream_info.stream_name(), 
            stream_info.type_info().type_name());
    stream_info_disposed->disposed(true);

    this->data_samples_.push_back(stream_info_disposed);
    input_stream_discovery_listener_->on_data_available(this);
}

void FileInputDiscoveryStreamReader::take(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    /**
     * This guard is essential since the take() and return_loan() operations triggered 
     * by calling on_data_available() execute on a different thread. The take() and 
     * return_loan() operations also need to access the data_samples_ list.
     */
    std::lock_guard<std::mutex> guard(data_samples_mutex_);
    std::copy(data_samples_.begin(), data_samples_.end(), std::back_inserter(stream));
}

void FileInputDiscoveryStreamReader::return_loan(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    /**
     * This guard is essential since the take() and return_loan() operations triggered 
     * by calling on_data_available() execute on a different thread. The take() and 
     * return_loan() operations also need to access the data_samples_ list.
     */
    std::lock_guard<std::mutex> guard(data_samples_mutex_);

    auto iter = data_samples_.begin();
    for (int i = 0; i < stream.size(); i++) {
        delete *iter;
        iter = this->data_samples_.erase(iter);
    }
    stream.clear();
}
