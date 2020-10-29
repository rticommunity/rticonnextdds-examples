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

/**
 * @brief Implementation of the adapter Stream to read DDS from MongoDB document
 *        objects.
 *
 * This class is responsible for retrieving stream samples from MongoDB database as
 * as part of a collection identified by the associated stream name. This class
 * is responsible for converting a sample from a mongocxx:document that has a structure
 * {data: {}, info: {}} into a stream smpale (represented as a pair [DynamicData,SampleInfo]).
 *
 * The samples are read from the database where the parent Connection is connected
 * and from the collection identified by the stream name.
 *
 * The set of samples is defined by all the samples whose *reception timestamp* is
 * between [t1,t2], where:
 * - t1: time from the last read. Initially this is set to the StreamReader creation
 *    time.
 * - t2: current time, that is, the time at which the read operation is called.
 *
 * To perform database reads, this class obtains client handle through
 * the parent factory MongoConnection, which is provided on object construction, an uses
 * a cursor with a filter to satisfy the time condition above.
 *
 */
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

    /*
     * --- StreamREader interface ---------------------------------------------------------
     */
    /**
     * @brief Reads a set of samples from MongoDB.
     *
     *
     * The provided list of samples are the result of converting the MongoDB documents
     * into dynamic data. The conversion process is best-effort. It attempts to set
     * each document element into a DynamicData member:
     *
     *  - If a document element is not part of the DynamicData member, it will be ignored
     *    (a debug log is emitted)
     *  - If a document is missing elements that are present in the DynamicData object,
     *    the latter will be left to their initialization default (typically zero-ed
     *    memory).
     *
     * Members are populated based on the current context. The document::data
     * object is the top-level context. From there:
     * 1. A primitive element is set to the equivalent DynamicData primitive member
     *   with the same name.
     * 2. A document element is set to the equivalent DynamicData complex member with
     *   the same name. A new context is created and repeats the process starting at 1.
     * 3. An array element is set to the equivalent DynamicData array member with
     *   the same name. A new context is created and repeats the process starting at 1.
     *   *for each element in the array*.
     */
    void take(
            std::vector<dds::core::xtypes::DynamicData *>& samples,
            std::vector<dds::sub::SampleInfo *>& infos) override final;

    /**
     * @brief Returns the memory used to generate the output samples in take().
     */
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
