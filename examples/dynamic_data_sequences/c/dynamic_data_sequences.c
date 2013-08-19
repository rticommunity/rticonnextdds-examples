#include <ndds_c.h>

/********* IDL representation for this example ************
 *
 * struct SimpleStruct {
 * 			long a_member;
 * };
 *
 * struct TypeWithSequence {
 * 			sequence<256,SimpleStruct> sequence_member;
 * };
 */

#define MAX_SEQ_LEN 5
/*
 * Uncomment to use the bind api instead of the get API.
*/
/* #define USE_BIND_API */




DDS_TypeCode *
sequence_element_get_typecode() {
	static DDS_TypeCode * tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	struct DDS_StructMemberSeq members = DDS_SEQUENCE_INITIALIZER;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory_get_instance();

		tc = DDS_TypeCodeFactory_create_struct_tc(tcf, "SimpleStruct", &members,
				&err);

		DDS_TypeCode_add_member(tc, "a_member", DDS_TYPECODE_MEMBER_ID_INVALID,
				DDS_TypeCodeFactory_get_primitive_tc(tcf, DDS_TK_LONG),
				DDS_TYPECODE_NONKEY_MEMBER, &err);

	}

	return tc;
}

DDS_TypeCode *
sequence_get_typecode() {
	static DDS_TypeCode * tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory_get_instance();

		tc = DDS_TypeCodeFactory_create_sequence_tc(tcf, MAX_SEQ_LEN,
				sequence_element_get_typecode(), &err);
	}

	return tc;
}

DDS_TypeCode *type_w_sequence_get_typecode() {
	static DDS_TypeCode * tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	struct DDS_StructMemberSeq members = DDS_SEQUENCE_INITIALIZER;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory_get_instance();

		tc = DDS_TypeCodeFactory_create_struct_tc(tcf, "TypeWithSequence",
				&members, &err);

		DDS_TypeCode_add_member(tc, "sequence_member",
				DDS_TYPECODE_MEMBER_ID_INVALID, sequence_get_typecode(),
				DDS_TYPECODE_NONKEY_MEMBER, &err);

	}

	return tc;
}

void write_data(DDS_DynamicData *sample) {
	DDS_DynamicData seqmember;
	DDS_DynamicData seqelement;
	int i = 0;

	DDS_DynamicData_initialize(&seqmember, sequence_get_typecode(),
			&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	DDS_DynamicData_initialize(&seqelement, sequence_element_get_typecode(),
			&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	for (i = 0; i < MAX_SEQ_LEN; ++i) {

		/* To access the elements of a sequence it is necessary
		 * to use a the id parameter. This parameter allows
		 *  accessing to every element of the sequence using a 1-based index.
		 */
#ifdef USE_BIND_API
		DDS_DynamicData_bind_complex_member(&seqmember, &seqelement, NULL,
				i + 1);
		DDS_DynamicData_set_long(&seqelement, "a_member",
				DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, i);
		printf("Writing sequence element #%d : \n", i + 1);
		DDS_DynamicData_print(&seqelement, stdout, 1);
		DDS_DynamicData_unbind_complex_member(&seqmember, &seqelement);
#else
		DDS_DynamicData_set_long(&seqelement, "a_member",
				DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, i);
		printf("Writing sequence element #%d : \n", i + 1);
		DDS_DynamicData_print(&seqelement, stdout, 1);
		DDS_DynamicData_set_complex_member(&seqmember, NULL, i + 1,
				&seqelement);
#endif
	}

	DDS_DynamicData_set_complex_member(sample, "sequence_member",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, &seqmember);

}

void read_data(DDS_DynamicData *sample) {
	DDS_DynamicData seqmember;
	DDS_DynamicData seqelement;
	DDS_Long value;
	int i;

	DDS_DynamicData_initialize(&seqmember, sequence_get_typecode(),
			&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	DDS_DynamicData_initialize(&seqelement, NULL,
			&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	DDS_DynamicData_get_complex_member(sample, &seqmember, "sequence_member",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

	for (i = 0; i < MAX_SEQ_LEN; ++i) {
		/*
		 * The same results can be obtained using
		 * bind_complex_member method. The main difference, is that
		 * in that case the members are not copied, just referenced
		 */
#ifdef USE_BIND_API
		DDS_DynamicData_bind_complex_member(&seqmember, &seqelement, NULL,
				i + 1);
#else
		DDS_DynamicData_get_complex_member(&seqmember, &seqelement, NULL,
				i + 1);
#endif

		DDS_DynamicData_get_long(&seqelement, &value, "a_member",
				DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

		printf("Reading sequence element #%d : \n", i + 1);
		DDS_DynamicData_print(&seqelement, stdout, 1);

#ifdef USE_BIND_API
		DDS_DynamicData_unbind_complex_member(&seqmember, &seqelement);
#endif

	}
}

int main() {
	DDS_DynamicData sample;

	DDS_DynamicData_initialize(&sample, type_w_sequence_get_typecode(),
			&DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	printf("***** Writing a sample *****\n");

	write_data(&sample);

	printf("***** Reading a sample *****\n");
	read_data(&sample);

	return 0;
}
