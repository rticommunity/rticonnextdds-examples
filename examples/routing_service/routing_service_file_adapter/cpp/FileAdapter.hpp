/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#ifndef FILEADAPTER_HPP
#define FILEADAPTER_HPP

#include <dds/core/corefwd.hpp>
#include <dds/core/macros.hpp>
#include <rti/routing/PropertySet.hpp>
#include <rti/routing/RoutingService.hpp>
#include <rti/routing/adapter/AdapterPlugin.hpp>

// TODO: Documentation
using namespace rti::routing;
using namespace rti::routing::adapter;

namespace rti {
namespace community {
namespace examples {
class FileAdapter : public rti::routing::adapter::AdapterPlugin {
public:
    FileAdapter(rti::routing::PropertySet &){};

    Connection *create_connection(
            rti::routing::adapter::detail::StreamReaderListener *,
            rti::routing::adapter::detail::StreamReaderListener *,
            const PropertySet &);

    void delete_connection(Connection *connection){};

    rti::config::LibraryVersion get_version() const;
};


}  // namespace examples
}  // namespace community
}  // namespace rti

// TODO: Document the create function
RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DECL(FileAdapter)

#endif