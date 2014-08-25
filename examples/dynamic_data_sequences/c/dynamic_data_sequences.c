/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/
/* dynamic_data_sequences.c

 This example
 - creates the type code of a sequence
 - creates a DynamicData instance
 - writes and reads the values

 Example:

 To run the example application:

 On Unix:

 objs/<arch>/SequencesExample

 On Windows:

 objs\<arch>\SequencesExample
 */

#include <ndds_c.h>

/********* IDL representation for this example ************
 *
 * struct SimpleStruct {
 *     long a_member;
 * };
 *
 * struct TypeWithSequence {
 *     sequence<SimpleStruct,5> sequence_member;
 * };
 */

#define MAX_SEQ_LEN 5
/*
 * Uncomment to use the bind api instead of the get API.
 */
/* #define USE_BIND_API */

DDS_TypeCode *
sequence_element_get_typecode(DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode * tc = NULL;
    struct DDS_StructMemberSeq members = DDS_SEQUENCE_INITIALIZER;
    DDS_ExceptionCode_t err;

    /* First, we create the typeCode for the simple struct */
    tc = DDS_TypeCodeFactory_create_struct_tc(tcf, "SimpleStruct", &members,
            &err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "! Unable to create sequence TC\n");
        goto fail;
    }

    DDS_TypeCode_add_member(tc, "a_member", DDS_TYPECODE_MEMBER_ID_INVALID,
            DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_LONG),
            DDS_TYPECODE_NONKEY_MEMBER, &err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "! Unable to add a_member to SimpleStruct\n");
        goto fail;
    }

    return tc;

    fail:
    if (tc != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, tc, &err);
    }
    return NULL;
}

DDS_TypeCode *
sequence_get_typecode(DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode * tc = NULL;
    DDS_TypeCode * seqElementTC = NULL;
    DDS_ExceptionCode_t err;

    seqElementTC = sequence_element_get_typecode(tcf);
    if (seqElementTC == NULL) {
        fprintf(stderr, "! Unable to create typeCode\n");
        goto fail;
    }

    /* We create the typeCode for the sequence */
    tc = DDS_TypeCodeFactory_create_sequence_tc(tcf, MAX_SEQ_LEN, seqElementTC,
            &err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "! Unable to create the sequence TC\n");
        goto fail;
    }

    if (seqElementTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, seqElementTC, &err);
    }
    return tc;

    fail:
    if (seqElementTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, seqElementTC, &err);
    }

    if (tc != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, tc, &err);
    }

    return NULL;
}

DDS_TypeCode *
type_w_sequence_get_typecode(DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode * tc = NULL;
    struct DDS_TypeCode * sequenceTC = NULL;
    struct DDS_StructMemberSeq members = DDS_SEQUENCE_INITIALIZER;
    DDS_ExceptionCode_t err;

    sequenceTC = sequence_get_typecode(tcf);
    if (sequenceTC == NULL) {
        fprintf(stderr, "! Unable to get sequenceTC\n");
        goto fail;
    }

    tc = DDS_TypeCodeFactory_create_struct_tc(tcf, "TypeWithSequence", &members,
            &err);
    if (tc == NULL) {
        fprintf(stderr, "! Unable to create Type with sequence TC\n");
        goto fail;
    }

    DDS_TypeCode_add_member(tc, "sequence_member",
            DDS_TYPECODE_MEMBER_ID_INVALID, sequenceTC,
            DDS_TYPECODE_NONKEY_MEMBER, &err);

    if (sequenceTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, sequenceTC, &err);
    }

    DDS_StructMemberSeq_finalize(&members);
    return tc;

    fail:
    if (tc != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, tc, &err);
    }
    if (sequenceTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, sequenceTC, &err);
    }
    DDS_StructMemberSeq_finalize(&members);
    return NULL;
}

void write_data(DDS_DynamicData *sample, DDS_TypeCodeFactory *factory) {
    DDS_DynamicData seqmember;
    DDS_DynamicData seqelement;

    DDS_Boolean seqMemberIsInitialized = DDS_BOOLEAN_FALSE;
    DDS_Boolean seqElementIsInitialized = DDS_BOOLEAN_FALSE;

    DDS_TypeCode *sequenceTC = NULL;
    DDS_TypeCode *sequenceElementTC = NULL;

    DDS_ExceptionCode_t err;
    DDS_ReturnCode_t retcode;

    int i = 0;

    sequenceTC = sequence_get_typecode(factory);
    if (sequenceTC == NULL) {
        fprintf(stderr, "! Unable to create typeCode\n");
        goto fail;
    }

    sequenceElementTC = sequence_element_get_typecode(factory);
    if (sequenceElementTC == NULL) {
        fprintf(stderr, "! Unable to create typeCode\n");
        goto fail;
    }

    seqMemberIsInitialized = DDS_DynamicData_initialize(&seqmember, sequenceTC,
            &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if (!seqMemberIsInitialized) {
        fprintf(stderr, "! Unable to initialize seqMember\n");
        goto fail;
    }

    seqElementIsInitialized = DDS_DynamicData_initialize(&seqelement,
            sequenceElementTC, &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if (!seqElementIsInitialized) {
        fprintf(stderr, "! Unable to initialize seqElement\n");
        goto fail;
    }

    for (i = 0; i < MAX_SEQ_LEN; ++i) {

        /* To access the elements of a sequence it is necessary
         * to use their id. This parameter allows accessing to every element
         * of the sequence using a 1-based index.
         * There are two ways of doing this: bind API and get API.
         * See the NestedStructExample for further details about the 
         * differences between these two APIs. */

#ifdef USE_BIND_API
        retcode = DDS_DynamicData_bind_complex_member(&seqmember, &seqelement,
                NULL, i + 1);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr,"! Unable to bind complex member\n");
            goto fail;
        }
        retcode = DDS_DynamicData_set_long(&seqelement, "a_member",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, i);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr,"! Unable to set a_member long\n");
            goto fail;
        }
        printf("Writing sequence element #%d : \n", i + 1);
        DDS_DynamicData_print(&seqelement, stdout, 1);
        retcode = DDS_DynamicData_unbind_complex_member(&seqmember, &seqelement);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr,"! Unable to unbind complex member\n");
            goto fail;
        }
#else
        retcode = DDS_DynamicData_set_long(&seqelement, "a_member",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, i);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "! Unable to set a_member long\n");
            goto fail;
        }
        printf("Writing sequence element #%d : \n", i + 1);
        DDS_DynamicData_print(&seqelement, stdout, 1);
        retcode = DDS_DynamicData_set_complex_member(&seqmember, NULL, i + 1,
                &seqelement);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "! Unable to set complex member\n");
            goto fail;
        }
#endif
    }

    retcode = DDS_DynamicData_set_complex_member(sample, "sequence_member",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, &seqmember);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "! Unable to set complex member\n");
        goto fail;
    }

    fail:
    if (sequenceTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, sequenceTC, &err);
    }

    if (sequenceElementTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, sequenceElementTC, &err);
    }

    if (seqMemberIsInitialized) {
        DDS_DynamicData_finalize(&seqmember);
    }

    if (seqElementIsInitialized) {
        DDS_DynamicData_finalize(&seqelement);
    }
    return;
}

void read_data(DDS_DynamicData *sample, DDS_TypeCodeFactory *factory) {
    struct DDS_TypeCode *sequenceTC = NULL;

    DDS_DynamicData seqmember;
    DDS_DynamicData seqelement;
    DDS_Boolean seqMemberIsInitialized = DDS_BOOLEAN_FALSE;
    DDS_Boolean seqElementIsInitialized = DDS_BOOLEAN_FALSE;

    DDS_Long value;
    struct DDS_DynamicDataInfo info;
    int i, seqlen;
    DDS_ReturnCode_t retcode;
    DDS_ExceptionCode_t err;

    sequenceTC = sequence_get_typecode(factory);
    if (sequenceTC == NULL) {
        fprintf(stderr, "! Unable to get sequence type code\n");
        goto fail;
    }
    seqMemberIsInitialized = DDS_DynamicData_initialize(&seqmember, sequenceTC,
            &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if (!seqMemberIsInitialized) {
        fprintf(stderr, "! Unable to initialize seqMember\n");
        goto fail;
    }

    seqElementIsInitialized = DDS_DynamicData_initialize(&seqelement, NULL,
            &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if (!seqElementIsInitialized) {
        fprintf(stderr, "! Unable to initialize seqElement\n");
        goto fail;
    }

    retcode = DDS_DynamicData_get_complex_member(sample, &seqmember,
            "sequence_member", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "! Unable to get complex member from seq member\n");
        goto fail;
    }

    /* Now we get the total amount of elements contained in the sequence
     * by accessing the dynamic data info
     */
    printf("* Getting sequence member info....\n");

    DDS_DynamicData_get_info(&seqmember, &info);
    seqlen = info.member_count;

    printf("* Sequence contains %d elements\n", seqlen);

    for (i = 0; i < seqlen; ++i) {
        /*
         * The same results can be obtained using
         * bind_complex_member method. The main difference, is that
         * in that case the members are not copied, just referenced
         */
#ifdef USE_BIND_API
        retcode = DDS_DynamicData_bind_complex_member(&seqmember, &seqelement,
                NULL, i + 1);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr,"! Unable to bind complex member\n");
            goto fail;
        }
#else
        retcode = DDS_DynamicData_get_complex_member(&seqmember, &seqelement,
                NULL, i + 1);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "! Unable to get complex member\n");
            goto fail;
        }
#endif

        retcode = DDS_DynamicData_get_long(&seqelement, &value, "a_member",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "! Unable to get long member from seq element\n");
            goto fail;
        }

        printf("Reading sequence element #%d : \n", i + 1);
        DDS_DynamicData_print(&seqelement, stdout, 1);

#ifdef USE_BIND_API
        retcode = DDS_DynamicData_unbind_complex_member(&seqmember, &seqelement);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "! Unable to unbind complex member\n");
        }
#endif

    }
    fail: if (sequenceTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, sequenceTC, &err);
    }

    if (seqMemberIsInitialized) {
        DDS_DynamicData_finalize(&seqmember);
    }

    if (seqElementIsInitialized) {
        DDS_DynamicData_finalize(&seqelement);
    }

    return;
}

int main() {
    DDS_DynamicData sample;
    DDS_TypeCodeFactory *factory = NULL;
    DDS_TypeCode *wSequenceTC = NULL;
    DDS_Boolean dynamicDataIsInitialized = DDS_BOOLEAN_FALSE;

    factory = DDS_TypeCodeFactory_get_instance();
    if (factory == NULL) {
        fprintf(stderr, "! Unable to get type code factory singleton\n");
        goto fail;
    }

    wSequenceTC = type_w_sequence_get_typecode(factory);
    if (wSequenceTC == NULL) {
        fprintf(stderr, "! Unable to create wSequence Type Code\n");
        goto fail;
    }

    dynamicDataIsInitialized = DDS_DynamicData_initialize(&sample, wSequenceTC,
            &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if (!dynamicDataIsInitialized) {
        fprintf(stderr, "! Unable to create dynamicData\n");
        goto fail;
    }
    printf("***** Writing a sample *****\n");

    write_data(&sample, factory);

    printf("***** Reading a sample *****\n");
    read_data(&sample, factory);

    fail:
    if (wSequenceTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, wSequenceTC, NULL);
    }
    if (dynamicDataIsInitialized) {
        DDS_DynamicData_finalize(&sample);
    }
    return 0;
}
