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

#ifndef MONGO_CONFIG_HPP
#define MONGO_CONFIG_HPP

#include <rti/routing/PropertySet.hpp>

namespace rti { namespace community { namespace examples {

/**
 * @brief Static class that represents a directory for all configuration
 * properties part of the MongoDB adapter.
 *
 * This class provides operations to obtain property names and their associated
 * value when they are present in a PropertySet.
 */

class MongoConfig {
public:
    enum property {
        /*
         * @brief Host address to a specific cluster or database.
         * Optional. Default: localhost:27017
         */
        CLUSTER_ADDRESS,
        /*
         * @brief Name of the database to connect and access.
         * Required.
         */
        DB_NAME,
        /**
         * @brief User name and passwored in format <user>:<password>
         * Required.
         */
        USER_AND_PASS,
        /**
         * @brief Additional DB connection options in MongoDB URI parameters
         * format. Optional. Default: retryWrites=true&w=majority
         */
        URI_PARAMS
    };

    template <MongoConfig::property Prop, typename Type = std::string>
    static Type parse(const rti::routing::PropertySet &properties);

    template <MongoConfig::property Prop>
    static const std::string &name();
};

}}}  // namespace rti::community::examples

#endif /* MONGO_CONFIG_HPP */
