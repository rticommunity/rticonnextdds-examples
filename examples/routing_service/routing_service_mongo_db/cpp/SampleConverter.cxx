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

#include "SampleConverter.hpp"

using namespace dds::core::xtypes;
using namespace bsoncxx;
using namespace rti::community::examples;


/*
 * -- From DDS to MongoDB conversion ----------------------------------------------------
 */


/* --- DynamicData --------------------------------------------------------------------*/

template<typename Primitive, typename DocOrArray>
struct primitive_setter;

template<typename Primitive>
struct primitive_setter<Primitive, bsoncxx::builder::stream::document> {

    static void set(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::document& document)
    {

        document << member_info.member_name()
                 << data.value<Primitive>(member_info.member_index());
    }

};


template<typename Primitive>
struct primitive_setter<Primitive, bsoncxx::builder::stream::array> {

    static void set(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::array& array)
    {
        array << data.value<Primitive>(member_info.member_index());
    }
};

template<typename Primitive, typename DocOrArray = bsoncxx::builder::stream::document>
void from_primitive(
        const dds::core::xtypes::DynamicData& data,
        const rti::core::xtypes::DynamicDataMemberInfo& member_info,
        DocOrArray& doc_or_array)
{
    primitive_setter<Primitive, DocOrArray>::set(data, member_info, doc_or_array);
}

template<typename ElementType, typename DocOrArray>
struct element_setter;

template<typename ElementType>
struct element_setter<ElementType, bsoncxx::builder::stream::array> {
    static void set(
            ElementType& element,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::array& array)
    {
        array << element;
    }
};

template<typename ElementType>
struct element_setter<ElementType, bsoncxx::builder::stream::document> {
    static void set(
            ElementType& element,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::document& document)
    {
        document << member_info.member_name()
                 << element;
    }
};


template<typename DocOrArray = bsoncxx::builder::stream::document>
void demultiplex_primitive_member(
        const dds::core::xtypes::DynamicData& data,
        const rti::core::xtypes::DynamicDataMemberInfo& member_info,
        DocOrArray& doc_or_array)
{
    using dds::core::xtypes::TypeKind;
    using dds::core::xtypes::DynamicData;
    using rti::core::xtypes::DynamicDataMemberInfo;

    typedef std::function<void(
            const DynamicData&,
            const DynamicDataMemberInfo&,
            DocOrArray&)> FromPrimitiveFunc;
    static std::unordered_map<int32_t, FromPrimitiveFunc> demux_table{
            {
                    TypeKind::BOOLEAN_TYPE,
                    from_primitive<DDS_Boolean, DocOrArray>
            },
            {
                    TypeKind::CHAR_8_TYPE,
                    from_primitive<DDS_Char, DocOrArray>
            },
            {
                    TypeKind::UINT_8_TYPE,
                    from_primitive<uint8_t, DocOrArray>
            },
            {
                    TypeKind::INT_16_TYPE,
                    from_primitive<int16_t, DocOrArray>
            },
            {
                    TypeKind::UINT_16_TYPE,
                    from_primitive<uint16_t, DocOrArray>
            },
            {
                    TypeKind::INT_32_TYPE,
                    from_primitive<int32_t, DocOrArray>
            },
            {
                    TypeKind::UINT_32_TYPE,
                    from_primitive<int64_t, DocOrArray>
            },
            {
                    TypeKind::INT_64_TYPE,
                    from_primitive<int64_t, DocOrArray>
            },
            {
                    TypeKind::ENUMERATION_TYPE,
                    from_primitive<int64_t, DocOrArray>
            },
            {
                    TypeKind::STRING_TYPE,
                    from_primitive<std::string, DocOrArray>
            },
            {
                    TypeKind::FLOAT_32_TYPE,
                    from_primitive<float_t, DocOrArray>
            },
            {
                    TypeKind::FLOAT_64_TYPE,
                    from_primitive<double, DocOrArray>
            }
    };

    typename std::unordered_map<int32_t, FromPrimitiveFunc>::iterator it =
            demux_table.find(member_info.member_kind().underlying());
    if (it == demux_table.end()) {
        // log unsupported
        return;
    }

    it->second(data, member_info, doc_or_array);
}

class Context {
public:

    Context(uint32_t the_member_count)
            : member_index(1),
              member_count(the_member_count)
    {}

    virtual void set_primitive(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) = 0;

    virtual void set_document(
            bsoncxx::builder::stream::document& document,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) = 0;

    virtual void set_array(
            bsoncxx::builder::stream::array& array,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) = 0;


    uint32_t member_index;
    uint32_t member_count;
};

template<typename DocOrArray>
class TypedContext : public Context {
public:
    TypedContext(uint32_t the_member_count)
            : Context(the_member_count)
    {

    }

    void set_primitive(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) override
    {
        demultiplex_primitive_member(data, member_info, doc_or_array);
    }

    void set_array(
            bsoncxx::builder::stream::array& array,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) override
    {
        bsoncxx::types::b_array array_view{array};
        element_setter<bsoncxx::types::b_array, DocOrArray>::set(
                array_view,
                member_info,
                doc_or_array);
    }

    void set_document(
            bsoncxx::builder::stream::document& document,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) override
    {
        bsoncxx::types::b_document doc_view{document};
        element_setter<bsoncxx::types::b_document, DocOrArray>::set(
                doc_view,
                member_info,
                doc_or_array);
    }

    DocOrArray doc_or_array;
};

typedef TypedContext<bsoncxx::builder::stream::document> DocumentContext;

class ArrayContext : public TypedContext<bsoncxx::builder::stream::array>
{
public:

    ArrayContext(
            uint32_t the_member_index = 1,
            std::vector<uint32_t> the_dimensions = {},
            uint32_t the_dimension_index = 0,
            uint32_t the_element_index = 1)
            : TypedContext<bsoncxx::builder::stream::array>(the_member_index),
              dimensions(the_dimensions),
              dimension_index(the_dimension_index),
              element_index(the_element_index)
    {
        member_index = the_member_index;
    }

    uint32_t dimension_index;
    std::vector<uint32_t> dimensions;
    uint32_t element_index;
};

inline
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


bsoncxx::document::value SampleConverter::to_document(
        const dds::core::xtypes::DynamicData& data)
{
    dds::core::xtypes::DynamicData& casted_data =
            const_cast<dds::core::xtypes::DynamicData&>(data);
    DocumentContext context(data.member_count());
    build_document(context,casted_data);

    return (context.doc_or_array << builder::stream::finalize);
}


/* --- SampleInfo --------------------------------------------------------------------*/


template<typename InfoType, typename DocOrArray = bsoncxx::builder::stream::document>
struct info_member_setter;


template<typename DocOrArray>
struct info_member_setter<dds::core::InstanceHandle, DocOrArray> {
        static void set(
                const std::string& name,
                const dds::core::InstanceHandle& handle,
                DocOrArray& doc_or_array)
        {
            doc_or_array << name << types::b_binary{
                    bsoncxx::binary_sub_type::k_md5,
                    handle->native().keyHash.length,
                    static_cast<const uint8_t *>(&handle->native().keyHash.value[0])};
        }
};

template<typename DocOrArray>
struct info_member_setter<rti::core::Guid, DocOrArray> {
    static void set(
            const std::string& name,
            const rti::core::Guid& guid,
            DocOrArray& doc_or_array)
    {
        doc_or_array << name << types::b_binary{
                bsoncxx::binary_sub_type::k_md5,
                rti::core::Guid::LENGTH,
                static_cast<const uint8_t *>(&guid.native().value[0])};
    }
};


template<typename DocOrArray>
struct info_member_setter<rti::core::SequenceNumber, DocOrArray> {
    static void set(
            const std::string& name,
            const rti::core::SequenceNumber& sn,
            DocOrArray& doc_or_array)
    {
        doc_or_array << name
                << builder::stream::open_document
                <<  "high" << types::b_int32{sn.high()}
                << "low" << types::b_int64{sn.low()}
                << builder::stream::close_document;
    }
};

template<typename DocOrArray>
struct info_member_setter<dds::core::Time, DocOrArray> {
    static void set(
            const std::string& name,
            const dds::core::Time& time,
            DocOrArray& doc_or_array)
    {
        doc_or_array << name
                     << builder::stream::open_document
                     <<  "sec" << types::b_int32{time.sec()}
                     << "nanosec" << types::b_int64{time.nanosec()}
                     << builder::stream::close_document;
    }
};


bsoncxx::document::value SampleConverter::to_document(
        const dds::sub::SampleInfo& info)
{
    using dds::core::InstanceHandle;
    using rti::core::Guid;
    using rti::core::SequenceNumber;
    bsoncxx::builder::stream::document info_doc{};

    #define SAMPLE_CONVERT_ADD_DOC_MEMBER(TYPE, MEMBER) \
    {                                                   \
        info_member_setter<TYPE>::set(                  \
                #MEMBER,                                \
                info->MEMBER(),                          \
                info_doc);                              \
    }

    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            dds::core::InstanceHandle,
            instance_handle);
    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            dds::core::Time,
            source_timestamp);
    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            dds::core::Time,
            reception_timestamp);
    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            rti::core::Guid,
            original_publication_virtual_guid);
    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            rti::core::SequenceNumber,
            original_publication_virtual_sequence_number);
    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            rti::core::Guid,
            related_original_publication_virtual_guid);
    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            rti::core::SequenceNumber,
            related_original_publication_virtual_sequence_number);
    SAMPLE_CONVERT_ADD_DOC_MEMBER(
            rti::core::Guid,
            topic_query_guid);

    return (info_doc << builder::stream::finalize);
}