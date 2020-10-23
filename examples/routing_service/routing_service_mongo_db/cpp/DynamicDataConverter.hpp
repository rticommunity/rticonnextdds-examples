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

#include <dds/core/xtypes/DynamicData.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

namespace rti {
namespace community {
namespace examples {



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
            bsoncxx::builder::stream::array& doc_or_array)
    {
        doc_or_array << element;
    }
};

template<typename ElementType>
struct element_setter<ElementType, bsoncxx::builder::stream::document> {
    static void set(
            ElementType& element,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::document& doc_or_array)
    {
        doc_or_array << member_info.member_name()
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
typedef TypedContext<bsoncxx::builder::stream::array> ArrayContext;

class DynamicDataConverter {
public:


public:
    DynamicDataConverter();


    static bsoncxx::document::value to_document(
            const dds::core::xtypes::DynamicData& data);


    template<typename T>
    static void from_native_primitive(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::document& document)
    {
        document << member_info.member_name()
                 << data.value<T>(member_info.member_index());

    }

    template<typename T>
    static void from_native_primitive(
            const dds::core::xtypes::DynamicData& data,
            const rti::core::xtypes::DynamicDataMemberInfo& member_info,
            bsoncxx::builder::stream::array& array)
    {
        array << data.value<T>(member_info.member_index());
    }

private:

};

}
}
}

#endif /* DYNAMICDATACONVERTER_HPP */

