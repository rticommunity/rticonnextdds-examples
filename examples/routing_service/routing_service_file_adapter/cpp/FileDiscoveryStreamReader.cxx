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
}

void FileDiscoveryStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    // TODO: Implement me
    return;
}


void FileDiscoveryStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    // TODO: Implement me
    return;
}

void FileDiscoveryStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    // TODO: Implement me
    return;
}

void FileDiscoveryStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    // TODO: Implement me
    return;
}

void FileDiscoveryStreamReader::return_loan(
        std::vector<dds::core::xtypes::DynamicData *> &,
        std::vector<dds::sub::SampleInfo *> &)
{
    // TODO: Implement me
}
