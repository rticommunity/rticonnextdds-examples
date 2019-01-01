/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#ifndef FILESTREAMREADER_HPP
#define FILESTREAMREADER_HPP

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamReader.hpp>

namespace rti {
namespace community {
namespace examples {

using namespace rti::routing;
using namespace rti::routing::adapter;

class FileDiscoveryStreamReader : public DiscoveryStreamReader {
public:
    FileDiscoveryStreamReader(const PropertySet &);

    void
            read(std::vector<dds::core::xtypes::DynamicData *> &,
                 std::vector<dds::sub::SampleInfo *> &);

    void
            read(std::vector<dds::core::xtypes::DynamicData *> &,
                 std::vector<dds::sub::SampleInfo *> &,
                 const SelectorState &selector_state);

    void
            take(std::vector<dds::core::xtypes::DynamicData *> &,
                 std::vector<dds::sub::SampleInfo *> &);

    void
            take(std::vector<dds::core::xtypes::DynamicData *> &,
                 std::vector<dds::sub::SampleInfo *> &,
                 const SelectorState &selector_state);

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData *> &,
            std::vector<dds::sub::SampleInfo *> &);

};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif