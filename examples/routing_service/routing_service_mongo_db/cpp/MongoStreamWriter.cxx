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


#include <rti/core/Exception.hpp>
#include <rti/topic/to_string.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "MongoStreamWriter.hpp"
#include "SampleConverter.hpp"

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace rti::community::examples;
using namespace bsoncxx;

MongoStreamWriter::MongoStreamWriter(
        MongoConnection& connection,
        const StreamInfo& stream_info,
        const std::string& db_name,
        const PropertySet &)
        : connection_(connection),
        db_name_(db_name),
        stream_name_(stream_info.stream_name())
{
   
};

MongoStreamWriter::~MongoStreamWriter()
{

};

mongocxx::database MongoStreamWriter::database()
{
    auto client = connection_.client_pool_.acquire();

    return client->database(db_name_);
}

/*
 * --- Private Interface ------------------------------------------------------
 */

/*
 * --- Adapter Interface ------------------------------------------------------
 */

int MongoStreamWriter::write(
        const std::vector<dds::core::xtypes::DynamicData *>& samples,
        const std::vector<dds::sub::SampleInfo *>& infos)
{
    /*
     * This is a blocking operation: calling database() will query the
     * mongo pool and wait until a client is available.
     * We will keep the obtained client locked to write all the sample sequence
     */
    mongocxx::collection db_collection = database()[stream_name_];

    for (uint32_t i = 0; i < samples.size(); i++) {

        builder::stream::document document_sample{};
        document_sample
                << "data"
                << types::b_document{
                    SampleConverter::to_document(*samples[i])};
        if (infos[i] != NULL) {
            document_sample
                    << "info"
                    << types::b_document{
                        SampleConverter::to_document(*infos[i])};
        }

        db_collection.insert_one(
                document_sample << builder::stream::finalize);
    }
            
    return samples.size();
}


