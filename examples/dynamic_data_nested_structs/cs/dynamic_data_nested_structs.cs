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
using System;
using System.Collections.Generic;
using System.Text;
/* dynamic_data_nested_struct_example.cs

 This example
 - creates the type code of for a nested struct
 (with inner and outer structs)
 - creates a DynamicData instance
 - sets the values of the inner struct
 - shows the differences between set_complex_member and bind_complex_member
 */

/********* IDL representation for this example ************
 
 struct InnerStruct {
     double x;
     double y;
 };

 struct OuterStruct {
    InnerStruct inner;
 };

 */

public class nestedStructExample {
    private static DDS.TypeCode inner_struct_get_type_code(
        DDS.TypeCodeFactory tcf) {
        DDS.StructMemberSeq members = new DDS.StructMemberSeq();

        /* First, we create the typeCode for a struct */
        try {
            DDS.TypeCode tc = tcf.create_struct_tc("InnerStruct", members);

            /* Member 1 will be a double named x */
            tc.add_member("x", DDS.TypeCode.MEMBER_ID_INVALID,
                    DDS.TypeCode.TC_DOUBLE, DDS.TypeCode.NONKEY_MEMBER);

            /* Member 2 will be a double named y */
            tc.add_member("y", DDS.TypeCode.MEMBER_ID_INVALID,
                DDS.TypeCode.TC_DOUBLE, DDS.TypeCode.NONKEY_MEMBER);
           
            return tc;
        } catch (Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return null;
        }

    }

    private static DDS.TypeCode outer_struct_get_type_code(
        DDS.TypeCodeFactory tcf ) {
        DDS.StructMemberSeq members = new DDS.StructMemberSeq();

        try {
            /* First, we create the typeCode for a struct */
            DDS.TypeCode tc = tcf.create_struct_tc("OuterStruct", members);

            DDS.TypeCode inner_tc = inner_struct_get_type_code(tcf);

            /* Member 1 of outer struct will be a struct of type inner_struct 
             * called inner*/
            tc.add_member("inner", DDS.TypeCode.MEMBER_ID_INVALID,
                    inner_tc, DDS.TypeCode.NONKEY_MEMBER);

            return tc;
        } catch (Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return null;
        }

    }

    public static void Main(string[] args) {
        try {
            /* Creating the union typeCode */
            DDS.TypeCodeFactory tcf = DDS.TypeCodeFactory.get_instance();

            /* Creating the typeCode of the inner_struct */
            DDS.TypeCode inner_tc = inner_struct_get_type_code(tcf);

            /* Creating the typeCode of the outer_struct that contains 
             * an inner_struct */
            DDS.TypeCode outer_tc = outer_struct_get_type_code(tcf);

            /* Now, we create a dynamicData instance for each type */
            DDS.DynamicData outer_data = new DDS.DynamicData(
                outer_tc, DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);
            DDS.DynamicData inner_data = new DDS.DynamicData(
                inner_tc, DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);
            DDS.DynamicData bounded_data = new DDS.DynamicData(
                null, DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT);

            Console.WriteLine(" Connext Dynamic Data Nested Struct Example");
            Console.WriteLine("--------------------------------------------");
            Console.WriteLine(" Data Types");
            Console.WriteLine("------------");

            inner_tc.print_IDL(0);
            outer_tc.print_IDL(0);

            /* Setting the inner data */
            inner_data.set_double("x", DDS.DynamicData.MEMBER_ID_UNSPECIFIED,
                3.14159);
            inner_data.set_double("y", DDS.DynamicData.MEMBER_ID_UNSPECIFIED,
                2.71828);

            Console.WriteLine("\n\n get/set_complex_member API");
            Console.WriteLine("----------------------------");

            /* Using set_complex_member, we copy inner_data values in
             * inner_struct of outer_data */
            Console.WriteLine("Setting the initial values of struct with " +
                "set_complex_member()");

            outer_data.set_complex_member("inner",
                DDS.DynamicData.MEMBER_ID_UNSPECIFIED, inner_data);
            outer_data.print(1);
            inner_data.clear_all_members();

            Console.WriteLine("\n + get_complex_member() called");
            outer_data.get_complex_member(inner_data, "inner",
                DDS.DynamicData.MEMBER_ID_UNSPECIFIED);

            Console.WriteLine("\n + inner_data value");
            inner_data.print(1);

            /* get_complex_member made a copy of the inner_struct. If we modify
             * inner_data values, outer_data inner_struct WILL NOT be modified.
             */

            Console.WriteLine("\n + setting new values to inner_data");
            inner_data.set_double("x", DDS.DynamicData.MEMBER_ID_UNSPECIFIED,
                1.00000);
            inner_data.set_double("y", DDS.DynamicData.MEMBER_ID_UNSPECIFIED,
                0.00001);

            /* Current value of outer_data
             * outer:
             * inner:
             *     x: 3.141590
             *     y: 2.718280
             * inner_data:
             *     x: 1.000000
             *     y: 0.000010
             */

            Console.WriteLine("\n + current outer_data value ");
            outer_data.print(1);

            /* Bind/Unbind member API */
            Console.WriteLine("\n\nbind/unbind API");
            Console.WriteLine("-----------------");

            /* Using bind_complex_member, we do not copy inner_struct, but bind 
             * it. So, if we modify bounded_data, the inner member inside 
             * outer_data WILL also be modified */
            Console.WriteLine("\n + bind complex member called");
            outer_data.bind_complex_member(bounded_data, "inner",
                DDS.DynamicData.MEMBER_ID_UNSPECIFIED);

            bounded_data.print(1);
            Console.WriteLine("\n + setting new values to bounded_data");
            
            bounded_data.set_double("x", DDS.DynamicData.MEMBER_ID_UNSPECIFIED,
                1.00000);
            bounded_data.set_double("y", DDS.DynamicData.MEMBER_ID_UNSPECIFIED,
                0.00001);

            /* Current value of outer data
             * outer:
             * inner:
             *     x: 1.000000
             *     y: 0.000010
             */

            bounded_data.print(1);

            outer_data.unbind_complex_member(bounded_data);
            Console.WriteLine("\n + current outer_data value ");
            outer_data.print(1);
            
            return;

        } catch (Exception e) {
        Console.WriteLine("register_type error {0}", e);
            return;
        }
    }
}