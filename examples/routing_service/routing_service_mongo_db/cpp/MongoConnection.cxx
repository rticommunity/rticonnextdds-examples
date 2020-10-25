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

#include "MongoConnection.hpp"
#include "MongoStreamWriter.hpp"

using namespace rti::community::examples;
using namespace rti::routing;
using namespace rti::routing::adapter;


/*
 * --- MongoConnection -------------------------------------------------------
 */

std::string build_uri(const PropertySet &properties)
{
    std::string cluster_addr = MongoConfig::parse<MongoConfig::CLUSTER_ADDRESS>(properties);
    std::string user_and_pass = MongoConfig::parse<MongoConfig::USER_AND_PASS>(properties);
    std::string uri_params = MongoConfig::parse<MongoConfig::URI_PARAMS>(properties);

    return "mongodb+srv://"
            + user_and_pass
            + "@" + cluster_addr
            + "/<database>?"
            + uri_params;
}

MongoConnection::MongoConnection(
        const PropertySet &properties)
        : client_pool_(mongocxx::uri(build_uri(properties))),
          db_name_(MongoConfig::parse<MongoConfig::DB_NAME>(properties))
{
}
                
/* --- Private interface ---*/

const std::string& MongoConnection::db_name() const
{
    return db_name_;
}

mongocxx::pool::entry MongoConnection::client()
{
    return client_pool_.acquire();
}

/* --- Adapter Interface --- */


StreamWriter *MongoConnection::create_stream_writer(
        Session *session,
        const StreamInfo &stream_info,
        const PropertySet &properties)
{
    return new MongoStreamWriter(
            *this,
            stream_info,
            properties);
};

void MongoConnection::delete_stream_writer(StreamWriter *writer)
{
    delete writer;
}