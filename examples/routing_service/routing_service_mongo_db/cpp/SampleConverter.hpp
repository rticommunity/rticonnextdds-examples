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
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

namespace rti {
namespace community {
namespace examples {


class DynamicDataConverter {
public:

    static bsoncxx::document::value to_document(
            const dds::core::xtypes::DynamicData& data);

private:
    DynamicDataConverter();
};


class SampleInfoConverter {

};

} } }

#endif /* DYNAMICDATACONVERTER_HPP */

