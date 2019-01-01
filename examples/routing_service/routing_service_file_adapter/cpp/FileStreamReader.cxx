/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileStreamReader.hpp"

using namespace rti::community::examples;

FileStreamReader::FileStreamReader(const PropertySet &)
{
    // TODO:: Implement me
}

void FileStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    // TODO: Implement me
    return;
}


void FileStreamReader::read(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    // TODO: Implement me
    return;
}

void FileStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos)
{
    // TODO: Implement me
    return;
}

void FileStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *> &samples,
        std::vector<dds::sub::SampleInfo *> &infos,
        const SelectorState &selector_state)
{
    // TODO: Implement me
    return;
}

void FileStreamReader::return_loan(
        std::vector<dds::core::xtypes::DynamicData *> &,
        std::vector<dds::sub::SampleInfo *> &)
{
    // TODO: Implement me
}

void *FileStreamReader::create_content_query(void *, const dds::topic::Filter &)
{
    // TODO: Implement me
    return NULL;
}

void FileStreamReader::delete_content_query(void *)
{
    // TODO: Implement me
}