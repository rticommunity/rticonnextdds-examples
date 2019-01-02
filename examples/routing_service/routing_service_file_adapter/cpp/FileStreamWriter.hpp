/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#ifndef FILESTREAMWRITER_HPP
#define FILESTREAMWRITER_HPP

#include <iostream>
#include <fstream>

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamWriter.hpp>

namespace rti {
namespace community {
namespace examples {

using namespace rti::routing;
using namespace rti::routing::adapter;

class FileStreamWriter : public DynamicDataStreamWriter {
public:
    FileStreamWriter(const PropertySet &);

    int
            write(const std::vector<dds::core::xtypes::DynamicData *> &samples,
                  const std::vector<dds::sub::SampleInfo *> &infos);

    ~FileStreamWriter() {
        outputfile_.close();
    };

private:

    static const std::string OUTPUT_FILE_PROPERTY_NAME;

    std::ofstream outputfile_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif