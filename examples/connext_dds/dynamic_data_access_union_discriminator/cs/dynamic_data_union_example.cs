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
/* dynamic_data_union_example.cs

 This example
 - creates the type code of for a union
 - creates a DynamicData instance
 - sets one of the members of the union
 - shows how to retrieve the discriminator
 - access to the value of one member of the union

 */
public class unionExample {
    private static DDS.TypeCode create_type_code(DDS.TypeCodeFactory tcf) {
        DDS.UnionMemberSeq members = new DDS.UnionMemberSeq();

        /* First, we create the typeCode for a union */
        /* Default-member index refers to which member of the union
         * will be the default one. In this example index = 1 refers
         * to the the member 'aLong'. Take into account that index
         * starts in 0 */
        try {
            DDS.TypeCode unionTC = tcf.create_union_tc("Foo",
                DDS.ExtensibilityKind.MUTABLE_EXTENSIBILITY,
                DDS.TypeCode.TC_LONG,
                1, /* default-member index */
                members); /* For now, it does not have any member */
                        
            /* Case 1 will be a short named aShort */
            unionTC.add_member("aShort", DDS.DynamicData.MEMBER_ID_UNSPECIFIED,
                    DDS.TypeCode.TC_SHORT, DDS.TypeCode.NONKEY_MEMBER);
            
            /* Case 2, the default, will be a long named aLong */
            unionTC.add_member("aLong", 2, DDS.TypeCode.TC_LONG,
                    DDS.TypeCode.NONKEY_MEMBER);

            /* Case 3 will be a double named aDouble */
            unionTC.add_member("aDouble", 3, DDS.TypeCode.TC_DOUBLE,
                    DDS.TypeCode.NONKEY_MEMBER);

            return unionTC;
        } catch (Exception e) {
            Console.WriteLine("register_type error {0}", e);
            return null;
        }

    }

    public static void Main(string[] args) {
        try {
            /* Creating the union typeCode */
            DDS.TypeCodeFactory tcf = DDS.TypeCodeFactory.get_instance();
            DDS.TypeCode unionTC = create_type_code(tcf);
            DDS.DynamicDataMemberInfo info = new DDS.DynamicDataMemberInfo();
            DDS.DynamicDataProperty_t myProperty =
                    DDS.DynamicData.DYNAMIC_DATA_PROPERTY_DEFAULT;
            short valueTestShort = 0;

            /* Creating a new dynamicData instance based on the union 
             * type code */
            DDS.DynamicData data = new DDS.DynamicData(unionTC, myProperty);
        
            /* If we get the current discriminator, it will be the default one
             * (not the first added) */
            data.get_member_info_by_index(info, 0);
        
            Console.WriteLine("The member selected is {0}", info.member_name);

            /* When we set a value in one of the members of the union,
             * the discriminator updates automatically to point that member.  */
            data.set_short("aShort", DDS.DynamicData.MEMBER_ID_UNSPECIFIED, 42);

            /* The discriminator should now reflect the new situation */
            data.get_member_info_by_index(info, 0);
        
            Console.WriteLine("The member selected is {0}", info.member_name);

            /* Getting the value of the target member */
            valueTestShort = data.get_short("aShort", 
                DDS.DynamicData.MEMBER_ID_UNSPECIFIED);
        
            Console.WriteLine("The member selected is {0} with value: {1}", 
                info.member_name, valueTestShort);

          return;
        } catch (Exception e) {
        Console.WriteLine("register_type error {0}", e);
            return;
        }
    }
}