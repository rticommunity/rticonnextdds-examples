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
/* dynamic_data_union_example.c

 This example
 - creates the type code of for a union
 - creates a DynamicData instance
 - sets one of the members of the union
 - shows how to retrieve the discriminator
 - access to the value of one member of the union

 Example:

 To run the example application:

 On Unix:

 objs/<arch>/UnionExample

 On Windows:

 objs\<arch>\UnionExample
 */

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"

struct DDS_TypeCode *create_type_code(struct DDS_TypeCodeFactory *factory) {
    struct DDS_TypeCode *unionTC = NULL;
    DDS_ExceptionCode_t ex;

    /* First, we create the typeCode for a union */
    /* Default-member index refers to which member of the union
     * will be the default one. In this example index = 1 refers
     * to the the member 'aLong'. Take into account that index
     * starts in 0 */
    unionTC = DDS_TypeCodeFactory_create_union_tc(factory, "Foo",
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_LONG),
            1, /* default-member index */
            NULL, /* For now, it does not have any member */
            &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "! Unable to create union TC\n");
        goto fail;
    }

    /* Case 1 will be a short named aShort */
    DDS_TypeCode_add_member(unionTC, "aShort",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_SHORT),
            DDS_TYPECODE_NONKEY_MEMBER, &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "! Unable to add member aShort\n");
        goto fail;
    }

    /* Case 2, the default, will be a long named aLong */
    DDS_TypeCode_add_member(unionTC, "aLong", 2,
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_LONG),
            DDS_TYPECODE_NONKEY_MEMBER, &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "! Unable to add member aLong\n");
        goto fail;
    }

    /* Case 3 will be a double named aDouble */
    DDS_TypeCode_add_member(unionTC, "aDouble", 3,
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_DOUBLE),
            DDS_TYPECODE_NONKEY_MEMBER, &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr, "! Unable to add member aDouble\n");
        goto fail;
    }

    return unionTC;

    fail: if (unionTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, unionTC, &ex);
    }
    return NULL;
}

int example() {
    struct DDS_TypeCode *unionTC = NULL;
    struct DDS_DynamicData data;
    DDS_ReturnCode_t retcode;
    struct DDS_DynamicDataMemberInfo info;
    struct DDS_TypeCodeFactory *factory = NULL;
    int ret = -1;
    struct DDS_DynamicDataProperty_t myProperty =
            DDS_DYNAMIC_DATA_PROPERTY_DEFAULT;
    DDS_Short valueTestShort;
    DDS_Boolean dynamicDataIsInitialized = DDS_BOOLEAN_FALSE;

    /* Getting a reference to the type code factory */
    factory = DDS_TypeCodeFactory_get_instance();
    if (factory == NULL) {
        fprintf(stderr, "! Unable to get type code factory singleton\n");
        goto fail;
    }

    /* Creating the union typeCode */
    unionTC = create_type_code(factory);
    if (unionTC == NULL) {
        fprintf(stderr, "! Unable to create typeCode\n");
        goto fail;
    }

    /* Creating a new dynamicData instance based on the union type code */
    dynamicDataIsInitialized = DDS_DynamicData_initialize(&data, unionTC,
            &myProperty);
    if (!dynamicDataIsInitialized) {
        fprintf(stderr, "! Unable to create dynamicData\n");
        goto fail;
    }

    /* If we get the current discriminator, it will be the default one
     (not the first added) */
    retcode = DDS_DynamicData_get_member_info_by_index(&data, &info, 0);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "! Unable to get member info\n");
        goto fail;
    }

    printf("The member selected is %s\n", info.member_name);

    /* When we set a value in one of the members of the union,
     * the discriminator updates automatically to point that member.  */
    retcode = DDS_DynamicData_set_short(&data, "aShort",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 42);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "! Unable to set value to aShort member\n");
        goto fail;
    }

    /* The discriminator should now reflect the new situation */
    retcode = DDS_DynamicData_get_member_info_by_index(&data, &info, 0);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "! Unable to get member info\n");
        goto fail;
    }

    printf("The member selected is %s\n", info.member_name);

    /* Getting the value of the target member */
    retcode = DDS_DynamicData_get_short(&data, &valueTestShort, "aShort",
            DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "! Unable to get member value\n");
        goto fail;
    }

    printf("The member selected is %s with value: %d\n", info.member_name,
            valueTestShort);

    ret = 1;

    fail: 
    if (unionTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, unionTC, NULL);
    }

    if (dynamicDataIsInitialized) {
        DDS_DynamicData_finalize(&data);
    }

    return ret;
}

int main(int argc, char *argv[]) {
    return example();
}
