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

#ifndef DYNAMICDATACONVERTER_HPP
#define DYNAMICDATACONVERTER_HPP

#include <unordered_map>
#include <vector>

#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/sub/SampleInfo.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

namespace rti { namespace community { namespace examples {

/**
 * @brief Helper to convert data from the DDS space to the MongoDB space.
 *
 */
class SampleConverter {
public:

    /**
     * @brief Converts a DynamicData object into a BSON document.
     *
     * The general mapping is:
     * - Structure -> bsoncxx::document
     * - Sequences and multi-dimensional arrays -> bsoncxx::array
     * - String -> bsoncxx::utf8
     * - primitive types: A corresponding bson primitive, except:
     *   + uint32_t -> bsoncxx::b_int64
     *   + enum -> bsoncxx::b_int64
     *
     * Unions and wide strings are not currently supported.
     */
    static bsoncxx::document::value to_document(
            const dds::core::xtypes::DynamicData& data);

    /**
     * @brief Converts a SampleInfo object into a BSON document
     *
     * The general mapping is:
     * - InstanceHandle -> bsoncxx::binary (md5)
     * - GUID -> bsoncxx::binary (md5)
     * - Sequence Number -> {high:int32, low:int64}
     * - Time_t -> Date
     *
     * Only a subset of fields are currently converted.
     */
    static bsoncxx::document::value to_document(
            const dds::sub::SampleInfo& info);

    /**
     * @brief Converts a bson Document into a DynamicData object.
     *
     * It expected that the DynamiData object has a type compatible with the Document,
     * that is, it's either a subset or superset of the document.
     *
     * This operation attempts to set members existing in the input document into
     * members in DynamicData that are present.
     *
     * @param data destination typed DynamicData
     * @param document input document
     */
     static dds::core::xtypes::DynamicData & from_document(
             dds::core::xtypes::DynamicData& data,
             bsoncxx::document::view document);

private:
    SampleConverter();
};


}  // namespace examples
}  // namespace community
}  // namespace rti

#endif /* DYNAMICDATACONVERTER_HPP */

