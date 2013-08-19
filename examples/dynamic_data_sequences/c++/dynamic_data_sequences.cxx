#include <ndds_cpp.h>

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
	struct DDS_StructMemberSeq members;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory::get_instance();

		tc = tcf->create_struct_tc("SimpleStruct", members, err);

		tc->add_member("a_member", DDS_TYPECODE_MEMBER_ID_INVALID,
				tcf->get_primitive_tc(DDS_TK_LONG), DDS_TYPECODE_NONKEY_MEMBER,
				err);

	}

	return tc;
}

DDS_TypeCode *
sequence_get_typecode() {
	static DDS_TypeCode * tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory::get_instance();

		tc = tcf->create_sequence_tc(MAX_SEQ_LEN,
				sequence_element_get_typecode(), err);
	}

	return tc;
}

DDS_TypeCode *type_w_sequence_get_typecode() {
	static DDS_TypeCode * tc = NULL;
	DDS_TypeCodeFactory *tcf = NULL;
	struct DDS_StructMemberSeq members;
	DDS_ExceptionCode_t err;

	if (tc == NULL) {
		tcf = DDS_TypeCodeFactory::get_instance();

		tc = tcf->create_struct_tc("TypeWithSequence", members, err);

		tc->add_member("sequence_member", DDS_TYPECODE_MEMBER_ID_INVALID,
				sequence_get_typecode(), DDS_TYPECODE_NONKEY_MEMBER, err);

	}

	return tc;
}

void write_data(DDS_DynamicData *sample) {
	DDS_DynamicData seqmember(sequence_get_typecode(),
			DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
	DDS_DynamicData seqelement(sequence_element_get_typecode(),
			DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
	int i = 0;

	for (i = 0; i < MAX_SEQ_LEN; ++i) {

		/* To access the elements of a sequence it is necessary
		 * to use a the id parameter. This parameter allows
		 *  accessing to every element of the sequence using a 1-based index.
		 */
#ifdef USE_BIND_API
		seqmember.bind_complex_member(seqelement, NULL,
				i + 1);
		seqelement.set_long( "a_member",
				DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, i);
		printf("Writing sequence element #%d : \n", i + 1);
		seqelement.print(stdout, 1);
		seqmember.unbind_complex_member(seqelement);
#else
		seqelement.set_long("a_member", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
				i);
		printf("Writing sequence element #%d : \n", i + 1);
		seqelement.print(stdout, 1);
		seqmember.set_complex_member(NULL, i + 1, seqelement);
#endif
	}

	DDS_DynamicData_set_complex_member(sample, "sequence_member",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, &seqmember);

}

void read_data(DDS_DynamicData *sample) {
	DDS_DynamicData seqmember(sequence_get_typecode(),
			DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
	DDS_DynamicData seqelement(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
	DDS_Long value;
	struct DDS_DynamicDataInfo info;
	int i, seqlen;

	sample->get_complex_member(seqmember, "sequence_member",
			DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

	/* Now we get the total amount of elements contained in the array
	 * by accessing the dynamic data info
	 */
	printf("* Getting sequence member info....\n");
	seqmember.get_info(info);
	seqlen = info.member_count;
	printf("* Sequence contains %d elements\n", seqlen);

	for (i = 0; i < seqlen; ++i) {
		/*
		 * The same results can be obtained using
		 * bind_complex_member method. The main difference, is that
		 * in that case the members are not copied, just referenced
		 */
#ifdef USE_BIND_API
		seqmember.bind_complex_member(seqelement, NULL,
				i + 1);
#else
		seqmember.get_complex_member(seqelement, NULL, i + 1);
#endif

		seqelement.get_long(value, "a_member",
				DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

		printf("Reading sequence element #%d : \n", i + 1);
		seqelement.print(stdout, 1);

#ifdef USE_BIND_API
		seqmember.unbind_complex_member(seqelement);
#endif

	}
}

int main() {
	DDS_DynamicData sample(type_w_sequence_get_typecode(),
			DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);

	printf("***** Writing a sample *****\n");

	write_data(&sample);

	printf("***** Reading a sample *****\n");
	read_data(&sample);

	return 0;
}
