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

#ifndef FILEADAPTER_HPP
#define FILEADAPTER_HPP

#include <dds/core/corefwd.hpp>
#include <dds/core/macros.hpp>
#include <rti/routing/PropertySet.hpp>
#include <rti/routing/RoutingService.hpp>
#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <mongocxx/instance.hpp>

namespace rti { namespace community { namespace examples {

/**
 * @brief Implementation of the AdapterPlugin plugin entry point to provide access
 * to MongoDB using the mongocxx driver.
 *
 * The implementation represents a holder to the mongocxx::instance, which is a singleton
 * that shall be instantiated before performing any other operation on the driver. This
 * implies that in a given Routing Service, only one instance of this plugin can be
 * loaded, that is, only one <plugin> entry is allowed (or one call to
 * rti::routing::Service::register_plugin() when embedding the service into your
 * application).
 *
 * Other than that, this class purely implements the AdapterPlugin interface behavior
 * to create and delete connections. There are currently no configuration properties for
 * this class.
 *
 */
class MongoAdapter : public rti::routing::adapter::AdapterPlugin {
public:

    explicit MongoAdapter(rti::routing::PropertySet &);

    /*
     * --- AdapterPlugin interface ------------------------------------------------------
     */
    rti::routing::adapter::Connection *create_connection(
            rti::routing::adapter::detail::StreamReaderListener *,
            rti::routing::adapter::detail::StreamReaderListener *,
            const rti::routing::PropertySet &) final;

    void delete_connection(rti::routing::adapter::Connection *connection) override final;

    rti::config::LibraryVersion get_version() const override final;

private:
    mongocxx::instance instance_;
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
 * MongoAdapterPlugin_create_adapter_plugin
 * \endcode
 */
RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DECL(MongoAdapter)

#endif
