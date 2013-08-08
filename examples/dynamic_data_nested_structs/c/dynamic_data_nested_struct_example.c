#include <ndds_c.h>

DDS_TypeCode *
inner_struct_get_typecode() {
	static DDS_TypeCode *tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	struct DDS_StructMemberSeq members = DDS_SEQUENCE_INITIALIZER;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {

		tcf = DDS_TypeCodeFactory_get_instance();

		tc = DDS_TypeCodeFactory_create_struct_tc(tcf, "InnerStruct", &members,
				&err);

		DDS_TypeCode_add_member(tc, "x", DDS_TYPECODE_MEMBER_ID_INVALID,
				DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
				DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, &err);

		DDS_TypeCode_add_member(tc, "y", DDS_TYPECODE_MEMBER_ID_INVALID,
				DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
				DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, &err);

	}

	return tc;
}

DDS_TypeCode *
outer_struct_get_typecode() {
	static DDS_TypeCode *tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	struct DDS_StructMemberSeq members= DDS_SEQUENCE_INITIALIZER;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory_get_instance();

		tc = DDS_TypeCodeFactory_create_struct_tc(tcf,"OuterStruct",
				&members,&err);

		DDS_TypeCode_add_member(tc,"inner",DDS_TYPECODE_MEMBER_ID_INVALID,
						inner_struct_get_typecode(),
						DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, &err);

	}

	return tc;
}


int main() {
struct DDS_TypeCode *inner_tc = inner_struct_get_typecode();
struct DDS_TypeCode *outter_tc = outer_struct_get_typecode();
DDS_ExceptionCode_t err;
DDS_DynamicData *outter_data = NULL;
DDS_DynamicData *inner_data = NULL;
DDS_DynamicData *bounded_data = NULL;

	printf(" Connext Dynamic Data Nested Struct Example \n"
	       "--------------------------------------------\n");


	printf(" Data Types\n"
	       "------------------\n");
	DDS_TypeCode_print_IDL(inner_tc,0,&err);
	DDS_TypeCode_print_IDL(outter_tc,0,&err);	

	outter_data = DDS_DynamicData_new(outter_tc,&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
	inner_data = DDS_DynamicData_new(inner_tc,&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	/* Setting the inner data */
	DDS_DynamicData_set_double(inner_data,"x",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,3.14159);
	DDS_DynamicData_set_double(inner_data,"y",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,2.71828);

	printf("\n\n get/set_complex_member API\n"
	       "------------------\n");
	/* Get/Set complex member API */
	printf("Setting the initial values of struct with set_complex_member()\n");
	DDS_DynamicData_set_complex_member(outter_data,"inner",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,inner_data);

	DDS_DynamicData_print(outter_data,stdout,1);
	
	DDS_DynamicData_clear_all_members(inner_data);

	printf("\n + get_complex_member() called\n");
	DDS_DynamicData_get_complex_member(outter_data,inner_data,"inner",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
	
	printf("\n + inner struct value\n");
	DDS_DynamicData_print(inner_data,stdout,1);

	/* get complex member makes a copy of the member, so modifying the dynamic 
	data obtained by get complex member WILL NOT modify the outter data */	
	printf("\n + setting new values to inner struct\n");
	DDS_DynamicData_set_double(inner_data,"x",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,1.00000);
	DDS_DynamicData_set_double(inner_data,"y",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,0.00001);

	/* Current value of outter_data
	outter: 
          inner:
	    x: 3.141590
	    y: 2.718280
	*/
	printf("\n + current outter struct value \n");
	DDS_DynamicData_print(outter_data,stdout,1);

	printf("\n\n bind/unbind API\n"
	       "------------------\n");
	/* Bind/Unbind member API */

	bounded_data = DDS_DynamicData_new(NULL,&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	printf("\n + bind complex member called\n");
	DDS_DynamicData_bind_complex_member(outter_data,bounded_data,"inner",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
	
	DDS_DynamicData_print(bounded_data,stdout,1);

	/* binding a member does not copy, so modifying the bounded member WILL modify the outer object */ 
	printf("\n + setting new values to inner struct\n");
	DDS_DynamicData_set_double(bounded_data,"x",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,1.00000);
	DDS_DynamicData_set_double(bounded_data,"y",DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,0.00001);

	/* Current value of outer data
	outter: 
          inner:
	    x: 1.000000
	    y: 0.000010
	*/
	
	DDS_DynamicData_print(bounded_data,stdout,1);

	DDS_DynamicData_unbind_complex_member(outter_data,bounded_data);

	printf("\n + current outter struct value \n");
	DDS_DynamicData_print(outter_data,stdout,1);

	return 0;
}
