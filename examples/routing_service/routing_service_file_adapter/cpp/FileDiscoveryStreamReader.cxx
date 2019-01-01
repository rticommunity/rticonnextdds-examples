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
    this->data_samples_.resize(1, StreamInfo("TestTopic", "TestType"));
}

void FileDiscoveryStreamReader::take(std::vector<rti::routing::StreamInfo*>& stream)
{
    std::cout << "Discovery take" << std::endl;
    stream.resize(this->data_samples_.size());
    stream[0] = &(this->data_samples_[0]);
}

void FileDiscoveryStreamReader::return_loan(std::vector<rti::routing::StreamInfo*>& stream)
{
    std::cout << "Discovery return" << std::endl;
    stream.clear();
}
