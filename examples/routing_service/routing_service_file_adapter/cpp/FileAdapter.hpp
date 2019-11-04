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

namespace rti { namespace community { namespace examples {
class FileAdapter : public rti::routing::adapter::AdapterPlugin {
public:
    FileAdapter(rti::routing::PropertySet &){};

    rti::routing::adapter::Connection *create_connection(
            rti::routing::adapter::detail::StreamReaderListener *,
            rti::routing::adapter::detail::StreamReaderListener *,
            const rti::routing::PropertySet &) final;

    void delete_connection(rti::routing::adapter::Connection *connection) final;

    rti::config::LibraryVersion get_version() const;
};


}  // namespace examples
}  // namespace community
}  // namespace rti

/**
 * This macro defines a C-linkage symbol that can be used as create function
 * for plug-in registration through XML.
 *
 * The generated symbol has the name:
 *
 * \code
 * FileAdapterPlugin_create_adapter_plugin
 * \endcode
 */
RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DECL(FileAdapter)

#endif
