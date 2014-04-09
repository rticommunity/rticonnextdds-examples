/* NestedStructExample.c
    
    This example 
      - creates the type code of for a nested struct 
            (with inner and outer structs) 
      - creates a DynamicData instance
      - sets the values of the inner struct
      - see the differents between set_complex_member and bind_complex_member
   
    Example:
        
       To run the example application:
                                  
       On Unix: 
       
       objs/<arch>/NestedStructExample 
                            
       On Windows:
       
       objs\<arch>\NestedStructExample  
*/

#include <ndds_c.h>

DDS_TypeCode *
inner_struct_get_typecode(struct DDS_TypeCodeFactory *tcf) {
    static DDS_TypeCode *tc = NULL;
    struct DDS_StructMemberSeq members = DDS_SEQUENCE_INITIALIZER;
    DDS_ExceptionCode_t ex;

    /* First, we create the typeCode for a struct */
	tc = DDS_TypeCodeFactory_create_struct_tc(tcf, "InnerStruct", &members, &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to create struct TC\n");
        goto fail;
    }

    /* Case 1 will be a double named x */
    DDS_TypeCode_add_member(tc, "x", DDS_TYPECODE_MEMBER_ID_INVALID,
            DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
            DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, &ex);

    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to add member x\n");
        goto fail;
    }
        
    /* Case 2 will be a double named y */
    DDS_TypeCode_add_member(tc, "y", DDS_TYPECODE_MEMBER_ID_INVALID,
	        DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
	        DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to add member y\n");
        goto fail;
    }

    DDS_StructMemberSeq_finalize(&members);
    return tc;

fail:
    if (tc != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, tc, &ex);
    }
    DDS_StructMemberSeq_finalize(&members);
    return NULL;
	
}

DDS_TypeCode *
outer_struct_get_typecode(struct DDS_TypeCodeFactory *tcf) {
	static DDS_TypeCode *tc = NULL;
	struct DDS_StructMemberSeq members= DDS_SEQUENCE_INITIALIZER;
	DDS_ExceptionCode_t ex;

    /* First, we create the typeCode for a struct */
	tc = DDS_TypeCodeFactory_create_struct_tc(tcf,"OuterStruct",
	    &members,&ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to create struct TC\n");
        goto fail;
    }

    /* This struct just will have a data named inner, type: struct inner */
	DDS_TypeCode_add_member(tc,"inner",DDS_TYPECODE_MEMBER_ID_INVALID,
	                inner_struct_get_typecode(tcf),
	                DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, &ex);
    if (ex != DDS_NO_EXCEPTION_CODE) {
        fprintf(stderr,"! Unable to add member x\n");
        goto fail;
    }

    DDS_StructMemberSeq_finalize(&members);
	return tc;

fail:
    if (tc != NULL) {
        DDS_TypeCodeFactory_delete_tc(tcf, tc, &ex);
    }
    DDS_StructMemberSeq_finalize(&members);
    return NULL;
}


int main() {
    struct DDS_TypeCode *inner_tc = NULL; 
    struct DDS_TypeCode *outer_tc = NULL;
    struct DDS_TypeCodeFactory *factory = NULL;
    
    DDS_ExceptionCode_t err;
    DDS_ReturnCode_t retcode;
    int ret = -1;

    DDS_DynamicData *outer_data = NULL;
    DDS_DynamicData *inner_data = NULL;
    DDS_DynamicData *bounded_data = NULL;

    /* Getting a reference to the type code factory */	
    factory = DDS_TypeCodeFactory_get_instance();
    if (factory == NULL) {
        fprintf(stderr, "! Unable to get type code factory singleton\n");
        goto fail;
    }

    /* Creating the inner typeCode */
    inner_tc = inner_struct_get_typecode(factory);
    if (inner_tc == NULL) {
        fprintf(stderr,"! Unable to create typeCode\n");
        goto fail;
    }

    /* Creating the outer typeCode */
    outer_tc = outer_struct_get_typecode(factory);
    if (inner_tc == NULL) {
        fprintf(stderr,"! Unable to create typeCode\n");
        goto fail;
    }

	printf(" Connext Dynamic Data Nested Struct Example \n"
	       "--------------------------------------------\n");


	printf(" Data Types\n"
	       "------------------\n");
	DDS_TypeCode_print_IDL(inner_tc,0,&err);
	DDS_TypeCode_print_IDL(outer_tc,0,&err);	

	/* Creating a new dynamicData instance based on the outer type code */
    outer_data = DDS_DynamicData_new(outer_tc, 
        &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if(outer_data == NULL) {
        fprintf(stderr,"! Unable to create outer dynamicData\n");
        goto fail;
    }

    inner_data = DDS_DynamicData_new(inner_tc, 
        &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if(outer_data == NULL) {
        fprintf(stderr,"! Unable to create inner dynamicData\n");
        goto fail;
    }
	/* Setting the inner data */
	retcode = DDS_DynamicData_set_double(
        inner_data,
        "x",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        3.14159);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to set value 'x' in the inner struct \n");
        goto fail;
    }

	retcode = DDS_DynamicData_set_double(
        inner_data,
        "y",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        2.71828);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to set value 'y' in the inner struct\n");
        goto fail;
    }

	printf("\n\n get/set_complex_member API\n"
	           "----------------------------\n");
	/* Get/Set complex member API */
	printf("Setting the initial values of struct with set_complex_member()\n");
	retcode = DDS_DynamicData_set_complex_member(
        outer_data,
        "inner",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        inner_data);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to set complex struct value "
            "(member inner in the outer struct)\n");
        goto fail;
    }

	DDS_DynamicData_print(outer_data,stdout,1);
	
	retcode = DDS_DynamicData_clear_all_members(inner_data);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to clear all member in the inner data\n");
        goto fail;
    }

	printf("\n + get_complex_member() called\n");
	retcode = DDS_DynamicData_get_complex_member(
        outer_data,
        inner_data,
        "inner",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
	if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to get complex struct value "
            "(member inner in the outer struct)\n");
        goto fail;
    }

	printf("\n + inner struct value\n");
	DDS_DynamicData_print(inner_data,stdout,1);
    
	/* get complex member makes a copy of the member, so modifying the dynamic 
	data obtained by get complex member WILL NOT modify the outer data */	
	printf("\n + setting new values to inner struct\n");
	retcode = DDS_DynamicData_set_double(
        inner_data,
        "x",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        1.00000);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to set value 'x' in the inner struct \n");
        goto fail;
    }

	retcode= DDS_DynamicData_set_double(
        inner_data,
        "y",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        0.00001);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to set value 'y' in the inner struct \n");
        goto fail;
    }
	/* Current value of outer_data
	outer: 
          inner:
	    x: 3.141590
	    y: 2.718280
	*/
	printf("\n + current outer struct value \n");
	DDS_DynamicData_print(outer_data,stdout,1);
    
	printf("\n\n bind/unbind API\n"
	       "------------------\n");
	/* Bind/Unbind member API */
    	bounded_data = DDS_DynamicData_new(NULL,
            &DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    if (bounded_data == NULL) {
        fprintf(stderr,"! Unable to create new dynamic data\n");
        goto fail;
    }

	printf("\n + bind complex member called\n");
	
    retcode = DDS_DynamicData_bind_complex_member(
        outer_data,
        bounded_data,
        "inner",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
	if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to bind the structs\n");
        goto fail;
    }

	DDS_DynamicData_print(bounded_data,stdout,1);
    
	/* binding a member does not copy, so modifying the bounded member WILL 
       modify the outer object */ 
	printf("\n + setting new values to inner struct\n");
	retcode = DDS_DynamicData_set_double(
        bounded_data,
        "x",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        1.00000);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to set value to 'x' with the bounded data\n");
        goto fail;
    }

	retcode = DDS_DynamicData_set_double(
        bounded_data,
        "y",
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        0.00001);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to set value to 'x' with the bounded data\n");
        goto fail;
    }

	/* Current value of outer data
	outer: 
          inner:
	    x: 1.000000
	    y: 0.000010
	*/
	
	DDS_DynamicData_print(bounded_data,stdout,1);
    retcode = DDS_DynamicData_unbind_complex_member(outer_data,bounded_data);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,"! Unable to unbind the data\n");
        goto fail;
    }

	printf("\n + current outer struct value \n");
	DDS_DynamicData_print(outer_data,stdout,1);
    
    ret = 1;

fail:
    if (inner_data != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, inner_tc, NULL);
        DDS_DynamicData_delete(inner_data);
    }
    
    if (outer_data != NULL) {
        DDS_TypeCodeFactory_delete_tc(factory, outer_tc, NULL);
        DDS_DynamicData_delete(outer_data);
    }

    if (bounded_data != NULL) {
        DDS_DynamicData_delete(bounded_data);
    }

    return ret;
}
