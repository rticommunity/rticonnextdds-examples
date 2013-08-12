#include <ndds_cpp.h>

DDS_TypeCode *
inner_struct_get_typecode() {
	static DDS_TypeCode *tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	struct DDS_StructMemberSeq members;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {

		tcf = DDS_TypeCodeFactory_get_instance();

		tc = tcf->create_struct_tc("InnerStruct", members, err);

		tc->add_member("x", DDS_TYPECODE_MEMBER_ID_INVALID,
				DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
				DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, err);

		tc->add_member("y", DDS_TYPECODE_MEMBER_ID_INVALID,
				DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_DOUBLE),
				DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, err);

	}

	return tc;
}

DDS_TypeCode *
outer_struct_get_typecode() {
	static DDS_TypeCode *tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	struct DDS_StructMemberSeq members;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory_get_instance();

		tc = tcf->create_struct_tc("OuterStruct", members, err);

		tc->add_member("inner", DDS_TYPECODE_MEMBER_ID_INVALID,
				inner_struct_get_typecode(),
				DDS_TYPECODE_NONKEY_REQUIRED_MEMBER, err);

	}

	return tc;
}

int main() {
	struct DDS_TypeCode *inner_tc = inner_struct_get_typecode();
	struct DDS_TypeCode *outter_tc = outer_struct_get_typecode();
	DDS_ExceptionCode_t err;

	DDS_DynamicData outer_data(outter_tc, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
	DDS_DynamicData inner_data(inner_tc, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
	DDS_DynamicData bounded_data(NULL,DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	printf(" Connext Dynamic Data Nested Struct Example \n"
			"--------------------------------------------\n");

	printf(" Data Types\n"
			"------------------\n");
	inner_tc->print_IDL(0, err);
	outter_tc->print_IDL(0, err);

	/* Setting the inner data */
	inner_data.set_double("x", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 3.14159);
	inner_data.set_double("y", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 2.71828);

	printf("\n\n get/set_complex_member API\n"
			"------------------\n");
	/* Get/Set complex member API */
	printf("Setting the initial values of struct with set_complex_member()\n");
	outer_data.set_complex_member("inner",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, inner_data);

	outer_data.print(stdout, 1);

	inner_data.clear_all_members();

	printf("\n + get_complex_member() called\n");
	outer_data.get_complex_member(inner_data, "inner",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

	printf("\n + inner struct value\n");
	inner_data.print(stdout, 1);

	/* get complex member makes a copy of the member, so modifying the dynamic 
	 data obtained by get complex member WILL NOT modify the outter data */
	printf("\n + setting new values to inner struct\n");
	inner_data.set_double("x", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 1.00000);
	inner_data.set_double("y", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 0.00001);

	/* Current value of outter_data
	 outter:
	 inner:
	 x: 3.141590
	 y: 2.718280
	 */
	printf("\n + current outter struct value \n");
	outer_data.print(stdout, 1);

	printf("\n\n bind/unbind API\n"
			"------------------\n");
	/* Bind/Unbind member API */

	printf("\n + bind complex member called\n");
	outer_data.bind_complex_member(bounded_data, "inner",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

	bounded_data.print(stdout, 1);

	/* binding a member does not copy, so modifying the bounded member WILL modify the outer object */
	printf("\n + setting new values to inner struct\n");
	bounded_data.set_double("x",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 1.00000);
	bounded_data.set_double("y",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 0.00001);

	/* Current value of outer data
	 outter:
	 inner:
	 x: 1.000000
	 y: 0.000010
	 */

	bounded_data.print(stdout, 1);

	outer_data.unbind_complex_member(bounded_data);

	printf("\n + current outter struct value \n");
	outer_data.print(stdout, 1);

	return 0;
}
