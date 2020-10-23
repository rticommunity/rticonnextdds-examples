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
    using bsoncxx::builder::stream::document;

    if (context.member_index == context.member_count + 1) {
        // end of members/items
        return;
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
        ArrayContext nested_context(
                member_info.element_count());
        build_document(nested_context, loaned_array.get());
        context.set_array(
                nested_context.doc_or_array,
                member_info);
    }
        break;

    case TypeKind::ARRAY_TYPE: {
        LoanedDynamicData loaned_array =
                data.loan_value(member_info.member_name());
        ArrayContext nested_context(
                member_info.element_count());

        //build_document(nested_context, loaned_array.get());

        context.set_array(
                nested_context.doc_or_array,
                member_info);
    }

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
