/*******************************************************************************
 (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

EnumType create_enum_type()
{
    return EnumType(
            "EngineState",
            { EnumMember("off", 0),
              EnumMember("on", 42)});
}

DynamicType create_struct_dynamic_type()
{
    // First create the type code for a struct
    StructType my_struct("EnumStruct");
    EnumType inner_enum = create_enum_type();

    // Member 1 will be an enum EngineState named engine_state_element
    my_struct.add_member(Member("engine_state_element",
        static_cast<const dds::core::xtypes::DynamicType &>(inner_enum)));

    return my_struct;
}

int32_t get_enum_ordinal_value_by_name(
        DynamicType struct_dynamic_type,
        std::string enum_element) {

    //Get value using scenario 1 of `print_enum_ordinal_value_by_name`
    const StructType &dynamic_struct =
            static_cast<const StructType &>(struct_dynamic_type);
    EnumType enum_type = static_cast<const EnumType &>(
            dynamic_struct.member("engine_state_element").type());

    // Return the specified value
    return enum_type.member(enum_element).ordinal();
}

void print_enum_ordinal_values_from_dynamic_type(
        DynamicType struct_dynamic_type) {

    // If you need a switch/case statement you can use
    // enum_dynamic_type.kind().underlying()
    if (struct_dynamic_type.kind() == TypeKind::STRUCTURE_TYPE) {

        // Different ways of getting the enum ordinals
        // Scenario 1. We already know the member name and it is an enum

        // cast the DynamicType into a StructType
        const StructType &dynamic_struct =
            static_cast<const StructType &>(struct_dynamic_type);

        EnumType enum_type = static_cast<const EnumType &>(
            dynamic_struct.member("engine_state_element").type());

        std::cout << "Scenario 1 - print enum ordinals from DynamicType" << std::endl;
        std::cout << "On: " << enum_type.member("on").ordinal() << std::endl;
        std::cout << "Off: " << enum_type.member("off").ordinal() << std::endl << std::endl;


        // Scenario 2, iterate through all the elements in the StructType
        // and do something for the enums
        for (auto member : dynamic_struct.members()) {
            // here you may want to use a switch/case
            if (member.type().kind() == TypeKind::ENUMERATION_TYPE) {
                EnumType engine_state_type = static_cast<const EnumType &>(
                    member.type());

                // This assumes that we know the enum strings
                if (engine_state_type.name() == "EngineState") {
                    std::cout << "Scenario 2 - print enum ordinals from DynamicType iterating members" << std::endl;
                    std::cout << "On: " << engine_state_type.member("on").ordinal() << std::endl;
                    std::cout << "Off: " << engine_state_type.member("off").ordinal() << std::endl << std::endl;
                }
            }
        }
    }

    return;
}

void print_enum_ordinal_values_from_dynamic_data(
        DynamicData struct_dynamic_data) {

    // Scenario 3, we can get the member type directly as a DynamicType
    // This assumes that we already know the name "EngineState" and that it is
    // an Enum.
    // This is similar to Scenario 1, but getting directly the EngineState as a
    // DynamicType instead of the struct that contains it.
    DynamicType enum_member_type = struct_dynamic_data.member_type("engine_state_element");
    EnumType enum_type = static_cast<const EnumType &>(enum_member_type);

    std::cout << "Scenario 3 - print enum ordinals from DynamicData" << std::endl;
    std::cout << "On: " << enum_type.member("on").ordinal() << std::endl;
    std::cout << "Off: " << enum_type.member("off").ordinal() << std::endl << std::endl;

    return;
}

void example()
{
    // Create the type of the struct with the enum
    DynamicType enum_dynamic_type = create_struct_dynamic_type();

    std::cout << "Print IDL corresponding type" << std::endl;
    print_idl(enum_dynamic_type);
    std::cout << std::endl;

    // Create the DynamicData.
    DynamicData enum_data(enum_dynamic_type);

    // Set the value of the current member
    enum_data.value<int32_t>(
        "engine_state_element",
        get_enum_ordinal_value_by_name(enum_dynamic_type, "on"));

    // Print the values of the enum elements from a dynamic data. It uses
    // the DynamicType internally.
    print_enum_ordinal_values_from_dynamic_type(enum_dynamic_type);
    print_enum_ordinal_values_from_dynamic_data(enum_data);

    std::cout << "Print enum ordinal value of DynamicData" << std::endl;
    std::cout << "Value: " << enum_data.value<int32_t>("engine_state_element") << std::endl;

    return;
}

int main(int argc, char *argv[])
{
    try {
        example();
    } catch (const std::exception &ex) {
        std::cerr << "Caught exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
