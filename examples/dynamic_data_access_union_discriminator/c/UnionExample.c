/* UnionExample.c
    
    This example 
      - creates the type code of for a union 
      - creates a DynamicData instance
      - sets one of the fields of the union
      - shows how to retrieve the discriminator 
   
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

struct DDS_TypeCode *createTypeCode(struct DDS_TypeCodeFactory *factory) {
    struct DDS_TypeCode *unionTC = NULL;
    DDS_ExceptionCode_t ex;

    /* First, we create the typeCode for a union*/
    unionTC = DDS_TypeCodeFactory_create_union_tc(
            factory,
            "Foo",
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_LONG),
            1, /* default index */
            NULL, /* For now, it does not have any member */
            &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to create union TC\n");
        goto fail;
    }

    /* Case 1 will be a short named aShort */
    DDS_TypeCode_add_member(
            unionTC,
            "aShort",
            -1, /* no default member */
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_SHORT),
            DDS_TYPECODE_NONKEY_MEMBER,
            &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to add member aShort\n");
        goto fail;
    }

    /* Case 2 will be a long named aLong */
    DDS_TypeCode_add_member(
            unionTC,
            "aLong",
            2,
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_LONG),
            DDS_TYPECODE_NONKEY_MEMBER,
            &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to add member aLong\n");
        goto fail;
    }

    /* Case 3, the default, will be a double named aDouble */
    DDS_TypeCode_add_member(
            unionTC,
            "aDouble",
            3,
            DDS_TypeCodeFactory_get_primitive_tc(factory, DDS_TK_DOUBLE),
            DDS_TYPECODE_NONKEY_MEMBER,
            &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to add member aDouble\n");
        goto fail;
    }

    return unionTC;

fail:
    if (unionTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, unionTC,&ex);
    }
    return NULL;
}


int example () {
    struct DDS_TypeCode *unionTC = NULL;
    struct DDS_DynamicData data;
    DDS_ReturnCode_t retcode;
    struct DDS_DynamicDataMemberInfo info;
    struct DDS_TypeCodeFactory *factory = NULL;
    int ret = -1;
    struct DDS_DynamicDataProperty_t  myProperty = DDS_DYNAMIC_DATA_PROPERTY_DEFAULT;

    /* I get a reference to the type code factory */	
    factory = DDS_TypeCodeFactory_get_instance();
    if (factory == NULL) {
        fprintf(stderr, "! Unable to get type code factory singleton\n");
        goto fail;
    }

    /* Creating the union typeCode */
    unionTC = createTypeCode(factory);
    if (unionTC == NULL) {
        fprintf(stderr,"! Unable to create typeCode\n");
        goto fail;
    }

    /* Creating a new dynamicData instance based on the union type code */
    if (!DDS_DynamicData_initialize(&data,unionTC,&myProperty)) {
        fprintf(stderr,"! Unable to create dynamicData\n");
        goto fail;
    }

    /* If I get the discriminator now, I will get the first one added */
    retcode = DDS_DynamicData_get_member_info_by_index(
            &data,&info,0);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to get member info\n");
        goto fail;
    }

    fprintf(stdout, "The field selected is %s\n", info.member_name);    

    /* Now I set one of the members */
    retcode = DDS_DynamicData_set_short(&data,
            "aShort",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 42);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to create dynamicData\n");
        goto fail;
    }
    
    /* The discriminator should now reflect the new situation */
    retcode = DDS_DynamicData_get_member_info_by_index(
            &data,&info,0);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to get member info\n");
        goto fail;
    }

    fprintf(stdout, "The field selected is %s\n", info.member_name);

    ret = 1;
fail:
    if (unionTC != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, unionTC,NULL);
    }

    return ret;
}

int main(int argc, char *argv[]) {
    return example();
}
