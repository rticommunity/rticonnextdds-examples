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
#ifndef MONGO_MONGOSTREAMREADER_HPP
#define MONGO_MONGOSTREAMREADER_HPP


#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamReader.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>

#include "MongoConnection.hpp"

namespace rti { namespace community { namespace examples {

class MongoStreamReader : public rti::routing::adapter::DynamicDataStreamReader {

public:
    /**
     * @brief Creates the StreamWriter from the required parameters.
     *
     * @param connection
     *      The parent factory connection
     * @param stream_info
     *      Information for the associated Stream, provided by Routing Service.
     * @param properties
     *      Configuration properites provided by the <property> tag within
     *      <output>.
     */
    MongoStreamReader(
            MongoConnection& connection,
            const rti::routing::StreamInfo& stream_info,
            const rti::routing::PropertySet &properties);

    void take(
            std::vector<dds::core::xtypes::DynamicData *>& samples,
            std::vector<dds::sub::SampleInfo *>& infos) override final;

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData *> & samples,
            std::vector<dds::sub::SampleInfo *> &) override final;

private:
    bsoncxx::document::value find_filter();
    static mongocxx::options::find find_options();

    MongoConnection& connection_;
    std::string stream_name_;
    bsoncxx::types::b_date last_read_;
    dds::core::xtypes::DynamicType type_;
};

}}}  // namespace rti::community::examples

#endif //MONGO_MONGOSTREAMREADER_HPP
