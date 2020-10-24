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

#include <dds/core/xtypes/CollectionTypes.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include "DynamicDataConverter.hpp"

using namespace dds::core::xtypes;
using namespace bsoncxx;
using namespace rti::community::examples;


DynamicDataConverter::DynamicDataConverter()
{
}

void build_document(
        Context& context,
        DynamicData &data)
{
    using rti::core::xtypes::LoanedDynamicData;
    using rti::core::xtypes::DynamicDataMemberInfo;
    using dds::core::xtypes::ArrayType;
    using bsoncxx::builder::stream::document;

    if (context.member_index == context.member_count + 1) {
        // end of members/items
        return;
    }

    // Handle sub-array state
    if (typeid(context) == typeid(ArrayContext)) {
        ArrayContext& as_array_context =
                static_cast<ArrayContext&>(context);
        if (as_array_context.dimension_index <
                as_array_context.dimensions.size()) {
            uint32_t current_dimension = as_array_context.dimension_index;
            // Arrays can have too many elements for a recursive approach, hence
            // we iterate on the elements to avoid stack overflow
            for (uint32_t i = 0;
                 i < as_array_context.dimensions[current_dimension];
                 ++i) {
                if (as_array_context.dimension_index
                        < as_array_context.dimensions.size() - 1) {
                    ArrayContext nested_context(
                            as_array_context.element_index,
                            as_array_context.dimensions,
                            as_array_context.dimension_index + 1,
                            as_array_context.element_index);
                    build_document(nested_context, data);
                    context.set_array(
                            nested_context.doc_or_array,
                            data.member_info(as_array_context.element_index));
                    // advance the element iterator index
                    as_array_context.element_index = nested_context.element_index;
                } else {
                    // mark the end of dimensions
                    ++as_array_context.dimension_index;
                    // Set member_count to allow only one recursion
                    context.member_count = as_array_context.element_index;
                    context.member_index = as_array_context.element_index;
                    build_document(context, data);
                    ++as_array_context.element_index;
                }
            }
            // end of elements, at this point we have reached the maximum
            // at the current dimension
            return;
        }
    }

    if (!data.member_exists(context.member_index)) {
        ++context.member_index;
        return build_document(context,data);
    }

    DynamicDataMemberInfo member_info =
            data.member_info(context.member_index);
    switch (member_info.member_kind().underlying()) {
    case TypeKind::STRUCTURE_TYPE: {

        LoanedDynamicData loaned_member =
                data.loan_value(member_info.member_index());
        DocumentContext nested_context(
                loaned_member.get().member_count());
        build_document(nested_context,loaned_member.get());
        context.set_document(
                nested_context.doc_or_array,
                member_info);

    }
        break;

    case TypeKind::SEQUENCE_TYPE: {

        LoanedDynamicData loaned_array =
                data.loan_value(member_info.member_name());
        // Sequences can have too many elements for a recursive approach, hence
        // we iterate on the elements to avoid stack overflow
        ArrayContext nested_context(1);
        for (uint32_t i = 0; i < member_info.element_count(); ++i) {
            // Set member_count to allow only one recursion
            nested_context.member_count = i;
            build_document(nested_context, loaned_array.get());
        }

        context.set_array(
                nested_context.doc_or_array,
                member_info);
    }
        break;

    case TypeKind::ARRAY_TYPE: {
        LoanedDynamicData loaned_array =
                data.loan_value(member_info.member_name());
        // Get dimensions as an array
        const ArrayType& array_type =
                static_cast<const ArrayType &> (loaned_array.get().type());
        std::vector<uint32_t> dimensions;
        dimensions.resize(array_type.dimension_count());
        for (uint32_t j = 0; j < array_type.dimension_count(); j++) {
            dimensions[j] = array_type.dimension(j);
        }

        ArrayContext nested_context(1, dimensions);
        build_document(nested_context, loaned_array.get());
        context.set_array(
                nested_context.doc_or_array,
                member_info);
    }

        break;

    default:
        context.set_primitive(data, member_info);
    }

    ++context.member_index;
    build_document(context, data);
}


bsoncxx::document::value DynamicDataConverter::to_document(
        const dds::core::xtypes::DynamicData& data)
{
    dds::core::xtypes::DynamicData& casted_data =
            const_cast<dds::core::xtypes::DynamicData&>(data);
    DocumentContext context(data.member_count());
    build_document(
            context,
            casted_data);

    return (context.doc_or_array << builder::stream::finalize);
}
