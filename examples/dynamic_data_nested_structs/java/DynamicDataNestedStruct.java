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
import java.io.File;
import java.io.FileDescriptor;
import com.rti.dds.dynamicdata.DynamicData;
import com.rti.dds.infrastructure.BadKind;
import com.rti.dds.typecode.*;

public class DynamicDataNestedStruct {

    static TypeCode innerStructGetTypeCode() {
        StructMember members[] = new StructMember[0];
        TypeCode tc;

        tc = TypeCodeFactory.TheTypeCodeFactory.create_struct_tc("InnerType",
                members);

        try {
            tc.add_member("x", TypeCode.MEMBER_ID_INVALID, TypeCode.TC_DOUBLE,
                    TypeCode.NONKEY_MEMBER);

            tc.add_member("y", TypeCode.MEMBER_ID_INVALID, TypeCode.TC_DOUBLE,
                    TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
        }

        return tc;

    }

    static TypeCode outerStructGetTypeCode() {
        StructMember members[] = new StructMember[0];
        TypeCode tc;

        tc = TypeCodeFactory.TheTypeCodeFactory.create_struct_tc("OuterType",
                members);

        try {
            tc.add_member("inner", TypeCode.MEMBER_ID_INVALID,
                    innerStructGetTypeCode(), TypeCode.NONKEY_MEMBER);
        } catch (Exception e) {
            System.err.println(e.getMessage());
        }

        return tc;

    }

    public static void main(String[] args) {
        DynamicData inner_data = new DynamicData(innerStructGetTypeCode(),
                DynamicData.PROPERTY_DEFAULT);
        DynamicData outer_data = new DynamicData(outerStructGetTypeCode(),
                DynamicData.PROPERTY_DEFAULT);
        DynamicData bounded_data = new DynamicData(null,
                DynamicData.PROPERTY_DEFAULT);

        /* Setting the inner data */
        inner_data.set_double("x", DynamicData.MEMBER_ID_UNSPECIFIED, 3.14159);
        inner_data.set_double("y", DynamicData.MEMBER_ID_UNSPECIFIED, 2.71828);

        System.out.println("\n\n get/set_complex_member API");
        System.out.println("------------------\n");

        /* Get/Set complex member API */
        System.out.println("Setting the initial values of struct with set_complex_member()\n");
        outer_data.set_complex_member("inner",
                DynamicData.MEMBER_ID_UNSPECIFIED, inner_data);

        outer_data.print(null, 1);

        System.out.println("\n + get_complex_member() called");
        outer_data.get_complex_member(inner_data, "inner",
                DynamicData.MEMBER_ID_UNSPECIFIED);

        System.out.println("\n + inner struct value");
        inner_data.print(null, 1);

        System.out.println("\n + setting new values to inner struct\n");
        inner_data.set_double("x", DynamicData.MEMBER_ID_UNSPECIFIED, 1.00000);
        inner_data.set_double("y", DynamicData.MEMBER_ID_UNSPECIFIED, 0.00001);

        System.out.println("\n + current outter struct value \n");
        outer_data.print(null, 1);


        System.out.println("\n\n bind/unbind API");
        System.out.println("------------------\n");

        /* Bind/Unbind member API */
        System.out.println("\n + bind complex member called\n");
        outer_data.bind_complex_member(bounded_data, "inner",
                DynamicData.MEMBER_ID_UNSPECIFIED);

        bounded_data.print(null, 1);

        /* binding a member does not copy, so modifying the bounded member 
         * WILL modify the outer object */
        System.out.println("\n + setting new values to inner struct\n");
        bounded_data.set_double("x",
                DynamicData.MEMBER_ID_UNSPECIFIED, 1.00000);
        bounded_data.set_double("y",
                DynamicData.MEMBER_ID_UNSPECIFIED, 0.00001);

        /* Current value of outer data
		 outter:
		 inner:
		 x: 1.000000
		 y: 0.000010
         */

        bounded_data.print(null, 1);

        outer_data.unbind_complex_member(bounded_data);

        System.out.println("\n + current outter struct value");
        outer_data.print(null, 1);

    }

}
