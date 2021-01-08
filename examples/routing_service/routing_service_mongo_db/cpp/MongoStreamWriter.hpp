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

#ifndef MONGO_STREAMWRITER_HPP
#define MONGO_STREAMWRITER_HPP

#include <iostream>

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamWriter.hpp>

#include "MongoConnection.hpp"

namespace rti { namespace community { namespace examples {

/**
 * @brief Implementation of the adapter StreamWriter to write DDS samples as
 * MongoDB objects.
 *
 * This class is responsible for inserting samples into a MongoDB database as
 * as part of a collection identified by the associated stream name. This class
 * is responsible for converting a stream sample (represented as a pair
 * [DynamicData,SampleInfo]) into a mongocxx::document.
 *
 * To perform database insertions, this class obtains a client handle through
 * the parent factory MongoConnection, which is provided on object construction.
 */
class MongoStreamWriter
        : public rti::routing::adapter::DynamicDataStreamWriter {
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
    MongoStreamWriter(
            MongoConnection &connection,
            const rti::routing::StreamInfo &stream_info,
            const rti::routing::PropertySet &properties);

    /*
     * --- StreamWriter interface
     * ---------------------------------------------------------
     */
    /**
     * @brief Performs the insertion of DDS samples into MongoDB.
     *
     * For each sample DynamicData::SampleInfo, a new document object is added
     * to the collection whose name is the name of the associated stream. Each
     * sample _i_ is represented as:
     *
     * ```
     *     {
     *          data: bson{samples[i]}
     *          info: bson{infos[i]}
     *     }
     * ```
     *
     * where _bson{samples[i]}_ is the BSON representation of a DDS data item
     * and _bson_{infos[i]} is the BSON representation of a DDS info item.
     *
     * @see SampleConverter
     *
     */
    int write(
            const std::vector<dds::core::xtypes::DynamicData *> &samples,
            const std::vector<dds::sub::SampleInfo *> &infos) override final;

private:
    MongoConnection &connection_;
    std::string stream_name_;
};

}}}  // namespace rti::community::examples

#endif
