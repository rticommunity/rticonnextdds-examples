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

#ifndef MONGO_CONNECTION_HPP
#define MONGO_CONNECTION_HPP

#include <dds/core/macros.hpp>
#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/Connection.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/database.hpp>

#include "MongoConfig.hpp"

namespace rti { namespace community { namespace examples {

class MongoStreamWriter;

/***
 * @brief Implementation of the adapter Connection. Represents a pool of client
 * connections to MongoDB.
 *
 * This is a key class since it establishes the actual connection to a specific MongoDB
 * database, given a user-provided URI.
 *
 * It uses a pool of clients to read and write from the DB. This is required to support
 * concurrency between the created StreamReaders and StreamWriters, that can operate
 * under multiple threads–either within the same Session's thread pool or separate
 * Sessions-.
 *
 * The current strategy is to have StreamReaders and StreamWriters to ask for an available
 * client database connection for the current calling context, using the database()
 * operation.
 *
 * The MongoConnection can receive the following configuration properties:
 *
 *  - MongoConfig::CLUSTER_ADDRESS
 *  - MongoConfig::DB_NAME
 *  - MongoConfig::USER_AND_PASS
 *  - MongoConfig::URI_PARAMS
 *
 *  @see MongoConfig
 */
class MongoConnection : public rti::routing::adapter::Connection {
public:

    /**
     * @brief Creates the connection given its configuration properites
     * @param properties
     *      Configuration properites provided by the <property> tag within
     *      <connection>.
     *
     */
    MongoConnection(const rti::routing::PropertySet &properties);

    /*
     * --- Connection interface ---------------------------------------------------------
     */
    rti::routing::adapter::StreamWriter *create_stream_writer(
            rti::routing::adapter::Session *session,
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &properties) override final;

    void delete_stream_writer(rti::routing::adapter::StreamWriter *writer) override final;


    rti::routing::adapter::StreamReader *create_stream_reader(
            rti::routing::adapter::Session *session,
            const rti::routing::StreamInfo &info,
            const rti::routing::PropertySet &properties,
            rti::routing::adapter::StreamReaderListener *listener) override final;

    void delete_stream_reader(rti::routing::adapter::StreamReader *reader) override final;
    /*
     * --- Private Interface ------------------------------------------------------------
     */
    const std::string& db_name() const;

    mongocxx::pool::entry client();

private:
    friend MongoStreamWriter;

private:
    mongocxx::pool client_pool_;
    std::string db_name_;
};

}  // namespace examples
}  // namespace community
}  // namespace rti

#endif
