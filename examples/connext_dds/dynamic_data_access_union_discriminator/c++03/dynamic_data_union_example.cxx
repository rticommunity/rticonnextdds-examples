/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <cstdlib>
#include <iostream>

#include <dds/dds.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

UnionType create_union_type() {
    // First, we create a DynamicType for a union.
    UnionType union_type("Foo", primitive_type<int32_t>());

    // Case 1 will be a short named aShort.
    union_type.add_member(UnionMember(
        "aShort", primitive_type<short>(), 0));

    // Case 2 will be a long named aLong.
    union_type.add_member(UnionMember(
        "aLong", primitive_type<int>(), UnionMember::DEFAULT_LABEL));

    // Case 3 will be a double named aDouble.
    union_type.add_member(UnionMember(
        "aDouble", primitive_type<double>(), 3));

    return union_type;
}

void example() {
    // Create the type of the union
    UnionType union_type = create_union_type();

    // Create the DynamicData.
    DynamicData union_data(union_type);

    // Get the current member, it will be the default one.
    union_data.value<int>("aLong", 0);
    DynamicDataMemberInfo info =
        union_data.member_info(union_data.discriminator_value());
    std::cout << "The member selected is " << info.member_name() << std::endl;

    // When we set a value in one of the members of the union,
    // the discriminator updates automatically to point that member.
    union_data.value<short>("aShort", 42);

    // The discriminator should now reflect the new situation.
    info = union_data.member_info(union_data.discriminator_value());
    std::cout << "The member selected is " << info.member_name();

    // Getting the value of the target member.
    short aShort = union_data.value<short>(union_data.discriminator_value());
    std::cout << " with value " << aShort << std::endl;
}

int main(int argc, char *argv[]) {
    try {
        example();
    } catch (const std::exception& ex) {
        std::cerr << "Caught excetion: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
