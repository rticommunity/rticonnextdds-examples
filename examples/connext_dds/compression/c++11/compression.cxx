

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from compression.idl
using RTI Code Generator (rtiddsgen) version 3.1.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#include <iosfwd>
#include <iomanip>

#include "rti/topic/cdr/Serialization.hpp"

#include "compression.hpp"
#include "compressionPlugin.hpp"

#include <rti/util/ostream_operators.hpp>

// ---- StringLine: 

StringLine::StringLine() :
    m_str_ ("")  {
}   

StringLine::StringLine (
    const std::string& str)
    :
        m_str_( str ) {
}

unsigned int max_string_length = 4096;

#ifdef RTI_CXX11_RVALUE_REFERENCES
#ifdef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
StringLine::StringLine(StringLine&& other_) OMG_NOEXCEPT  :m_str_ (std::move(other_.m_str_))
{
} 

StringLine& StringLine::operator=(StringLine&&  other_) OMG_NOEXCEPT {
    StringLine tmp(std::move(other_));
    swap(tmp); 
    return *this;
}
#endif
#endif   

void StringLine::swap(StringLine& other_)  OMG_NOEXCEPT 
{
    using std::swap;
    swap(m_str_, other_.m_str_);
}  

bool StringLine::operator == (const StringLine& other_) const {
    if (m_str_ != other_.m_str_) {
        return false;
    }
    return true;
}
bool StringLine::operator != (const StringLine& other_) const {
    return !this->operator ==(other_);
}

std::ostream& operator << (std::ostream& o,const StringLine& sample)
{
    ::rti::util::StreamFlagSaver flag_saver (o);
    o <<"[";
    o << "str: " << sample.str() ;
    o <<"]";
    return o;
}

// --- Type traits: -------------------------------------------------

namespace rti { 
    namespace topic {

        #ifndef NDDS_STANDALONE_TYPE
        template<>
        struct native_type_code< StringLine > {
            static DDS_TypeCode * get()
            {
                using namespace ::rti::topic::interpreter;

                static RTIBool is_initialized = RTI_FALSE;

                static DDS_TypeCode StringLine_g_tc_str_string;

                static DDS_TypeCode_Member StringLine_g_tc_members[1]=
                {

                    {
                        (char *)"str",/* Member name */
                        {
                            0,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }
                };

                static DDS_TypeCode StringLine_g_tc =
                {{
                        DDS_TK_STRUCT, /* Kind */
                        DDS_BOOLEAN_FALSE, /* Ignored */
                        -1, /*Ignored*/
                        (char *)"StringLine", /* Name */
                        NULL, /* Ignored */      
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        1, /* Number of members */
                        StringLine_g_tc_members, /* Members */
                        DDS_VM_NONE, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER,
                        DDS_BOOLEAN_TRUE, /* _isCopyable */
                        NULL, /* _sampleAccessInfo: assigned later */
                        NULL /* _typePlugin: assigned later */
                    }}; /* Type code for StringLine*/

                if (is_initialized) {
                    return &StringLine_g_tc;
                }

                StringLine_g_tc_str_string =
                        initialize_string_typecode((max_string_length));

                StringLine_g_tc._data._annotations._allowedDataRepresentationMask = 5;

                StringLine_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&StringLine_g_tc_str_string;

                /* Initialize the values for member annotations. */
                StringLine_g_tc_members[0]._annotations._defaultValue._d = RTI_XCDR_TK_STRING;
                StringLine_g_tc_members[0]._annotations._defaultValue._u.string_value = (DDS_Char *) "";

                StringLine_g_tc._data._sampleAccessInfo = sample_access_info();
                StringLine_g_tc._data._typePlugin = type_plugin_info();    

                is_initialized = RTI_TRUE;

                return &StringLine_g_tc;
            }

            static RTIXCdrSampleAccessInfo * sample_access_info()
            {
                static RTIBool is_initialized = RTI_FALSE;

                StringLine *sample;

                static RTIXCdrMemberAccessInfo StringLine_g_memberAccessInfos[1] =
                {RTIXCdrMemberAccessInfo_INITIALIZER};

                static RTIXCdrSampleAccessInfo StringLine_g_sampleAccessInfo = 
                RTIXCdrSampleAccessInfo_INITIALIZER;

                if (is_initialized) {
                    return (RTIXCdrSampleAccessInfo*) &StringLine_g_sampleAccessInfo;
                }

                RTIXCdrHeap_allocateStruct(
                    &sample, 
                    StringLine);
                if (sample == NULL) {
                    return NULL;
                }

                StringLine_g_memberAccessInfos[0].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->str() - (char *)sample);

                StringLine_g_sampleAccessInfo.memberAccessInfos = 
                StringLine_g_memberAccessInfos;

                {
                    size_t candidateTypeSize = sizeof(StringLine);

                    if (candidateTypeSize > RTIXCdrLong_MAX) {
                        StringLine_g_sampleAccessInfo.typeSize[0] =
                        RTIXCdrLong_MAX;
                    } else {
                        StringLine_g_sampleAccessInfo.typeSize[0] =
                        (RTIXCdrUnsignedLong) candidateTypeSize;
                    }
                }

                StringLine_g_sampleAccessInfo.useGetMemberValueOnlyWithRef =
                RTI_XCDR_TRUE;

                StringLine_g_sampleAccessInfo.getMemberValuePointerFcn = 
                interpreter::get_aggregation_value_pointer< StringLine >;

                StringLine_g_sampleAccessInfo.languageBinding = 
                RTI_XCDR_TYPE_BINDING_CPP_11_STL ;

                RTIXCdrHeap_freeStruct(sample);
                is_initialized = RTI_TRUE;
                return (RTIXCdrSampleAccessInfo*) &StringLine_g_sampleAccessInfo;
            }

            static RTIXCdrTypePlugin * type_plugin_info()
            {
                static RTIXCdrTypePlugin StringLine_g_typePlugin = 
                {
                    NULL, /* serialize */
                    NULL, /* serialize_key */
                    NULL, /* deserialize_sample */
                    NULL, /* deserialize_key_sample */
                    NULL, /* skip */
                    NULL, /* get_serialized_sample_size */
                    NULL, /* get_serialized_sample_max_size_ex */
                    NULL, /* get_serialized_key_max_size_ex */
                    NULL, /* get_serialized_sample_min_size */
                    NULL, /* serialized_sample_to_key */
                    NULL,
                    NULL,
                    NULL,
                    NULL
                };

                return &StringLine_g_typePlugin;
            }
        }; // native_type_code
        #endif

        const ::dds::core::xtypes::StructType& dynamic_type< StringLine >::get()
        {
            return static_cast<const ::dds::core::xtypes::StructType&>(
                ::rti::core::native_conversions::cast_from_native< ::dds::core::xtypes::DynamicType >(
                    *(native_type_code< StringLine >::get())));
        }

    }
}

namespace dds { 
    namespace topic {
        void topic_type_support< StringLine >:: register_type(
            ::dds::domain::DomainParticipant& participant,
            const std::string& type_name) 
        {

            ::rti::domain::register_type_plugin(
                participant,
                type_name,
                StringLinePlugin_new,
                StringLinePlugin_delete);
        }

        std::vector<char>& topic_type_support< StringLine >::to_cdr_buffer(
            std::vector<char>& buffer, 
            const StringLine& sample,
            ::dds::core::policy::DataRepresentationId representation)
        {
            // First get the length of the buffer
            unsigned int length = 0;
            RTIBool ok = StringLinePlugin_serialize_to_cdr_buffer(
                NULL, 
                &length,
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to calculate cdr buffer size");

            // Create a vector with that size and copy the cdr buffer into it
            buffer.resize(length);
            ok = StringLinePlugin_serialize_to_cdr_buffer(
                &buffer[0], 
                &length, 
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to copy cdr buffer");

            return buffer;
        }

        void topic_type_support< StringLine >::from_cdr_buffer(StringLine& sample, 
        const std::vector<char>& buffer)
        {

            RTIBool ok  = StringLinePlugin_deserialize_from_cdr_buffer(
                &sample, 
                &buffer[0], 
                static_cast<unsigned int>(buffer.size()));
            ::rti::core::check_return_code(ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
            "Failed to create StringLine from cdr buffer");
        }

        void topic_type_support< StringLine >::reset_sample(StringLine& sample) 
        {
            sample.str("");
        }

        void topic_type_support< StringLine >::allocate_sample(StringLine& sample, int, int) 
        {
            ::rti::topic::allocate_sample(sample.str(), -1, max_string_length);
        }

    }
}  

