/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

#include "FileStreamWriter.hpp"
#include <rti/topic/to_string.hpp>


using namespace rti::community::examples;

FileStreamWriter::FileStreamWriter(const PropertySet &)
{
        //TODO: Implement
};

int FileStreamWriter::write(
        const std::vector<dds::core::xtypes::DynamicData *> &samples,
        const std::vector<dds::sub::SampleInfo *> &infos)
{
    for (std::vector<dds::core::xtypes::DynamicData *>::const_iterator it
         = samples.begin();
         it != samples.end();
         ++it) {
        std::string str = rti::topic::to_string(
                (**it), rti::topic::PrintFormatProperty::Json());
        std::cout << "Received Sample (JSON):" << std::endl << str << std::endl;
    }
    return 0;
}