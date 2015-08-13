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

#include <iostream>
#include <dds/dds.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

// IDL representation for this example:
//
// struct SimpleStruct {
//     long a_member;
// };
//
// struct TypeWithSequence {
//     sequence<SimpleStruct,5> sequence_member;
// };
//

const int MAX_SEQ_LEN = 5;

DynamicType create_typecode_simple_struct()
{
    // First create the type code for a struct.
    StructType simple_struct("SimpleStruct");

    // The member will be a integer named "a_member".
    simple_struct.add_member(Member("a_member", primitive_type<int>()));

    return simple_struct;
}

DynamicType create_typecode_sequence_struct()
{
    // First create the type code for a struct.
    StructType type_with_sequence("TypeWithSequence");

    // Add a sequence of type SimpleStruct
    DynamicType simple_struct = create_typecode_simple_struct();
    type_with_sequence.add_member(Member(
        "sequence_member",
        SequenceType(simple_struct, MAX_SEQ_LEN)));

    return type_with_sequence;
}

void write_data(DynamicData& sample)
{
    DynamicType simple_struct_typecode = create_typecode_simple_struct();
    std::vector<DynamicData> sequence_data;
    for (int i = 0; i < MAX_SEQ_LEN; i++) {
        // To access the elements of a sequence it is necessary
        // to use their name. There are two ways of doing this: bind/loan API
        // and get API. See the "dynamic_data_nested_struct" for further details
        // about the differences between these two APIs.

        // TODO: Using loan

        // Get/Set:
        // Create a simple_struct for this sequence element.
        DynamicData simple_struct_element(simple_struct_typecode);
        simple_struct_element.value("a_member", i);

        // Add to the sequence.
        std::cout << "Writing sequence element #" << i << " : "
                  << std::endl << simple_struct_element << std::endl;
        sequence_data.push_back(simple_struct_element);
    }

    // Set the sequence value.
    sample.set_values("sequence_member", sequence_data);
}

void read_data(const DynamicData& sample)
{

}

int main()
{
    // Create the struct with the sequence member.
    DynamicType struct_sequence_type = create_typecode_sequence_struct();
    DynamicData struct_sequence_data(struct_sequence_type);

    std::cout << "***** Writing a sample *****" << std::endl;
    write_data(struct_sequence_data);

    std::cout << "***** Reading a sample *****" << std::endl;
    read_data(struct_sequence_data);

    return 0;
}
