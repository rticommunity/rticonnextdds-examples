/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef RS_ADAPTER_PLUGIN_HPP_
#define RS_ADAPTER_PLUGIN_HPP_

#include <rti/routing/PropertySet.hpp>
#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/Connection.hpp>

class RsAdapterPlugin : public rti::routing::adapter::AdapterPlugin {
public:
    RsAdapterPlugin(const rti::routing::PropertySet &properties);
    virtual ~RsAdapterPlugin();

    auto create_connection(
            rti::routing::adapter::StreamReaderListener
                    *input_stream_discovery_listener,
            rti::routing::adapter::StreamReaderListener
                    *output_stream_discovery_listener,
            const rti::routing::PropertySet &properties)
            -> rti::routing::adapter::Connection *;

    void delete_connection(rti::routing::adapter::Connection *connection);

private:
    /* List of connections created by adapter */
    std::list<rti::routing::adapter::Connection *> connections_;
};

/* declare entry point class */
RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DECL(RsAdapterPlugin)

#endif /* RSL_ADAPTER_PLUGIN_HPP_ */
