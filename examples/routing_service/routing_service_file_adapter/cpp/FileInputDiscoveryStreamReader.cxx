/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileInputDiscoveryStreamReader.hpp"

using namespace rti::routing;
using namespace rti::community::examples;

const std::string FileInputDiscoveryStreamReader::SQUARE_FILE_NAME = "samples_in_square.csv";
const std::string FileInputDiscoveryStreamReader::CIRCLE_FILE_NAME = "samples_in_circle.csv";
const std::string FileInputDiscoveryStreamReader::TRIANGLE_FILE_NAME = "samples_in_triangle.csv";

bool FileInputDiscoveryStreamReader::fexists(const std::string filename)
{
    std::ifstream input_file;
    input_file.open(filename);
    return input_file.is_open();
}

FileInputDiscoveryStreamReader::FileInputDiscoveryStreamReader(const PropertySet &)
{
    /**
     * In our example, we provide statically the stream information available. 
     * Make sure, the stream name and the stream type matches the <stream_name> 
     * and <registered_type_name> respectively in the XML configuration file.
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
}

void FileInputDiscoveryStreamReader::dispose()
{
    for (int i = 0; i < this->data_samples_.size(); i++) {
        this->data_samples_[i]->disposed(true);
    }
}

void FileInputDiscoveryStreamReader::take(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    stream.resize(this->data_samples_.size());
    for (int i = 0; i < this->data_samples_.size(); i++) {
        stream[i] = this->data_samples_[i];
    }
}

void FileInputDiscoveryStreamReader::return_loan(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    stream.clear();
}
