/*******************************************************************************
 (c) 2005-2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#ifndef Primes_hpp
#define Primes_hpp

#include <dds/dds.hpp>

static const int32_t PRIME_SEQUENCE_MAX_LENGTH = 1024L;
enum class PrimeNumberCalculationStatus {
    REPLY_IN_PROGRESS, 
    REPLY_COMPLETED, 
    REPLY_ERROR
};

class PrimeNumberTypeBuilder
{
public:
    PrimeNumberTypeBuilder() :
        m_request_type_("PrimeNumberRequest"),
        m_reply_type_("PrimeNumberReply")
    {
        m_request_type_.add_member(
            dds::core::xtypes::Member("n", dds::core::xtypes::primitive_type<int32_t>())
        );
        m_request_type_.add_member(
            dds::core::xtypes::Member(
                "primes_per_reply",
                dds::core::xtypes::primitive_type<int32_t>()
            )
        );

        m_reply_type_.add_member(
            dds::core::xtypes::Member(
                "primes",
                dds::core::xtypes::SequenceType(
                    dds::core::xtypes::primitive_type<int32_t>(), PRIME_SEQUENCE_MAX_LENGTH
                )
            )
        );
        m_reply_type_.add_member(
            dds::core::xtypes::Member(
                "status",
                dds::core::xtypes::EnumType(
                    "PrimeNumberCalculationStatus",
                    {
                        dds::core::xtypes::EnumMember(
                            "REPLY_IN_PROGRESS",
                            static_cast<int32_t>(PrimeNumberCalculationStatus::REPLY_IN_PROGRESS)
                        ),
                        dds::core::xtypes::EnumMember(
                            "REPLY_COMPLETED",
                            static_cast<int32_t>(PrimeNumberCalculationStatus::REPLY_COMPLETED)
                        ),
                        dds::core::xtypes::EnumMember(
                            "REPLY_ERROR",
                            static_cast<int32_t>(PrimeNumberCalculationStatus::REPLY_ERROR)
                        )
                    }
                )
            )
        );
    }

    const dds::core::xtypes::StructType& request_type() const {
        return m_request_type_;
    };

    const dds::core::xtypes::StructType& reply_type() const {
        return m_reply_type_;
    };

private:
    dds::core::xtypes::StructType m_request_type_;
    dds::core::xtypes::StructType m_reply_type_;
};

#endif // Primes_hpp
