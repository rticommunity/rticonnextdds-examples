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
/* dynamic_data_nested_struct_example.cxx

 This example
 - creates the type code of for a nested struct
 (with inner and outer structs)
 - creates a DynamicData instance
 - sets the values of the inner struct
 - shows the differences between set_complex_member and bind_complex_member

 Example:

 To run the example application:

 On Unix:

 objs/<arch>/NestedStructExample

 On Windows:

 objs\<arch>\NestedStructExample
 */
/********* IDL representation for this example ************
 
 struct InnerStruct {
 double x;
 double y;
 };

 struct OuterStruct {
 InnerStruct inner;
 };

 */
#include <iostream>
#include <ndds_cpp.h>

using namespace std;

DDS_TypeCode *
inner_struct_get_typecode(DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode *tc = NULL;
    DDS_StructMemberSeq members;
    DDS_ExceptionCode_t err;

    /* First, we create the typeCode for a struct */
    tc = tcf->create_struct_tc("InnerStruct", members, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to create struct TC" << endl;
        goto fail;
    }

    /* Member 1 will be a double named x */
    tc->add_member("x", DDS_TYPECODE_MEMBER_ID_INVALID,
            DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
            DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member x" << endl;
        goto fail;
    }

    /* Member 2 will be a double named y */
    tc->add_member("y", DDS_TYPECODE_MEMBER_ID_INVALID,
            DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
            DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member y" << endl;
        goto fail;
    }

    DDS_StructMemberSeq_finalize(&members);
    return tc;

    fail: if (tc != NULL) {
        tcf->delete_tc(tc, err);
    }
    DDS_StructMemberSeq_finalize(&members);
    return NULL;
}

DDS_TypeCode *
outer_struct_get_typecode(DDS_TypeCodeFactory * tcf) {
    static DDS_TypeCode *tc = NULL;
    DDS_TypeCode *innerTC = NULL;

    struct DDS_StructMemberSeq members;
    DDS_ExceptionCode_t err;

    /* First, we create the typeCode for a struct */
    tc = tcf->create_struct_tc("OuterStruct", members, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to create struct TC" << endl;
        goto fail;
    }

    innerTC = inner_struct_get_typecode(tcf);
    if (innerTC == NULL) {
        cerr << "! Unable to create innerTC" << endl;
        goto fail;
    }
    /* Member 1 of outer struct will be a struct of type inner_struct 
     * called inner*/
    tc->add_member("inner", DDS_TYPECODE_MEMBER_ID_INVALID, innerTC,
            DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member inner struct" << endl;
        goto fail;
    }

    if (innerTC != NULL) {
        tcf->delete_tc(innerTC, err);
    }

    DDS_StructMemberSeq_finalize(&members);
    return tc;

    fail: if (tc != NULL) {
        tcf->delete_tc(tc, err);
    }
    DDS_StructMemberSeq_finalize(&members);

    if (innerTC != NULL) {
        tcf->delete_tc(innerTC, err);
    }
    return NULL;
}

int main() {
    DDS_TypeCodeFactory *tcf = NULL;
    DDS_ExceptionCode_t err;

    /* Getting a reference to the type code factory */
    tcf = DDS_TypeCodeFactory::get_instance();
    if (tcf == NULL) {
        cerr << "! Unable to get type code factory singleton" << endl;
        return -1;
    }

    /* Creating the typeCode of the inner_struct */
    struct DDS_TypeCode *inner_tc = inner_struct_get_typecode(tcf);
    if (inner_tc == NULL) {
        cerr << "! Unable to create inner typecode " << endl;
        return -1;
    }

    /* Creating the typeCode of the outer_struct that contains an inner_struct */
    struct DDS_TypeCode *outer_tc = outer_struct_get_typecode(tcf);
    if (inner_tc == NULL) {
        cerr << "! Unable to create outer typecode " << endl;
        tcf->delete_tc(outer_tc, err);
        return -1;
    }

    DDS_ReturnCode_t retcode;
    int ret = -1;

    /* Now, we create a dynamicData instance for each type */
    DDS_DynamicData outer_data(outer_tc, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    DDS_DynamicData inner_data(inner_tc, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    DDS_DynamicData bounded_data(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

    cout << " Connext Dynamic Data Nested Struct Example" << endl
            << "--------------------------------------------" << endl;

    cout << " Data Types" << endl << "------------------" << endl;
    inner_tc->print_IDL(0, err);
    outer_tc->print_IDL(0, err);

    /* Setting the inner data */
    retcode = inner_data.set_double("x", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            3.14159);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set value 'x' in the inner struct" << endl;
        goto fail;
    }

    retcode = inner_data.set_double("y", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            2.71828);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set value 'y' in the inner struct" << endl;
        goto fail;
    }

    cout << endl << endl << " get/set_complex_member API" << endl
            << "------------------" << endl;
    /* Using set_complex_member, we copy inner_data values in inner_struct of
     * outer_data */
    cout << "Setting the initial values of struct with set_complex_member()"
            << endl;
    retcode = outer_data.set_complex_member("inner",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, inner_data);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set complex struct value "
                << "(member inner in the outer struct)" << endl;
        goto fail;
    }

    outer_data.print(stdout, 1);

    retcode = inner_data.clear_all_members();
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to clear all member in the inner struct" << endl;
        goto fail;
    }

    cout << endl << " + get_complex_member() called" << endl;

    retcode = outer_data.get_complex_member(inner_data, "inner",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to get complex struct value"
                << "(member inner in the outer struct)" << endl;
        goto fail;
    }
    cout << endl << " + inner_data value" << endl;
    inner_data.print(stdout, 1);

    /* get_complex_member made a copy of the inner_struct. If we modify
     * inner_data values, outer_data inner_struct WILL NOT be modified. */
    cout << endl << " + setting new values to inner_data" << endl;
    retcode = inner_data.set_double("x", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            1.00000);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set value 'x' in the inner struct" << endl;
        goto fail;
    }

    retcode = inner_data.set_double("y", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            0.00001);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set value 'y' in the inner struct" << endl;
        goto fail;
    }

    /* Current value of outer_data
     * outer:
     * inner:
     *     x: 3.141590
     *     y: 2.718280
     * inner_data:
     *     x: 1.000000
     *     y: 0.000010
     */
    cout << endl << " + current outer_data value " << endl;
    outer_data.print(stdout, 1);

    /* Bind/Unbind member API */
    cout << endl << endl << "bind/unbind API" << endl << "------------------"
            << endl;

    /* Using bind_complex_member, we do not copy inner_struct, but bind it. 
     * So, if we modify bounded_data, the inner member inside outer_data WILL
     * also be modified */
    cout << endl << " + bind complex member called" << endl;
    retcode = outer_data.bind_complex_member(bounded_data, "inner",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to bind the structs" << endl;
        goto fail;
    }

    bounded_data.print(stdout, 1);

    cout << endl << " + setting new values to bounded_data" << endl;
    retcode = bounded_data.set_double("x",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 1.00000);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set value to 'x' with the bounded data" << endl;
        goto fail;
    }

    retcode = bounded_data.set_double("y",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 0.00001);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set value to 'y' in the bounded data" << endl;
        goto fail;
    }

    /* Current value of outer data
     * outer:
     * inner:
     *     x: 1.000000
     *     y: 0.000010
     */

    bounded_data.print(stdout, 1);

    retcode = outer_data.unbind_complex_member(bounded_data);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to unbind the data" << endl;
        goto fail;
    }

    cout << endl << " + current outer_data value " << endl;
    outer_data.print(stdout, 1);

    ret = 1;

    fail: if (inner_tc != NULL) {
        tcf->delete_tc(inner_tc, err);
    }

    if (outer_tc != NULL) {
        tcf->delete_tc(outer_tc, err);
    }
    return ret;
}
