

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from aws.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Connext distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Connext manual.
*/

#include <iosfwd>
#include <iomanip>

#include "rti/topic/cdr/Serialization.hpp"

#include "aws.hpp"
#include "awsPlugin.hpp"

#include <rti/util/ostream_operators.hpp>

// ---- aws: 

aws::aws() :
    m_key_ (0) ,
    m_number_ (0)  {
}   

aws::aws (
    int32_t key,
    int32_t number)
    :
        m_key_( key ),
        m_number_( number ) {
}

#ifdef RTI_CXX11_RVALUE_REFERENCES
#ifdef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
aws::aws(aws&& other_) OMG_NOEXCEPT  :m_key_ (std::move(other_.m_key_))
,
m_number_ (std::move(other_.m_number_))
{
} 

aws& aws::operator=(aws&&  other_) OMG_NOEXCEPT {
    aws tmp(std::move(other_));
    swap(tmp); 
    return *this;
}
#endif
#endif   

void aws::swap(aws& other_)  OMG_NOEXCEPT 
{
    using std::swap;
    swap(m_key_, other_.m_key_);
    swap(m_number_, other_.m_number_);
}  

bool aws::operator == (const aws& other_) const {
    if (m_key_ != other_.m_key_) {
        return false;
    }
    if (m_number_ != other_.m_number_) {
        return false;
    }
    return true;
}
bool aws::operator != (const aws& other_) const {
    return !this->operator ==(other_);
}

// --- Getters and Setters: -------------------------------------------------
int32_t& aws::key() OMG_NOEXCEPT {
    return m_key_;
}

const int32_t& aws::key() const OMG_NOEXCEPT {
    return m_key_;
}

void aws::key(int32_t value) {
    m_key_ = value;
}

int32_t& aws::number() OMG_NOEXCEPT {
    return m_number_;
}

const int32_t& aws::number() const OMG_NOEXCEPT {
    return m_number_;
}

void aws::number(int32_t value) {
    m_number_ = value;
}

std::ostream& operator << (std::ostream& o,const aws& sample)
{
    rti::util::StreamFlagSaver flag_saver (o);
    o <<"[";
    o << "key: " << sample.key()<<", ";
    o << "number: " << sample.number() ;
    o <<"]";
    return o;
}

// --- Type traits: -------------------------------------------------

namespace rti { 
    namespace topic {

        template<>
        struct native_type_code<aws> {
            static DDS_TypeCode * get()
            {
                static RTIBool is_initialized = RTI_FALSE;

                static DDS_TypeCode_Member aws_g_tc_members[2]=
                {

                    {
                        (char *)"key",/* Member name */
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
                        RTI_CDR_KEY_MEMBER , /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL/* Ignored */
                    }, 
                    {
                        (char *)"number",/* Member name */
                        {
                            1,/* Representation ID */          
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
                        NULL/* Ignored */
                    }
                };

                static DDS_TypeCode aws_g_tc =
                {{
                        DDS_TK_STRUCT,/* Kind */
                        DDS_BOOLEAN_FALSE, /* Ignored */
                        -1, /*Ignored*/
                        (char *)"aws", /* Name */
                        NULL, /* Ignored */      
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        2, /* Number of members */
                        aws_g_tc_members, /* Members */
                        DDS_VM_NONE  /* Ignored */         
                    }}; /* Type code for aws*/

                if (is_initialized) {
                    return &aws_g_tc;
                }

                aws_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_long;

                aws_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_long;

                is_initialized = RTI_TRUE;

                return &aws_g_tc;
            }
        }; // native_type_code

        const dds::core::xtypes::StructType& dynamic_type<aws>::get()
        {
            return static_cast<const dds::core::xtypes::StructType&>(
                rti::core::native_conversions::cast_from_native<dds::core::xtypes::DynamicType>(
                    *(native_type_code<aws>::get())));
        }

    }
}  

namespace dds { 
    namespace topic {
        void topic_type_support<aws>:: register_type(
            dds::domain::DomainParticipant& participant,
            const std::string& type_name) 
        {

            rti::domain::register_type_plugin(
                participant,
                type_name,
                awsPlugin_new,
                awsPlugin_delete);
        }

        std::vector<char>& topic_type_support<aws>::to_cdr_buffer(
            std::vector<char>& buffer, const aws& sample)
        {
            // First get the length of the buffer
            unsigned int length = 0;
            RTIBool ok = awsPlugin_serialize_to_cdr_buffer(
                NULL, 
                &length,
                &sample);
            rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to calculate cdr buffer size");

            // Create a vector with that size and copy the cdr buffer into it
            buffer.resize(length);
            ok = awsPlugin_serialize_to_cdr_buffer(
                &buffer[0], 
                &length, 
                &sample);
            rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to copy cdr buffer");

            return buffer;
        }

        void topic_type_support<aws>::from_cdr_buffer(aws& sample, 
        const std::vector<char>& buffer)
        {

            RTIBool ok  = awsPlugin_deserialize_from_cdr_buffer(
                &sample, 
                &buffer[0], 
                static_cast<unsigned int>(buffer.size()));
            rti::core::check_return_code(ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
            "Failed to create aws from cdr buffer");
        }

        void topic_type_support<aws>::reset_sample(aws& sample) 
        {
            rti::topic::reset_sample(sample.key());
            rti::topic::reset_sample(sample.number());
        }

        void topic_type_support<aws>::allocate_sample(aws& sample, int, int) 
        {
        }

    }
}  

