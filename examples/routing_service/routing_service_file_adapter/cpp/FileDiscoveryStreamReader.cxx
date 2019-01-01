/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileDiscoveryStreamReader.hpp"

using namespace rti::community::examples;

FileDiscoveryStreamReader::FileDiscoveryStreamReader(const PropertySet &)
{
    // TODO:: Implement me
    std::cout << "Create Discovery Stream Reader" << std::endl;
}

void FileDiscoveryStreamReader::take(std::vector<rti::routing::StreamInfo*>& stream)
{
    std::cout << "Discovery take" << std::endl;
    stream.resize(1);
    stream[0] = new rti::routing::StreamInfo("TestTopic","TestType");
}

void FileDiscoveryStreamReader::return_loan(std::vector<rti::routing::StreamInfo*>& stream)
{
    std::cout << "Discovery return" << std::endl;
}
