/*******************************************************************************
 (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <iostream>
#include <ndds_cpp.h>

using namespace std;

/*****************************************************************************/
/************** This file defines the type for following commented Idl *******/
/*****************************************************************************/

/* --- Request type: ------------------------------------------------------- */

/* 
 * This is the request type. 
 *
 * It simply contains an integer that requests all the prime numbers below it.
 *
 *
*/

/*
struct PrimeNumberRequest {
        //Requests the calculation of the prime numbers below n 
        long n;
        //How many prime numbers should be included in each reply sample. 
        //A smaller value means that results are received more frequently
        //(and hence more data samples are sent)
        long primes_per_reply;
};

*/
/* --- Reply type: --------------------------------------------------------- */
/*
//Maximum size of the a sequence of prime numbers
const long PRIME_SEQUENCE_MAX_LENGTH = 1024;
//Indicates the status of one of the multiple possible replies for a request
enum PrimeNumberCalculationStatus {
        //Indicates that this reply contains a new sequence of
        //prime numbers for a request, but there are still more to come
        REPLY_IN_PROGRESS,
        //Indicates that this is the last sequence of 
        //prime numbers for a request.
        REPLY_COMPLETED,
        //Indicates that there was an error. After an error
        //there won't be any more replies for a request
        REPLY_ERROR
};


// The replier sends sequences of prime numbers as they are being calculated.
struct PrimeNumberReply {
        // A sequence of prime numbers
        sequence<long, PRIME_SEQUENCE_MAX_LENGTH> primes;
        
        
        // Status information about this reply
         PrimeNumberCalculationStatus status;
};
*/

#define THE_PRIME_SEQUENCE_MAX_LENGTH 1024

/*****************************************************************************/
/* Returns a C String with the name of this data type                        */
/*****************************************************************************/
const char * HelloWorldType_get_type_name(){
    return "HelloWorld";
}

/*****************************************************************************/
/* Get the typecode for the prime_number_request                             */
/*****************************************************************************/
DDS_TypeCode * create_prime_number_request_typecode(DDS_TypeCodeFactory * tcf) {
    
    static DDS_TypeCode *request_typecode =NULL;
    
    DDS_StructMemberSeq members;
    DDS_ExceptionCode_t err;

    /* We create the typecode for a struct: */
    request_typecode = tcf->create_struct_tc("PrimeNumberRequest", members, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to create struct TC" << endl;
        goto fail;
    }
    /* We add a member "n" to the struct: */
    request_typecode->add_member("n",
                                DDS_TYPECODE_MEMBER_ID_INVALID, 
                                tcf->get_primitive_tc(DDS_TK_LONG),
                                DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, /* As required member. */
                                err);

    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << stderr << "! Unable to add a_member to PrimeNumberRequest" << endl;
        goto fail;
    }
    /* We add a member "primes_per_reply" to the struct: */
    request_typecode->add_member("primes_per_reply",
                                DDS_TYPECODE_MEMBER_ID_INVALID, 
                                tcf->get_primitive_tc(DDS_TK_LONG),
                                DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, /*As required member. */
                                err);

    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << stderr << "! Unable to add a_member to PrimeNumberRequest" << endl;
        goto fail;
    }

    /* We finalize and return the Typecode: */
    DDS_StructMemberSeq_finalize(&members);
    return request_typecode;

    fail: if (request_typecode != NULL) {
        tcf->delete_tc(request_typecode, err);
    }
    DDS_StructMemberSeq_finalize(&members);

    return NULL;

}

/*****************************************************************************/
/* Get the typecode for the PrimeNumberCalculationStatus                     */
/*****************************************************************************/
DDS_TypeCode * create_PrimeNumberCalculationStatus_enum_typecode(
        DDS_TypeCodeFactory *tcf) {
    
    static DDS_TypeCode *enumTC = NULL;
    struct DDS_EnumMemberSeq members;
    DDS_ExceptionCode_t err;

    /* First, we create the typeCode for the enum */
	enumTC = tcf->create_enum_tc("PrimeNumberCalculationStatus", members, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to create typecode: " << err << endl;
        goto fail;
    }

	/* Case 1: We add the member REPLY_IN_PROGRESS with value 0 */
	enumTC->add_member_to_enum("REPLY_IN_PROGRESS", 0, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
       cerr << "! Unable to add member REPLY_IN_PROGRESS: " << err << endl;
       goto fail;
    }

	/* Case 2: We add the member REPLY_COMPLETED with value 1 */
	enumTC->add_member_to_enum("REPLY_COMPLETED", 1, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
       cerr << "! Unable to add member REPLY_IN_COMPLETED: " << err << endl;
       goto fail;
    }

    /* Case 3: We add the member REPLY_ERROR with value 2 */
	enumTC->add_member_to_enum("REPLY_ERROR", 2, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
       cerr << "! Unable to add member REPLY_IN_ERROR: " << err << endl;
       goto fail;
    }

    return enumTC;

    fail: 
    if (enumTC != NULL) {
        tcf->delete_tc(enumTC, err);
    }
    return NULL;

}

/*****************************************************************************/
/* Get the typecode for the sequence of longs                                */
/*****************************************************************************/
DDS_TypeCode * create_PrimeNumberReply_sequence_typecode(DDS_TypeCodeFactory *tcf) {

    static DDS_TypeCode *reply_typecode = NULL;
    DDS_ExceptionCode_t err;

    /* We create the typeCode for the sequence */
    reply_typecode = tcf->create_sequence_tc(
            THE_PRIME_SEQUENCE_MAX_LENGTH, 
            tcf->get_primitive_tc(DDS_TK_LONG),
            err);

    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to create the sequence reply_typecode" << endl;
        goto fail;
    }

    return reply_typecode;

fail:
    if (reply_typecode != NULL) {
        tcf->delete_tc(reply_typecode, err);
    }
    return NULL;
}

/*****************************************************************************/
/* Get the typecode for the PrimeNumberReply                                 */
/*****************************************************************************/
DDS_TypeCode * type_w_PrimeNumberReply_typecode(DDS_TypeCodeFactory *tcf) {

    static DDS_TypeCode *tc = NULL;
    DDS_TypeCode *sequenceTC = NULL;
    DDS_TypeCode *enumTC = NULL;
    struct DDS_StructMemberSeq members;
    DDS_ExceptionCode_t err;

    /* We get the Typecode for the sequence: */
    sequenceTC = get_PrimeNumberReply_sequence_typecode(tcf);
    if (sequenceTC == NULL) {
        cerr << "! Unable to get sequenceTC" << endl;
        goto fail;
    }

    /* We get the typecode for the enum: */
    enumTC = get_PrimeNumberCalculationStatus_enum_typecode(tcf);
    if (enumTC == NULL) {
        cerr << "! Unable to create innerTC" << endl;
        goto fail;
    }

    tc = tcf->create_struct_tc("PrimeNumberReply", members, err);
    if (tc == NULL) {
        cerr << "! Unable to create Type with sequence TC" << endl;
        goto fail;
    }
    /* We add the sequence to the struct */
    tc->add_member(
            "primes",
            DDS_TYPECODE_MEMBER_ID_INVALID,
            sequenceTC,
            DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, /* As required member. */
            err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member to struct" << endl;
        goto fail;
    }
    /* We add the status Enum to the struct */
    tc->add_member(
            "status",
            DDS_TYPECODE_MEMBER_ID_INVALID,
            enumTC,
            DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, /* As required member. */
            err);

    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member to struct" << endl;
        goto fail;
    }

    return tc;

fail:
    if (sequenceTC != NULL) {
        tcf->delete_tc(sequenceTC, err);
    }
    if (tc != NULL) {
        tcf->delete_tc(tc, err);
    }
    return NULL;
}
