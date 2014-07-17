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
import com.rti.dds.dynamicdata.DynamicDataMemberInfo;
import com.rti.dds.typecode.*;

public class UnionExample {

    static TypeCode createTypecode() {
        /* First, we create the typeCode for a union */
        /*
         * Default-member index refers to which member of the union will be the
         * default one. In this example index = 1 refers to the the member
         * 'aLong'. Take into account that index starts in 0
         */

        UnionMember members[] = new UnionMember[0];
        TypeCode unionTC;

        unionTC = TypeCodeFactory.TheTypeCodeFactory.create_union_tc("Foo",
                TypeCode.TC_LONG, 1, // default index
                members); // For now, it does not have any member

        /* Case 1 will be a short named aShort */
        try {
            unionTC.add_member("aShort", TypeCode.MEMBER_ID_INVALID,
                    TypeCode.TC_SHORT, TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        /* Case 2 will be a long named aLong */
        try {
            unionTC.add_member("aLong", 2, TypeCode.TC_LONG,
                    TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        /* Case 3 will be a double named aDouble */
        try {
            unionTC.add_member("aDouble", 3, TypeCode.TC_DOUBLE,
                    TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return null;
        }

        return unionTC;

    }

    public static void main(String[] args) {
        DynamicDataMemberInfo info = new DynamicDataMemberInfo();
        /* Creating a new dynamicData instance based on the union type code */
        DynamicData data = new DynamicData(createTypecode(),
                DynamicData.PROPERTY_DEFAULT);

        /*
         * If we get the current discriminator, it will be the default one (not
         * the first added)
         */
        try {
            data.get_member_info_by_index(info, 0);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }

        System.out.println("The member selected is " + info.member_name);

        /*
         * When we set a value in one of the members of the union, the
         * discriminator updates automatically to point that member.
         */
        try {
            data.set_short("aShort", DynamicData.MEMBER_ID_UNSPECIFIED,
                    (short) 42);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }

        /* The discriminator should now reflect the new situation */
        try {
            data.get_member_info_by_index(info, 0);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }

        System.out.println("The member selected is " + info.member_name);

        /* Getting the value of the target member */
        try {
            short testValue = data.get_short("aShort",
                    DynamicData.MEMBER_ID_UNSPECIFIED);

            System.out.println("The member selected is " + info.member_name
                    + " with value: " + testValue);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }

    }
}
