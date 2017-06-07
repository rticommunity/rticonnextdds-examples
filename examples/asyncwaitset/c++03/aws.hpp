

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from aws.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Connext distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Connext manual.
*/

#ifndef aws_1074856872_hpp
#define aws_1074856872_hpp

#include <iosfwd>

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport __declspec(dllexport)
#endif

#include "dds/domain/DomainParticipant.hpp"
#include "dds/topic/TopicTraits.hpp"
#include "dds/core/SafeEnumeration.hpp"
#include "dds/core/String.hpp"
#include "dds/core/array.hpp"
#include "dds/core/vector.hpp"
#include "dds/core/Optional.hpp"
#include "dds/core/xtypes/DynamicType.hpp"
#include "dds/core/xtypes/StructType.hpp"
#include "dds/core/xtypes/UnionType.hpp"
#include "dds/core/xtypes/EnumType.hpp"
#include "dds/core/xtypes/AliasType.hpp"
#include "rti/core/array.hpp"
#include "rti/core/BoundedSequence.hpp"
#include "rti/util/StreamFlagSaver.hpp"
#include "rti/domain/PluginSupport.hpp"
#include "rti/core/LongDouble.hpp"
#include "dds/core/External.hpp"
#include "rti/core/Pointer.hpp"
#include "rti/topic/TopicTraits.hpp"
#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

class NDDSUSERDllExport aws {

  public:
    aws();
    aws(
        int32_t key,
        int32_t number);

    #ifdef RTI_CXX11_RVALUE_REFERENCES
    #ifndef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
    aws (aws&&) = default;
    aws& operator=(aws&&) = default;
    aws& operator=(const aws&) = default;
    aws(const aws&) = default;
    #else
    aws(aws&& other_) OMG_NOEXCEPT;  
    aws& operator=(aws&&  other_) OMG_NOEXCEPT;
    #endif
    #endif 

    int32_t& key() OMG_NOEXCEPT; 
    const int32_t& key() const OMG_NOEXCEPT;
    void key(int32_t value);

    int32_t& number() OMG_NOEXCEPT; 
    const int32_t& number() const OMG_NOEXCEPT;
    void number(int32_t value);

    bool operator == (const aws& other_) const;
    bool operator != (const aws& other_) const;

    void swap(aws& other_) OMG_NOEXCEPT ;

  private:

    int32_t m_key_;
    int32_t m_number_;

};

inline void swap(aws& a, aws& b)  OMG_NOEXCEPT 
{
    a.swap(b);
}

NDDSUSERDllExport std::ostream& operator<<(std::ostream& o, const aws& sample);

namespace dds { 
    namespace topic {

        template<>
        struct topic_type_name<aws> {
            NDDSUSERDllExport static std::string value() {
                return "aws";
            }
        };

        template<>
        struct is_topic_type<aws> : public dds::core::true_type {};

        template<>
        struct topic_type_support<aws> {
            NDDSUSERDllExport 
            static void register_type(
                dds::domain::DomainParticipant& participant,
                const std::string & type_name);

            NDDSUSERDllExport 
            static std::vector<char>& to_cdr_buffer(
                std::vector<char>& buffer, const aws& sample);

            NDDSUSERDllExport 
            static void from_cdr_buffer(aws& sample, const std::vector<char>& buffer);

            NDDSUSERDllExport 
            static void reset_sample(aws& sample);

            NDDSUSERDllExport 
            static void allocate_sample(aws& sample, int, int);

            static const rti::topic::TypePluginKind::type type_plugin_kind = 
            rti::topic::TypePluginKind::STL;
        };

    }
}

namespace rti { 
    namespace topic {
        template<>
        struct dynamic_type<aws> {
            typedef dds::core::xtypes::StructType type;
            NDDSUSERDllExport static const dds::core::xtypes::StructType& get();
        };

        template <>
        struct extensibility<aws> {
            static const dds::core::xtypes::ExtensibilityKind::type kind =
            dds::core::xtypes::ExtensibilityKind::EXTENSIBLE;                
        };

    }
}

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif // aws_1074856872_hpp

