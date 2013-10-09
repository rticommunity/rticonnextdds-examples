
/*
  WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

  This file was generated from HelloWorld.idl using "rtiddsgen".
  The rtiddsgen tool is part of the RTI Connext distribution.
  For more information, type 'rtiddsgen -help' at a command shell
  or consult the RTI Connext manual.
*/


#ifndef NDDS_STANDALONE_TYPE
    #ifdef __cplusplus
        #ifndef ndds_cpp_h
            #include "ndds/ndds_cpp.h"
        #endif
        #ifndef dds_c_log_impl_h              
            #include "dds_c/dds_c_log_impl.h"                                
        #endif        
    #else
        #ifndef ndds_c_h
            #include "ndds/ndds_c.h"
        #endif
    #endif
    
    #ifndef cdr_type_h
        #include "cdr/cdr_type.h"
    #endif    

    #ifndef osapi_heap_h
        #include "osapi/osapi_heap.h" 
    #endif
#else
    #include "ndds_standalone_type.h"
#endif



#include "HelloWorld.h"

/* ========================================================================= */
const char *HelloWorldTYPENAME = "HelloWorld";

DDS_TypeCode* HelloWorld_get_typecode()
{
    static RTIBool is_initialized = RTI_FALSE;

    static DDS_TypeCode HelloWorld_g_tc_prefix_string = DDS_INITIALIZE_STRING_TYPECODE((HELLODDS_MAX_STRING_SIZE));
    static DDS_TypeCode HelloWorld_g_tc_payload_sequence = DDS_INITIALIZE_SEQUENCE_TYPECODE((HELLODDS_MAX_PAYLOAD_SIZE),NULL);

    static DDS_TypeCode_Member HelloWorld_g_tc_members[3]=
    {
        {
            (char *)"prefix",/* Member name */
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
            DDS_BOOLEAN_FALSE, /* Is a key? */
            DDS_PRIVATE_MEMBER,/* Ignored */
            0,/* Ignored */
            NULL/* Ignored */
        },
        {
            (char *)"sampleId",/* Member name */
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
            DDS_BOOLEAN_FALSE, /* Is a key? */
            DDS_PRIVATE_MEMBER,/* Ignored */
            0,/* Ignored */
            NULL/* Ignored */
        },
        {
            (char *)"payload",/* Member name */
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
            DDS_BOOLEAN_FALSE, /* Is a key? */
            DDS_PRIVATE_MEMBER,/* Ignored */
            0,/* Ignored */
            NULL/* Ignored */
        }
    };

    static DDS_TypeCode HelloWorld_g_tc =
    {{
        DDS_TK_STRUCT,/* Kind */
        DDS_BOOLEAN_FALSE, /* Ignored */
        -1,/* Ignored */
        (char *)"HelloWorld", /* Name */
        NULL, /* Ignored */
        0, /* Ignored */
        0, /* Ignored */
        NULL, /* Ignored */
        3, /* Number of members */
        HelloWorld_g_tc_members, /* Members */
        DDS_VM_NONE /* Ignored */
    }}; /* Type code for HelloWorld*/

    if (is_initialized) {
        return &HelloWorld_g_tc;
    }

    HelloWorld_g_tc_payload_sequence._data._typeCode = (RTICdrTypeCode *)&DDS_g_tc_octet;

    HelloWorld_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&HelloWorld_g_tc_prefix_string;
    HelloWorld_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_long;
    HelloWorld_g_tc_members[2]._representation._typeCode = (RTICdrTypeCode *)&HelloWorld_g_tc_payload_sequence;

    is_initialized = RTI_TRUE;

    return &HelloWorld_g_tc;
}


RTIBool HelloWorld_initialize(
    HelloWorld* sample) {
  return HelloWorld_initialize_ex(sample,RTI_TRUE,RTI_TRUE);
}
        
RTIBool HelloWorld_initialize_ex(
    HelloWorld* sample,RTIBool allocatePointers,RTIBool allocateMemory)
{

    void* buffer = NULL;
    if (buffer) {} /* To avoid warnings */
        
    
    if (allocatePointers) {} /* To avoid warnings */
    if (allocateMemory) {} /* To avoid warnings */

    if (allocateMemory) {
        sample->prefix = DDS_String_alloc(((HELLODDS_MAX_STRING_SIZE)));
        if (sample->prefix == NULL) {
            return RTI_FALSE;
        }
    } else {
        if (sample->prefix != NULL) { 
            sample->prefix[0] = '\0';
        }
    }
            

    if (!RTICdrType_initLong(&sample->sampleId)) {
        return RTI_FALSE;
    }                
            

    if (allocateMemory) {
        DDS_OctetSeq_initialize(&sample->payload);
        if (!DDS_OctetSeq_set_maximum(&sample->payload,
                ((HELLODDS_MAX_PAYLOAD_SIZE)))) {
            return RTI_FALSE;
        }
    } else {
        DDS_OctetSeq_set_length(&sample->payload, 0); 
    }
            


    return RTI_TRUE;
}

void HelloWorld_finalize(
    HelloWorld* sample)
{
    HelloWorld_finalize_ex(sample,RTI_TRUE);
}
        
void HelloWorld_finalize_ex(
    HelloWorld* sample,RTIBool deletePointers)
{        
    if (sample) { } /* To avoid warnings */
    if (deletePointers) {} /* To avoid warnings */


    DDS_String_free(sample->prefix);                
            


    DDS_OctetSeq_finalize(&sample->payload);
            

}

RTIBool HelloWorld_copy(
    HelloWorld* dst,
    const HelloWorld* src)
{        

    if (!RTICdrType_copyString(
        dst->prefix, src->prefix, ((HELLODDS_MAX_STRING_SIZE)) + 1)) {
        return RTI_FALSE;
    }
            

    if (!RTICdrType_copyLong(
        &dst->sampleId, &src->sampleId)) {
        return RTI_FALSE;
    }
            

    if (!DDS_OctetSeq_copy_no_alloc(&dst->payload,
                                          &src->payload)) {
        return RTI_FALSE;
    }
            


    return RTI_TRUE;
}


/**
 * <<IMPLEMENTATION>>
 *
 * Defines:  TSeq, T
 *
 * Configure and implement 'HelloWorld' sequence class.
 */
#define T HelloWorld
#define TSeq HelloWorldSeq
#define T_initialize_ex HelloWorld_initialize_ex
#define T_finalize_ex   HelloWorld_finalize_ex
#define T_copy       HelloWorld_copy

#ifndef NDDS_STANDALONE_TYPE
#include "dds_c/generic/dds_c_sequence_TSeq.gen"
#ifdef __cplusplus
#include "dds_cpp/generic/dds_cpp_sequence_TSeq.gen"
#endif
#else
#include "dds_c_sequence_TSeq.gen"
#ifdef __cplusplus
#include "dds_cpp_sequence_TSeq.gen"
#endif
#endif

#undef T_copy
#undef T_finalize_ex
#undef T_initialize_ex
#undef TSeq
#undef T

