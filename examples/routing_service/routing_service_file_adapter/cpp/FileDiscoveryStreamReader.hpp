/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#ifndef FILEDISCOVERYSTREAMREADER_HPP
#define FILEDISCOVERYSTREAMREADER_HPP

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/DiscoveryStreamReader.hpp>

namespace rti {
namespace community {
namespace examples {

using namespace rti::routing;
using namespace rti::routing::adapter;

// This class implements a DiscoveryStreamReader, a special kind of StreamReader
// that provide discovery information about the available streams and their
// types.
class FileDiscoveryStreamReader : public DiscoveryStreamReader {
public:
    FileDiscoveryStreamReader(const PropertySet &);

    void take(std::vector<rti::routing::StreamInfo*>&);

    void return_loan(std::vector<rti::routing::StreamInfo*>&);

private:
    std::vector<rti::routing::StreamInfo> data_samples_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif