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
/* dynamic_data_sequences.cxx

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

#include <ndds_cpp.h>
#include <iostream>

using namespace std;

/********* IDL representation for this example ************
 *
 * struct SimpleStruct {
 *     long a_member;
 * };
 *
 * struct TypeWithSequence {
 * 	   sequence<SimpleStruct,5> sequence_member;
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
    struct DDS_StructMemberSeq members;
    DDS_ExceptionCode_t err;

    /* First, we create the typeCode for the simple struct */
    tc = tcf->create_struct_tc("SimpleStruct", members, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << stderr << "! Unable to create sequence TC" << endl;
        goto fail;
    }

    tc->add_member("a_member", DDS_TYPECODE_MEMBER_ID_INVALID,
            tcf->get_primitive_tc(DDS_TK_LONG), DDS_TYPECODE_NONKEY_MEMBER,
            err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << stderr << "! Unable to add a_member to SimpleStruct" << endl;
        goto fail;
    }

    return tc;
    fail:
    if (tc != NULL) {
        tcf->delete_tc(tc, err);
    }
    return NULL;
}

DDS_TypeCode *
sequence_get_typecode(DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode * tc = NULL;
    DDS_TypeCode *seqElementTC = NULL;
    DDS_ExceptionCode_t err;

    seqElementTC = sequence_element_get_typecode(tcf);
    if (seqElementTC == NULL) {
        cerr << "! Unable to create TypeCode" << endl;
        goto fail;
    }
    /* We create the typeCode for the sequence */
    tc = tcf->create_sequence_tc(MAX_SEQ_LEN, seqElementTC, err);

    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to create the sequence TC" << endl;
        goto fail;
    }

    if (seqElementTC != NULL) {
        tcf->delete_tc(seqElementTC, err);
    }
    return tc;

    fail:
    if (seqElementTC != NULL) {
        tcf->delete_tc(seqElementTC, err);
    }
    if (tc != NULL) {
        tcf->delete_tc(tc, err);
    }
    return NULL;
}

DDS_TypeCode *
type_w_sequence_get_typecode(DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode * tc = NULL;
    DDS_TypeCode * sequenceTC = NULL;
    struct DDS_StructMemberSeq members;
    DDS_ExceptionCode_t err;

    sequenceTC = sequence_get_typecode(tcf);
    if (sequenceTC == NULL) {
        cerr << "! Unable to get sequenceTC" << endl;
        goto fail;
    }

    tc = tcf->create_struct_tc("TypeWithSequence", members, err);
    if (tc == NULL) {
        cerr << "! Unable to create Type with sequence TC" << endl;
        goto fail;
    }

    tc->add_member("sequence_member", DDS_TYPECODE_MEMBER_ID_INVALID,
            sequenceTC, DDS_TYPECODE_NONKEY_MEMBER, err);
    if (sequenceTC != NULL) {
        tcf->delete_tc(sequenceTC, err);
    }

    return tc;

    fail: if (sequenceTC != NULL) {
        tcf->delete_tc(sequenceTC, err);
    }
    if (tc != NULL) {
        tcf->delete_tc(tc, err);
    }
    return NULL;
}

void write_data(DDS_DynamicData *sample, DDS_TypeCodeFactory *factory) {
    /* Creating typecodes */
    DDS_TypeCode *sequenceTC = sequence_get_typecode(factory);
    if (sequenceTC == NULL) {
        cerr << "! Unable to create sequence typecode " << endl;
        return;
    }

    DDS_TypeCode *sequenceElementTC = sequence_element_get_typecode(factory);
    if (sequenceElementTC == NULL) {
        cerr << "! Unable to create sequence element typecode " << endl;
        return;
    }
    DDS_ReturnCode_t retcode;
    DDS_ExceptionCode_t err;

    DDS_DynamicData seqmember(sequenceTC, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    DDS_DynamicData seqelement(sequenceElementTC,
            DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    int i = 0;

    for (i = 0; i < MAX_SEQ_LEN; ++i) {

        /* To access the elements of a sequence it is necessary
         * to use their id. This parameter allows accessing to every element
         * of the sequence using a 1-based index.
         * There are two ways of doing this: bind API and get API.
         * See the NestedStructExample for further details about the 
         * differences between these two APIs. */

#ifdef USE_BIND_API
        retcode = seqmember.bind_complex_member(seqelement, NULL,
                i + 1);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to bind complex member" << endl;
            goto fail;
        }

        retcode = seqelement.set_long( "a_member",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, i);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to set a_member long" << endl;
            goto fail;
        }
        printf("Writing sequence element #%d : \n", i + 1);
        seqelement.print(stdout, 1);
        retcode = seqmember.unbind_complex_member(seqelement);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to unbind complex member" << endl;
            goto fail;
        }
#else
        retcode = seqelement.set_long("a_member",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, i);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to set a_member long" << endl;
            goto fail;
        }

        printf("Writing sequence element #%d : \n", i + 1);
        seqelement.print(stdout, 1);
        retcode = seqmember.set_complex_member(NULL, i + 1, seqelement);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to set complex member" << endl;
            goto fail;
        }
#endif
    }

    retcode = DDS_DynamicData_set_complex_member(sample, "sequence_member",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, &seqmember);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set complex member" << endl;
        goto fail;
    }
    fail: if (sequenceTC != NULL) {
        factory->delete_tc(sequenceTC, err);
    }
    if (sequenceElementTC != NULL) {
        factory->delete_tc(sequenceElementTC, err);
    }
    return;
}

void read_data(DDS_DynamicData *sample, DDS_TypeCodeFactory *factory) {
    DDS_TypeCode * sequenceTC = sequence_get_typecode(factory);
    if (sequenceTC == NULL) {
        cerr << "! Unable to get sequence type code" << endl;
        return;
    }

    DDS_DynamicData seqmember(sequenceTC, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

    DDS_DynamicData seqelement(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

    DDS_ReturnCode_t retcode;
    DDS_ExceptionCode_t err;

    DDS_Long value;
    struct DDS_DynamicDataInfo info;
    int i, seqlen;

    sample->get_complex_member(seqmember, "sequence_member",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

    /* Now we get the total amount of elements contained in the array
     * by accessing the dynamic data info
     */
    cout << "* Getting sequence member info...." << endl;
    seqmember.get_info(info);
    seqlen = info.member_count;
    cout << "* Sequence contains " << seqlen << " elements" << endl;

    for (i = 0; i < seqlen; ++i) {
        /*
         * The same results can be obtained using
         * bind_complex_member method. The main difference, is that
         * in that case the members are not copied, just referenced
         */
#ifdef USE_BIND_API
        retcode = seqmember.bind_complex_member(seqelement, NULL,
                i + 1);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to bind complex member" << endl;
            goto fail;
        }
#else
        retcode = seqmember.get_complex_member(seqelement, NULL, i + 1);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to bind complex member" << endl;
            goto fail;
        }
#endif

        retcode = seqelement.get_long(value, "a_member",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to get long member from seq element" << endl;
            goto fail;
        }
        cout << "Reading sequence element #" << i + 1 << " :" << endl;
        seqelement.print(stdout, 1);

#ifdef USE_BIND_API
        retcode = seqmember.unbind_complex_member(seqelement);
        if (retcode != DDS_RETCODE_OK) {
            cerr << "! Unable to u8nbind complex member" << endl;
        }

#endif

    }

    fail:
    if (sequenceTC != NULL) {
        factory->delete_tc(sequenceTC, err);
    }
    return;
}

int main() {
    DDS_TypeCodeFactory *factory = NULL;
    DDS_TypeCode * wSequenceTC = NULL;
    DDS_ExceptionCode_t err;

    factory = DDS_TypeCodeFactory::get_instance();
    if (factory == NULL) {
        cerr << "! Unable to get type code factory singleton" << endl;
        return -1;
    }

    wSequenceTC = type_w_sequence_get_typecode(factory);
    if (wSequenceTC == NULL) {
        cerr << "! Unable to create wSequence Type Code" << endl;
        return -1;
    }

    DDS_DynamicData sample(wSequenceTC, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

    cout << "***** Writing a sample *****" << endl;

    write_data(&sample, factory);

    cout << "***** Reading a sample *****" << endl;
    read_data(&sample, factory);

    /* Delete the created TC */
    if (wSequenceTC != NULL) {
        factory->delete_tc(wSequenceTC, err);
    }

    return 0;
}
