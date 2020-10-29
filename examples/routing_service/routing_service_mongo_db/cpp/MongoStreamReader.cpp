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

#include <mongocxx/collection.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "SampleConverter.hpp"
#include "MongoStreamReader.hpp"

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::community::examples;
using namespace bsoncxx;

document::value MongoStreamReader::find_filter()
{
    types::b_date current_date{std::chrono::system_clock::now()};
    builder::stream::document find_exp{};
    find_exp << "info.reception_timestamp"
             << builder::stream::open_document
             << "$gte" << last_read_
             << "$lt" << current_date
             << builder::stream::close_document;
    last_read_ = current_date;

    return (find_exp << builder::stream::finalize);
}


mongocxx::options::find MongoStreamReader::find_options()
{
    static mongocxx::options::find cursor_options{};
    static bool inited = false;
    if (!inited) {
        cursor_options
            .max_time(std::chrono::milliseconds(50000));
        inited = true;
    }

    return cursor_options;
}

MongoStreamReader::MongoStreamReader(
        MongoConnection& connection,
        const rti::routing::StreamInfo& stream_info,
        const rti::routing::PropertySet &properties)
        : connection_(connection),
          stream_name_(stream_info.stream_name()),
          last_read_(std::chrono::system_clock::now()),
          type_(stream_info.type_info().dynamic_type())
{

}

/*
 * --- Adapter Interface ------------------------------------------------------
 */

void MongoStreamReader::take(
        std::vector<dds::core::xtypes::DynamicData *>& samples,
        std::vector<dds::sub::SampleInfo *>& infos)
{
    auto client = connection_.client();
    mongocxx::database database = client->database(connection_.db_name());

    auto cursor = database.collection(stream_name_)
            .find(find_filter(), MongoStreamReader::find_options());

    for (const bsoncxx::document::view& document : cursor) {
        samples.push_back(new dds::core::xtypes::DynamicData(type_));
        SampleConverter::from_document(
                *samples.back(),
                document["data"].get_document().view());

    }
}

void MongoStreamReader::return_loan(
        std::vector<dds::core::xtypes::DynamicData *>& samples,
        std::vector<dds::sub::SampleInfo *>& infos)
{
    for (uint32_t i = 0; i < samples.size(); ++i) {
        delete samples[i];
        if (i < infos.size()) {
            delete infos[i];
        }
    }
    samples.resize(0);
    infos.resize(0);
}

