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

namespace rti { namespace community { namespace examples {

class FileStreamWriter : 
        public rti::routing::adapter::DynamicDataStreamWriter {
public:
    FileStreamWriter(const rti::routing::PropertySet &);

    int write(
            const std::vector<dds::core::xtypes::DynamicData *> &samples,
            const std::vector<dds::sub::SampleInfo *> &infos);

    ~FileStreamWriter();

private:

    static const std::string OUTPUT_FILE_PROPERTY_NAME;
    std::ofstream output_file_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif
