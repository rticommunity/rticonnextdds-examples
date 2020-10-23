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

class MongoConfig {
public:

    enum property {
        URI,
        DB_NAME
    };

    template <MongoConfig::property Prop, typename Type=std::string>
    static Type parse(const rti::routing::PropertySet& properties);

    template <MongoConfig::property Prop>
    static const std::string& name();

};

} } }

#endif /* MONGO_CONFIG_HPP */

