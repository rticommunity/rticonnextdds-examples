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
/* dynamic_data_union_example.cxx

 This example
 - creates the type code of for a union
 - creates a DynamicData instance
 - sets one of the members of the union
 - shows how to retrieve the discriminator
 - access to the value of one member of the union

 Example:

 To run the example application:

 On Unix:

 objs/<arch>/union_example

 On Windows:

 objs\<arch>\union_example
 */

#include <iostream>
#include <ndds_cpp.h>

using namespace std;

DDS_TypeCode *create_type_code(DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode *unionTC = NULL;
    struct DDS_UnionMemberSeq members;
    DDS_ExceptionCode_t err;

    /* First, we create the typeCode for a union */
    /* Default-member index refers to which member of the union
     * will be the default one. In this example index = 1 refers
     * to the the member 'aLong'. Take into account that index
     * starts in 0 */
    unionTC = tcf->create_union_tc("Foo", DDS_MUTABLE_EXTENSIBILITY,
            tcf->get_primitive_tc(DDS_TK_LONG), 1, /* default-member index */
            members, /* For now, it does not have any member */
            err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to create typecode: " << err << endl;
        goto fail;
    }

    /* Case 1 will be a short named aShort */
    unionTC->add_member("aShort", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            tcf->get_primitive_tc(DDS_TK_SHORT), DDS_TYPECODE_NONKEY_MEMBER,
            err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member aShort: " << err << endl;
        goto fail;
    }

    /* Case 2, the default, will be a long named aLong */
    unionTC->add_member("aLong", 2, tcf->get_primitive_tc(DDS_TK_LONG),
            DDS_TYPECODE_NONKEY_MEMBER, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member aLong: " << err << endl;
        goto fail;
    }

    /* Case 3 will be a double named aDouble */
    unionTC->add_member("aDouble", 3, tcf->get_primitive_tc(DDS_TK_DOUBLE),
            DDS_TYPECODE_NONKEY_MEMBER, err);
    if (err != DDS_NO_EXCEPTION_CODE) {
        cerr << "! Unable to add member aDouble: " << err << endl;
        goto fail;
    }

    return unionTC;

    fail: if (unionTC != NULL) {
        tcf->delete_tc(unionTC, err);
    }
    return NULL;
}

int example() {
    /* Creating the union typeCode */
    DDS_TypeCodeFactory *tcf = NULL;
    tcf = DDS_TypeCodeFactory::get_instance();
    if (tcf == NULL) {
        cerr << "! Unable to get type code factory singleton" << endl;
        return -1;
    }

    struct DDS_TypeCode *unionTC = create_type_code(tcf);
    DDS_ReturnCode_t retcode;
    struct DDS_DynamicDataMemberInfo info;
    int ret = -1;
    DDS_ExceptionCode_t err;
    struct DDS_DynamicDataProperty_t myProperty =
            DDS_DYNAMIC_DATA_PROPERTY_DEFAULT;
    DDS_Short valueTestShort;

    /* Creating a new dynamicData instance based on the union type code */
    struct DDS_DynamicData data(unionTC, myProperty);
    if (!data.is_valid()) {
        cerr << "! Unable to create dynamicData" << endl;
        goto fail;
    }

    /* If we get the current discriminator, it will be the default one
     * (not the first added) */
    retcode = data.get_member_info_by_index(info, 0);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to get member info" << endl;
        goto fail;
    }

    cout << "The member selected is " << info.member_name << endl;

    /* When we set a value in one of the members of the union,
     * the discriminator updates automatically to point that member.  */
    retcode = data.set_short("aShort", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            42);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to set short member " << endl;
        goto fail;
    }

    /* The discriminator should now reflect the new situation */
    retcode = data.get_member_info_by_index(info, 0);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to get member info" << endl;
        goto fail;
    }

    cout << "The member selected is " << info.member_name << endl;

    /* Getting the value of the target member */
    retcode = data.get_short(valueTestShort, "aShort",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
    if (retcode != DDS_RETCODE_OK) {
        cerr << "! Unable to get member value" << endl;
        goto fail;
    }

    cout << "The member selected is " << info.member_name << "with value: "
            << valueTestShort << endl;

    ret = 1;

    fail:
    if (unionTC != NULL) {
        tcf->delete_tc(unionTC, err);
    }
    return ret;
}

int main(int argc, char *argv[]) {
    return example();
}
