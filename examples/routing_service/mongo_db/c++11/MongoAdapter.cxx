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
#include <memory>
#include <sstream>
#include <string_view> 
#include "MongoAdapter.hpp"
#include "MongoConnection.hpp"
#include <mongocxx/logger.hpp>
#include <rti/routing/Logger.hpp>

using namespace rti::community::examples;
using namespace rti::routing;
using namespace rti::routing::adapter;

/**
 * @brief Implementation of the MongoDB driver logger that redirects messages to
 * the Connext logger.
 */
class MongoLogger : public mongocxx::logger {
public:
    void operator()(
            mongocxx::log_level level,
            std::string_view domain,
            std::string_view message) noexcept override final
    {
        using mongocxx::log_level;
        std::stringstream ss;
        ss << domain << ':' << message;

        switch (level) {
        case log_level::k_debug:
        case log_level::k_trace:
            rti::routing::Logger::instance().debug(
                    ss.str());
            break;

        case log_level::k_info:
            rti::routing::Logger::instance().remote(
                    ss.str());
            break;

        case log_level::k_message:
            rti::routing::Logger::instance().local(
                    ss.str());
            break;

        case log_level::k_warning:
            rti::routing::Logger::instance().warn(
                    ss.str());
            break;

        default:
            rti::routing::Logger::instance().error(
                    ss.str());
            break;
        }
    }
};

MongoAdapter::MongoAdapter(PropertySet &)
        : instance_(std::unique_ptr<MongoLogger>())
{
}

Connection *MongoAdapter::create_connection(
        rti::routing::adapter::detail::StreamReaderListener *,
        rti::routing::adapter::detail::StreamReaderListener *,
        const PropertySet &properties)
{
    return new MongoConnection(properties);
}

void MongoAdapter::delete_connection(Connection *connection)
{
    delete connection;
}

rti::config::LibraryVersion MongoAdapter::get_version() const
{
    return { 1, 0, 0, 'r' };
};

RTI_ADAPTER_PLUGIN_CREATE_FUNCTION_DEF(MongoAdapter)
