import com.rti.dds.dynamicdata.DynamicData;
import com.rti.dds.dynamicdata.DynamicDataInfo;
import com.rti.dds.dynamicdata.DynamicDataMemberInfo;
import com.rti.dds.typecode.*;

/*********
 * IDL representation for this example ************
 * 
 * public class SimpleStruct { long a_member; };
 * 
 * public class TypeWithSequence { sequence<256,SimpleStruct> sequence_member;
 * };
 */

/*
 * Uncomment to use the bind api instead of the get API.
 */
// Constants.bin_api = true;

public class DynamicDataSequences {
    public static int MAX_SEQ_LEN = 5;
    public static boolean bin_api = false;

    /*
     * Uncomment to use the bind api instead of the get API.
     */
    // bin_api = true;

    static TypeCode sequence_element_get_typecode() {
        StructMember members[] = new StructMember[0];
        TypeCode tc;

        /* First, we create the typeCode for the simple struct */
        try {
            tc = TypeCodeFactory.TheTypeCodeFactory.create_struct_tc("Foo",
                    members);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        try {
            tc.add_member("a_member", TypeCode.MEMBER_ID_INVALID,
                    TypeCode.TC_LONG, TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        return tc;

    }

    static TypeCode sequence_get_typecode() {
        TypeCode tc;

        TypeCode seqElementTC = sequence_element_get_typecode();
        if (seqElementTC == null) {
            System.err.println("! Unable to create TypeCode");
            return null;
        }

        /* We create the typeCode for the sequence */
        try {
            tc = TypeCodeFactory.TheTypeCodeFactory.create_sequence_tc(
                    MAX_SEQ_LEN, seqElementTC);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        if (seqElementTC != null) {
            TypeCodeFactory.TheTypeCodeFactory.delete_tc(seqElementTC);
        }

        return tc;

    }

    static TypeCode type_w_sequence_get_typecode() {
        StructMember members[] = new StructMember[0];
        
        TypeCode sequenceTC = sequence_get_typecode();
        if (sequenceTC == null) {
            System.err.println("! Unable to create Type with sequence TC");
            return null;
        }

        TypeCode tc = TypeCodeFactory.TheTypeCodeFactory.create_struct_tc(
                "TypeWithSequence", null);
        if (tc == null) {
            System.err.println("! Unable to create Type with sequence TC");
        }

        try {
            tc.add_member("sequence_member", TypeCode.MEMBER_ID_INVALID,
                    sequenceTC, TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        if (sequenceTC != null) {
            TypeCodeFactory.TheTypeCodeFactory.delete_tc(sequenceTC);
        }

        return tc;

    }

    public static void write_data(DynamicData sample) {
        /* Creating TypeCodes */
        TypeCode sequenceTC = sequence_get_typecode();
        if (sequenceTC == null) {
            System.err.println("! Unable to create sequence typecode");
            return;
        }

        TypeCode sequenceElementTC = sequence_element_get_typecode();
        if (sequenceElementTC == null) {
            System.err.println("! Unable to create sequence element TypeCode");
            return;
        }

        DynamicData seqmember = new DynamicData(sequenceTC,
                DynamicData.PROPERTY_DEFAULT);
        DynamicData seqelement = new DynamicData(sequenceElementTC,
                DynamicData.PROPERTY_DEFAULT);

        for (int i = 0; i < MAX_SEQ_LEN; ++i) {
            /*
             * To access the elements of a sequence it is necessary to use their
             * id. This parameter allows accessing to every element of the
             * sequence using a 1-based index. There are two ways of doing this:
             * bind API and get API. See the NestedStructExample for further
             * details about the differences between these two APIs.
             */

            if (bin_api == true) { // bind API
                try {
                    seqmember.bind_complex_member(seqelement, null, i + 1);
                    seqelement.set_long("a_member",
                            DynamicData.MEMBER_ID_UNSPECIFIED, i);
                    System.out.println("Writing sequence element #" + (i + 1)
                            + " : ");
                    System.out.println(seqelement);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            } else { // get API
                try {
                    seqelement.set_long("a_member",
                            DynamicData.MEMBER_ID_UNSPECIFIED, i);
                    System.out.println("Writing sequence element #" + (i + 1)
                            + " : ");
                    seqmember.set_complex_member(null, i + 1, seqelement);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }

        }

        try {
            sample.set_complex_member("sequence_member",
                    DynamicData.MEMBER_ID_UNSPECIFIED, seqmember);
        } catch (Exception e) {
            System.err.println(e.getMessage());
        }

        if (sequenceTC != null) {
            TypeCodeFactory.TheTypeCodeFactory.delete_tc(sequenceTC);
        }

        if (sequenceElementTC != null) {
            TypeCodeFactory.TheTypeCodeFactory.delete_tc(sequenceElementTC);
        }

        return;

    }

    public static void read_data(DynamicData sample) {
        TypeCode sequenceTC = sequence_get_typecode();
        if (sequenceTC == null) {
            System.err.println("! Unable to get sequence Typecode");
            return;
        }

        DynamicData seqmember = new DynamicData(sequenceTC,
                DynamicData.PROPERTY_DEFAULT);
        DynamicData seqelement = new DynamicData(null,
                DynamicData.PROPERTY_DEFAULT);

        long value = 1;
        int seqlen = 0;
        DynamicDataInfo info = new DynamicDataInfo();

        sample.get_complex_member(seqmember, "sequence_member",
                DynamicData.MEMBER_ID_UNSPECIFIED);

        /*
         * Now we get the total amount of elements contained in the array by
         * accessing the dynamic data info
         */
        System.out.println("* Getting sequence member info....");
        seqmember.get_info(info);

        seqlen = info.member_count;
        System.out.println("* Sequence contains " + seqlen + " elements");

        for (int i = 0; i < seqlen; ++i) {
            /*
             * The same results can be obtained using bind_complex_member
             * method. The main difference, is that in that case the members are
             * not copied, just referenced
             */
            if (bin_api) { // using bind API
                try {
                    seqmember.bind_complex_member(seqelement, null, i + 1);
                } catch (Exception e) {
                    System.out.println(e.getMessage());
                }
            } else { // using get API
                try {
                    seqmember.get_complex_member(seqelement, null, i + 1);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }

            value = seqmember.get_long("a_member",
                    DynamicData.MEMBER_ID_UNSPECIFIED);
            System.out.println("Readeing sequence element #" + (i + 1) + " :");
            System.out.println(seqelement);
            
            if (bin_api) {
                try {
                    seqmember.unbind_complex_member(seqelement);
                } catch (Exception e) {
                    System.out.println(e.getMessage());
                }
            }
        }

    }

    public static void main(String[] args) {

        TypeCode wSequenceTC = type_w_sequence_get_typecode();
        if (wSequenceTC == null) {
            System.err.println("! Unable to create wSequence TypeCode");
            return;
        }

        DynamicData sample = new DynamicData(wSequenceTC,
                DynamicData.PROPERTY_DEFAULT);
        System.out.println("***** Writing a sample *****");
        write_data(sample);

        System.out.println("***** Reading a sample *****");
        read_data(sample);

        /* Delete the created TC */
        if (wSequenceTC != null) {
            TypeCodeFactory.TheTypeCodeFactory.delete_tc(wSequenceTC);
        }
    }
}
