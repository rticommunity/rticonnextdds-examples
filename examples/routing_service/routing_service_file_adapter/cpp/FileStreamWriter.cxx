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

const std::string FileStreamWriter::OUTPUT_FILE_PROPERTY_NAME = "example.adapter.output_file";

FileStreamWriter::FileStreamWriter(const PropertySet &property)
{

    const auto &it = property.find(OUTPUT_FILE_PROPERTY_NAME);
    if ( it != property.end()) {
        std::string outfilepath = it->second;
        std::cout << "FilePath: " << outfilepath << std::endl;
        outputfile_.open(outfilepath);
    }

    if (!outputfile_.is_open()) {
        throw std::logic_error("Open output file");
    }

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
        outputfile_ << str << std::endl;
    }
    return 0;
}