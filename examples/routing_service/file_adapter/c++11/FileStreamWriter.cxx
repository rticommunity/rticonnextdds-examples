/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include "FileStreamWriter.hpp"
#include <rti/core/Exception.hpp>
#include <rti/topic/to_string.hpp>

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::community::examples;

const std::string FileStreamWriter::OUTPUT_FILE_PROPERTY_NAME =
        "example.adapter.output_file";

FileStreamWriter::FileStreamWriter(const PropertySet &properties)
{
    std::string output_file_name;
    for (const auto &property : properties) {
        if (property.first == OUTPUT_FILE_PROPERTY_NAME) {
            output_file_name = property.second;
            output_file_.open(output_file_name);
            break;
        }
    }

    if (!output_file_.is_open()) {
        throw dds::core::IllegalOperationError(
                "Error opening output file: " + output_file_name);
    } else {
        std::cout << "Output file name: " << output_file_name << std::endl;
    }
}

int FileStreamWriter::write(
        const std::vector<dds::core::xtypes::DynamicData *> &samples,
        const std::vector<dds::sub::SampleInfo *> &infos)
{
    for (auto sample : samples) {
        std::cout << "Received Sample: " << std::endl
                  << rti::topic::to_string(*sample) << std::endl;

        output_file_ << sample->value<std::string>("color") << ","
                     << sample->value<int32_t>("x") << ","
                     << sample->value<int32_t>("y") << ","
                     << sample->value<int32_t>("shapesize") << std::endl;
    }
    return 0;
}

FileStreamWriter::~FileStreamWriter()
{
    output_file_.close();
}
