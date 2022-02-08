/*
 *  (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */
#include <mongocxx/uri.hpp>

#include "MongoConfig.hpp"

using namespace rti::community::examples;

/*
 *  --- Cluster address
 * -----------------------------------------------------------------
 */
template <>
const std::string &MongoConfig::name<MongoConfig::CLUSTER_ADDRESS>()
{
    static std::string __name("mongo.cluster_address");
    return __name;
}

template <>
std::string MongoConfig::parse<MongoConfig::CLUSTER_ADDRESS>(
        const rti::routing::PropertySet &properties)
{
    rti::routing::PropertySet::const_iterator it =
            properties.find(MongoConfig::name<MongoConfig::CLUSTER_ADDRESS>());
    if (it != properties.end()) {
        return it->second;
    }

    return "localhost:27017";
}

/*
 *  --- User & Pass
 * ---------------------------------------------------------------------
 */
template <>
const std::string &MongoConfig::name<MongoConfig::USER_AND_PASS>()
{
    static std::string __name("mongo.uri_params");
    return __name;
}

template <>
std::string MongoConfig::parse<MongoConfig::USER_AND_PASS>(
        const rti::routing::PropertySet &properties)
{
    rti::routing::PropertySet::const_iterator it =
            properties.find(MongoConfig::name<MongoConfig::USER_AND_PASS>());
    if (it != properties.end()) {
        return it->second;
    }

    return "rti_example:adapter";
}

/*
 *  ---  Uri params
 * ---------------------------------------------------------------------
 */
template <>
const std::string &MongoConfig::name<MongoConfig::URI_PARAMS>()
{
    static std::string __name("mongo.uri_params");
    return __name;
}

template <>
std::string MongoConfig::parse<MongoConfig::URI_PARAMS>(
        const rti::routing::PropertySet &properties)
{
    rti::routing::PropertySet::const_iterator it =
            properties.find(MongoConfig::name<MongoConfig::URI_PARAMS>());
    if (it != properties.end()) {
        return it->second;
    }

    return "retryWrites=true&w=majority";
}

/*
 *  --- Database name
 * -------------------------------------------------------------------
 */
template <>
const std::string &MongoConfig::name<MongoConfig::DB_NAME>()
{
    static std::string __name("mongo.db_name");
    return __name;
}

template <>
std::string MongoConfig::parse<MongoConfig::DB_NAME>(
        const rti::routing::PropertySet &properties)
{
    rti::routing::PropertySet::const_iterator it =
            properties.find(MongoConfig::name<MongoConfig::DB_NAME>());
    if (it == properties.end()) {
        throw dds::core::InvalidArgumentError(
                "database name is mandatory (property name: "
                + MongoConfig::name<MongoConfig::DB_NAME>());
    }

    return it->second;
}
