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
/*
 * Uncomment to use the bind api instead of the get API.
 */
//#define USE_BIND_API 

using System;
using System.Collections.Generic;
using System.Text;
/* dynamic_data_sequences.cs

  This example
  - creates the type code of a sequence
  - creates a DynamicData instance
  - writes and reads the values

 */

/********* IDL representation for this example ************
 *
 * struct SimpleStruct {
 *     long a_member;
 * };
 *
 * struct TypeWithSequence {
 * 	   sequence<SimpleStruct,5> sequence_member;
 * };
 */


public class sequencesExample {
    private static uint MAX_SEQ_LEN = 5;
    private const String sequence_member_name = "sequence_member";
    private static DDS.TypeCode sequence_element_get_typecode(
        DDS.TypeCodeFactory tcf) {
        DDS.StructMemberSeq members = new DDS.StructMemberSeq();

        try {
            /* First, we create the typeCode for the simple struct */
            DDS.TypeCode tc = tcf.create_struct_tc("SimpleStruct", members);

            tc.add_member("a_member", DDS.TypeCode.MEMBER_ID_INVALID,
                    DDS.TypeCode.TC_LONG, DDS.TypeCode.NONKEY_MEMBER);

            return tc;
        } catch (DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return null;
        }

    }

    private static DDS.TypeCode sequence_get_typecode(
        DDS.TypeCodeFactory tcf ) {
       
        try {
            DDS.TypeCode seq_element_tc = sequence_element_get_typecode(tcf);
            if (seq_element_tc == null) {
                Console.WriteLine("Error to create " +
                    "sequence_element_get_typecode");
                return null;
            }
            /* We create the typeCode for the sequence */
            DDS.TypeCode tc = tcf.create_sequence_tc(MAX_SEQ_LEN, 
                    seq_element_tc);

            return tc;
        } catch (DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return null;
        }

    }

    private static DDS.TypeCode type_w_sequence_get_typecode(
    DDS.TypeCodeFactory tcf ) {
        DDS.StructMemberSeq members = new DDS.StructMemberSeq();

        try {
            DDS.TypeCode sequence_tc = sequence_get_typecode(tcf);
            if (sequence_tc == null) {
                Console.WriteLine("Error to create sequence_get_typecode");
                return null;
            }
            /* We create the typeCode for struct which contains the sequence */
            DDS.TypeCode tc = tcf.create_struct_tc("TypeWithSequence",
                members);

            tc.add_member(sequence_member_name, DDS.TypeCode.MEMBER_ID_INVALID,
                sequence_tc, DDS.TypeCode.NONKEY_MEMBER);

            return tc;
        } catch (DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return null;
        }

    }

    private static void write_data (DDS.DynamicData sample, 
        DDS.TypeCodeFactory tcf) {
        /* Creating typecodes */
        DDS.TypeCode sequence_tc = sequence_get_typecode(tcf);
        if (sequence_tc == null) {
            Console.WriteLine("Error to create sequence_get_typecode in " +
                "writing_data");
            return;
        }

        DDS.TypeCode sequence_element_tc = sequence_element_get_typecode(tcf);
        if (sequence_element_tc == null) {
            Console.WriteLine("Error to create sequence_element_get_typecode " +
                "in writing_data");
            return;
        }

        try {
            /* Creating DynamicData */
            DDS.DynamicData seq_member = new DDS.DynamicData(sequence_tc,
                DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);
            DDS.DynamicData seq_element = 
                new DDS.DynamicData(sequence_element_tc,
                DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);

            for (int i = 0; i < MAX_SEQ_LEN; ++i) {
                /* To access the elements of a sequence it is necessary
                 * to use their id. This parameter allows accessing to every 
                 * element of the sequence using a 1-based index.
                 * There are two ways of doing this: bind API and get API.
                 * See the NestedStructExample for further details about the 
                 * differences between these two APIs. */

#if (USE_BIND_API)
                seq_member.bind_complex_member(seq_element, null, i + 1);
                seq_element.set_int("a_member", 
                    DDS.DynamicData.MEMBER_ID_UNSPECIFIED, i);
                Console.WriteLine("Writing sequence elemente #{0} : ", i + 1);
                seq_element.print(1);
                seq_member.unbind_complex_member(seq_element);
#else
                seq_element.set_int("a_member",
                    DDS.DynamicData.MEMBER_ID_UNSPECIFIED, i);
                Console.WriteLine("Writing sequence element #{0}", i + 1);
                seq_element.print(1);
                seq_member.set_complex_member(null, i + 1, seq_element);
#endif
            }

            sample.set_complex_member(sequence_member_name,
                DDS.DynamicData.MEMBER_ID_UNSPECIFIED, seq_member);

            /* Delete the created TC */
            if (sequence_element_tc != null) {
                tcf.delete_tc(sequence_element_tc);
            }

            if (sequence_tc != null) {
                tcf.delete_tc(sequence_tc);
            }
            
            return;
        } catch (DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return;
        }
    }

    private static void read_data(DDS.DynamicData sample, 
        DDS.TypeCodeFactory tcf ) {
       
        /* Creating typecodes */
        DDS.TypeCode sequence_tc = sequence_get_typecode(tcf);
        if (sequence_tc == null) {
            Console.WriteLine("Error to create sequence_get_typecode in " + 
                "reading_data");
            return;
        }

        /* Creating DynamicData */
        DDS.DynamicData seq_member = new DDS.DynamicData(sequence_tc,
            DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);
        DDS.DynamicData seq_element = new DDS.DynamicData(null,
            DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);
        
        DDS.DynamicDataInfo info = new DDS.DynamicDataInfo();

        sample.get_complex_member(seq_member, sequence_member_name, 
            DDS.DynamicData.MEMBER_ID_UNSPECIFIED);
            
        /* Now we get the total amount of elements contained in the array
         * by accessing the dynamic data info
         */
        Console.WriteLine("* Getting sequence member info....");
        seq_member.get_info(info);
        Console.WriteLine("* Sequence contains {0} elements", 
            info.member_count);

        for (int i = 0; i < info.member_count; ++i) {
            /*
             * The same results can be obtained using
             * bind_complex_member method. The main difference, is that
             * in that case the members are not copied, just referenced
             */

#if (USE_BIND_API)
             try {
                 seq_member.bind_complex_member(seq_element, null, i + 1);
             } catch (DDS.Exception e) {
                 Console.WriteLine("register_type error {0}", e);
                 return;
             }

#else
            try {
                seq_member.get_complex_member(seq_element, null, i + 1);
            } catch (DDS.Exception e) {
                Console.WriteLine("register_type error {0}", e);
                return;
            }
            
#endif
            long value = seq_element.get_long("a_member", 
                DDS.DynamicData.MEMBER_ID_UNSPECIFIED);
            Console.WriteLine("Reading sequence element #{0} : ",
                i + 1);
            seq_element.print(1);

#if (USE_BIND_API)
            try {
                seq_member.unbind_complex_member(seq_element);
            } catch (DDS.Exception e) {
                Console.WriteLine("register_type error {0}", e);
                return;
            }
#endif
        }

        /* Delete the created TC */
        if (sequence_tc != null) {
            tcf.delete_tc(sequence_tc);
        }

        return;
    }

    public static void Main(string[] args) {
       DDS.TypeCodeFactory tcf = DDS.TypeCodeFactory.get_instance();

        DDS.TypeCode w_sequence_tc = type_w_sequence_get_typecode(tcf);
        if (w_sequence_tc == null) {
            Console.WriteLine("Error to create type_w_sequence_get_typecode");
            return;
        }
        try {
            DDS.DynamicData sample = new DDS.DynamicData(w_sequence_tc,
                DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);

            Console.WriteLine("***** Writing a sample *****");
            write_data(sample, tcf);

            Console.WriteLine("***** Reading a sample *****");
            read_data(sample, tcf);

            /* Delete the created TC */
            if (w_sequence_tc != null) {
                tcf.delete_tc(w_sequence_tc);
            }
        } catch (DDS.Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return;
        }
    }
}