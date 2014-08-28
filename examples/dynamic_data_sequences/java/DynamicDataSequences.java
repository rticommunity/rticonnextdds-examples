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
import com.rti.dds.dynamicdata.DynamicData;
import com.rti.dds.dynamicdata.DynamicDataInfo;
import com.rti.dds.dynamicdata.DynamicDataMemberInfo;
import com.rti.dds.typecode.*;

/*********
 * IDL representation for this example ************
 * 
 * public class SimpleStruct { 
 *     long a_member;
 * };
 * 
 * public class TypeWithSequence { 
 *     sequence<SimpleStruct,5> sequence_member;
 * };
 */

public class DynamicDataSequences {
    public static int MAX_SEQ_LEN = 5;
    /*
     * Modify bin_api to true to use the bind api instead of the get API.
     */
    public static boolean BIN_API = false;

    static TypeCode sequenceElementGetTypeCode() {
        StructMember members[] = new StructMember[0];
        TypeCode tc;

        /* First, we create the typeCode for the simple struct */
        try {
            tc = TypeCodeFactory.TheTypeCodeFactory.create_struct_tc(
                    "SimpleStruct", members);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        try {
            tc.add_member("a_member", TypeCode.MEMBER_ID_INVALID,
                    TypeCode.TC_LONG,
                    TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        return tc;

    }

    static TypeCode sequenceGetTypeCode() {
        TypeCode tc;

        TypeCode seqElementTC = sequenceElementGetTypeCode();
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

        return tc;

    }

    static TypeCode typeWithSequenceGetTypecode() {
        StructMember members[] = new StructMember[0];
        TypeCode sequenceTC = sequenceGetTypeCode();
        if (sequenceTC == null) {
            System.err.println("! Unable to create Type with sequence TC");
            return null;
        }

        TypeCode tc = TypeCodeFactory.TheTypeCodeFactory.create_struct_tc(
                "TypeWithSequence", members);
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

        return tc;

    }

    public static void writeData(DynamicData sample) {
        /* Creating TypeCodes */
        TypeCode sequenceTC = sequenceGetTypeCode();
        if (sequenceTC == null) {
            System.err.println("! Unable to create sequence typecode");
            return;
        }

        TypeCode sequenceElementTC = sequenceElementGetTypeCode();
        if (sequenceElementTC == null) {
            System.err.println("! Unable to create sequence element TypeCode");
            return;
        }

        DynamicData seqMember = new DynamicData(sequenceTC,
                DynamicData.PROPERTY_DEFAULT);
        DynamicData seqElement = new DynamicData(sequenceElementTC,
                DynamicData.PROPERTY_DEFAULT);

        for (int i = 0; i < MAX_SEQ_LEN; ++i) {
            /*
             * To access the elements of a sequence it is necessary to use their
             * id. This parameter allows accessing to every element of the
             * sequence using a 1-based index. There are two ways of doing this:
             * bind API and get API. See the NestedStructExample for further
             * details about the differences between these two APIs.
             */

            if (BIN_API == true) { // bind API
                try {
                    seqMember.bind_complex_member(seqElement, null, i + 1);
                    seqElement.set_int("a_member",
                            DynamicData.MEMBER_ID_UNSPECIFIED, i);
                    System.out.println("Writing sequence element #" + (i + 1)
                            + " : ");
                    seqElement.print(null, 1);
                    seqMember.unbind_complex_member(seqElement);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            } else { // get API
                try {
                    seqElement.set_int("a_member",
                            DynamicData.MEMBER_ID_UNSPECIFIED, i);
                    System.out.println("Writing sequence element #" + (i + 1)
                            + " : ");
                    seqElement.print(null, 1);
                    seqMember.set_complex_member(null, i + 1, seqElement);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }

        }

        try {
            sample.set_complex_member("sequence_member",
                    DynamicData.MEMBER_ID_UNSPECIFIED, seqMember);
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

    public static void readData(DynamicData sample) {
        TypeCode sequenceTC = sequenceGetTypeCode();
        if (sequenceTC == null) {
            System.err.println("! Unable to get sequence Typecode");
            return;
        }

        DynamicData seqMember = new DynamicData(sequenceTC,
                DynamicData.PROPERTY_DEFAULT);
        DynamicData seqElement = new DynamicData(null,
                DynamicData.PROPERTY_DEFAULT);

        int seqLen = 0;
        DynamicDataInfo info = new DynamicDataInfo();

        sample.get_complex_member(seqMember, "sequence_member",
                DynamicData.MEMBER_ID_UNSPECIFIED);

        /*
         * Now we get the total amount of elements contained in the array by
         * accessing the dynamic data info
         */
        System.out.println("* Getting sequence member info....");
        seqMember.get_info(info);

        seqLen = info.member_count;
        System.out.println("* Sequence contains " + seqLen + " elements");

        for (int i = 0; i < seqLen; ++i) {
            /*
             * The same results can be obtained using bind_complex_member
             * method. The main difference, is that in that case the members are
             * not copied, just referenced
             */
            if (BIN_API) { // using bind API
                try {
                    seqMember.bind_complex_member(seqElement, null, i + 1);
                } catch (Exception e) {
                    System.out.println(e.getMessage());
                }
            } else { // using get API
                try {
                    seqMember.get_complex_member(seqElement, null, i + 1);
                } catch (Exception e) {
                    System.err.println(e.getMessage());
                }
            }

            System.out.println("Reading sequence element #" + (i + 1) + " :");
            seqElement.print(null, 1);

            if (BIN_API) {
                try {
                    seqMember.unbind_complex_member(seqElement);
                } catch (Exception e) {
                    System.out.println(e.getMessage());
                }
            }
        }

    }

    public static void main(String[] args) {

        TypeCode wSequenceTC = typeWithSequenceGetTypecode();
        if (wSequenceTC == null) {
            System.err.println("! Unable to create wSequence TypeCode");
            return;
        }

        DynamicData sample = new DynamicData(wSequenceTC,
                DynamicData.PROPERTY_DEFAULT);
        System.out.println("***** Writing a sample *****");
        writeData(sample);

        System.out.println("\n\n***** Reading a sample *****");
        readData(sample);

        /* Delete the created TC */
        if (wSequenceTC != null) {
            TypeCodeFactory.TheTypeCodeFactory.delete_tc(wSequenceTC);
        }
    }
}
