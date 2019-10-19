/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileDiscoveryStreamReader.hpp"

using namespace rti::routing;
using namespace rti::community::examples;

FileDiscoveryStreamReader::FileDiscoveryStreamReader(const PropertySet &)
{
    /**
     * In our example, we provide statically the stream information available. 
     * Make sure, the stream name and the stream type matches the <stream_name> 
     * and <registered_type_name> respectively in the XML configuration file.
     */
    this->data_samples_.push_back(new StreamInfo("Square", "ShapeType"));
}

void FileDiscoveryStreamReader::take(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    stream.resize(this->data_samples_.size());
    for (int i = 0; i < this->data_samples_.size(); i++) {
        stream[i] = this->data_samples_[i];
    }
}

void FileDiscoveryStreamReader::return_loan(
        std::vector<rti::routing::StreamInfo *> &stream)
{
    stream.clear();
}
