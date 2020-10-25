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
#include <rti/routing/Logger.hpp>
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

/**
 * @brief Helper to add a primitive member to either a document or an array
 *
 * Template specializations are expected to have a single static set() operation
 * to perform the insertion.
 *
 * @tparam Primitive
 *      one of the valid DDS primitive types
 * @tparam DocOrArray
 *      Either a bsoncxx::document or  bsoncxx::array.
 */
template<typename Primitive, typename DocOrArray>
struct primitive_setter;

template<typename Primitive>
struct primitive_setter<Primitive, bsoncxx::builder::stream::document> {

    /**
     * @brief Obtains the value for the specified primitive member from _data_ and
     * adds it to a document.
     *
     * @param[in] data
     *      DynamicData complex member containing the primitive member
     * @param[in] member_info
     *      Description of the primitive member. Must hold a valid member index and name.
     * @param[out] document
     *      Output document
     */
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
    /**
     * @brief Obtains the value for the specified sequence/array item from _data_ and
     * adds it to an array.
     *
     * @param[in] data
     *      DynamicData sequence.array member containing the primitive element.
     * @param[in] member_info
     *      Description of the primitive element. Must contain a valid member index.
     * @param[out] array
     *      Output array
     */
    static void set(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::array& array)
    {
        array << data.value<Primitive>(member_info.member_index());
    }
};

/**
 * @brief Helper function to extra a primitive member or element from DynamicData and
 * inset it into a BSON document or array.
 */
template<typename Primitive, typename DocOrArray = bsoncxx::builder::stream::document>
void from_primitive(
        const dds::core::xtypes::DynamicData& data,
        const rti::core::xtypes::DynamicDataMemberInfo& member_info,
        DocOrArray& doc_or_array)
{
    primitive_setter<Primitive, DocOrArray>::set(data, member_info, doc_or_array);
}

/**
 * @brief Helper function that obtains the value from the specified primitive
 * member and adds it to a bson document or an array.
 *
 * This operation selects the appropriate primitive_setter based on the TypeKind
 * of the primitive member. If the specified member is of an unsupported type, this
 * operation simply logs a message and returns.
 */
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
        std::ostringstream string_stream;
        string_stream << "unsupported type for member=" << member_info.member_name();
        rti::routing::Logger::instance().debug(string_stream.str());
        return;
    }

    it->second(data, member_info, doc_or_array);
}

/**
 * @brief Helper to add a BSON element to either a document or an array
 *
 * Template specializations are expected to have a single static set() operation
 * to perform the insertion.
 *
 * @tparam ElementType
 *      one of the valid bsoncxx::types
 * @tparam DocOrArray
 *      Either a bsoncxx::document or  bsoncxx::array.
 */
template<typename ElementType, typename DocOrArray>
struct element_setter;

template<typename ElementType>
struct element_setter<ElementType, bsoncxx::builder::stream::array> {
    /**
     * @brief Adds the specified element to a bson array.
     *
     * @param[in] element
     *      Instance of a a valid BSON type.
     * @param[in] member_info
     *      Description of the element, as its associated DynamicData member
     * @param[out] array
     *      Output array
     */
    static void set(
            ElementType& element,
            const rti::core::xtypes::DynamicDataMemberInfo&,
            bsoncxx::builder::stream::array& array)
    {
        array << element;
    }
};

template<typename ElementType>
struct element_setter<ElementType, bsoncxx::builder::stream::document> {

    /**
     * @brief Adds the specified element to a bson document.
     *
     * @param[in] element
     *      Instance of a a valid BSON type.
     * @param[in] member_info
     *      Description of the element, as its associated DynamicData member
     * @param[out] document
     *      Output document
     */
    static void set(
            ElementType& element,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::document& document)
    {
        document << member_info.member_name()
                 << element;
    }
};

/**
 * @brief Abstract class that represent a member context, used to build
 * a bsoncxx::document or bsoncxx::array from DynamicData.
 *
 * This class defines the behavior required by the build_document() operation in order
 * to keep track of the nesting level of a member and perform the proper operation to
 * add the member into an output bson document and array.
 */
class ComplexMemberContext {
public:

    /**
     * @brief Creates the context given the total number of members.
     * The underlying _member_index_ is set to 1, the minimum valid index for a DynamicData
     * member.
     */
    ComplexMemberContext(uint32_t the_member_count)
            : member_index(1),
              member_count(the_member_count)
    {}

    /**
     * @brief Sets the primitive member for this  context.
     *
     * @param[in] data
     *      The source DynamicData object
     * @param[in] member_info
     *      Description of the member inside _data_
     */
    virtual void set_primitive(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) = 0;

    /**
     * @brief Sets a bson document in this context.
     *
     * @param[in] document
     *      The source bson document
     * @param[in] member_info
     *      Description of the complex member
     */
    virtual void set_document(
            bsoncxx::builder::stream::document& document,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) = 0;

    /**
     * @brief Sets a bson array in this context.
     *
     * @param[in] array
     *      The source bson array
     * @param[in] member_info
     *      Description of the array/sequence member
     */
    virtual void set_array(
            bsoncxx::builder::stream::array& array,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) = 0;

    // @brief index of the member whose value is retrieved from the DynData object
    uint32_t member_index;
    // @brief total number of members belonging to this complex member context.
    uint32_t member_count;
};

/**
 * @brief Implementation of the ComplexMemberContext that chooses the proper setter
 * specialization based on the destination document or array.
 *
 * The class provides a holder to a destination object instance that is either bson
 * document or array, based on the specialization.
 *
 * @tparam DocOrArray
 *      The type of the destination where members are set.
 */
template<typename DocOrArray>
class TypedComplexMemberContext : public ComplexMemberContext {
public:
    /**
     * @see  ComplexMemberContext(uint32_t)
     */
    TypedComplexMemberContext(uint32_t the_member_count)
            : ComplexMemberContext(the_member_count)
    {

    }

    /**
     * @brief calls demultiplex_primitive_member
     */
    void set_primitive(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info) override
    {
        demultiplex_primitive_member(data, member_info, doc_or_array);
    }

    /**
     * @brief calls element_setter<array>set()
     */
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

    /***
     * @brief calls element_setter<document>set()
     */
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

    // @brief instance to the bson object destination
    DocOrArray doc_or_array;
};

/**
 * @brief Specializaiton of TypedComplexMemberContext for a bson document
 */
typedef TypedComplexMemberContext<bsoncxx::builder::stream::document> DocumentContext;

/**
 * @brief Specializaiton of TypedComplexMemberContext for a bson array.
 *
 * The class also adds additional state needed to properly set items within the destination
 * array.
 */
class ArrayContext : public TypedComplexMemberContext<bsoncxx::builder::stream::array>
{
public:

    ArrayContext(
            uint32_t the_member_index = 1,
            std::vector<uint32_t> the_dimensions = {},
            uint32_t the_dimension_index = 0,
            uint32_t the_element_index = 1)
            : TypedComplexMemberContext<bsoncxx::builder::stream::array>(the_member_index),
              dimensions(the_dimensions),
              dimension_index(the_dimension_index),
              element_index(the_element_index)
    {
        member_index = the_member_index;
    }

    // @brief current dimension for which elements are set
    uint32_t dimension_index;
    // @brief list of dimensions
    std::vector<uint32_t> dimensions;
    // @brief index within the destination array where the element is set
    uint32_t element_index;
};

/***
 * @brief Builds a document from the content of the specified DynamicData object.
 *
 * This is a recursive operation that traverses a DynamicData object using DFS approach.
 * It automatically generates the appropriate complex member–either document or array–
 * based on the type of each member.
 *
 * It is expected that the first call to this operation receives a DocumentContext
 * and the top-level DynamicData object. Subsequent operations are self-made to build
 * the full bson document.
 *
 * @param[in] context
 *      Current complex member context
 * @param[in] data
 *      DynamicData object holding the content for the complex or array member. In
 *      recursive calls, this will be a loaned member.
 */
inline
void build_document(
        ComplexMemberContext& context,
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

/**
 * @brief Helper to add a DDS info member to either a document or an array
 *
 * Template specializations are expected to have a single static set() operation
 * to perform the insertion.
 *
 * @tparam InfoType
 *      one of the valid DDS infrastructure types (e.g., Guid, InstanceHandle, Time_t, etc).
 * @tparam DocOrArray
 *      Either a bsoncxx::document or bsoncxx::array.
 */
template<typename InfoType, typename DocOrArray = bsoncxx::builder::stream::document>
struct info_member_setter;


template<typename DocOrArray>
struct info_member_setter<dds::core::InstanceHandle, DocOrArray> {
        /**
         * @brief Sets the specified handle as an MD5 binary element into a destination
         * document or array.
         *
         * @param[in] name
         *      Name of the member. Can be empty for array destinations.
         * @param[in] handle
         *      InstanceHandle to be set
         * @param[out] doc_or_array
         *      Destination object
         */
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
    /**
     * @brief Sets the specified guid as an MD5 binary element into a destination
     * document or array.
     *
     * @param[in] name
     *      Name of the member. Can be empty for array destinations.
     * @param[in] guid
     *      Guid to be set
     * @param[out] doc_or_array
     *      Destination object
     */
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
    /**
     * @brief Sets the specified sequence number as a document element into a destination
     * document or array.
     *
     * @param[in] name
     *      Name of the member. Can be empty for array destinations.
     * @param[in] sn
     *      SequenceNumber to be set
     * @param[out] doc_or_array
     *      Destination object
     */
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
    /**
     * @brief Sets the specified time as a document element into a destination
     * document or array.
     *
     * @param[in] name
     *      Name of the member. Can be empty for array destinations.
     * @param[in] time
     *      Time to be set
     * @param[out] doc_or_array
     *      Destination object
     */
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
                info->MEMBER(),                         \
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