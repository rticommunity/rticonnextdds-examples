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

DynamicType create_dynamictype_simple_struct()
{
    // First create the type code for a struct.
    StructType simple_struct("SimpleStruct");

    // The member will be a integer named "a_member".
    simple_struct.add_member(Member("a_member", primitive_type<int32_t>()));

    return simple_struct;
}

DynamicType create_dynamictype_sequence_struct()
{
    // First create the type code for a struct.
    StructType type_with_sequence("TypeWithSequence");

    // Add a sequence of type SimpleStruct
    DynamicType simple_struct = create_dynamictype_simple_struct();
    type_with_sequence.add_member(Member(
        "sequence_member",
        SequenceType(simple_struct, MAX_SEQ_LEN)));

    return type_with_sequence;
}

void write_data(DynamicData& sample)
{
    // Get the sequence member to set its elements.
    DynamicData sequence_member = sample.value<DynamicData>("sequence_member");

    // Get the type of the sequence members.
    DynamicType simple_struct_dynamic_type = static_cast<const SequenceType&>(
        sequence_member.type()).content_type();

    // Write a new struct for each member of the sequence.
    for (int i = 0; i < MAX_SEQ_LEN; i++) {
        // To access the elements of a sequence it is necessary
        // to use their id. This parameter allows accessing to every element
        // of the sequence using a 1-based index.
        // There are two ways of doing this: loan API and value API.
        // See the dynamic_data_nested_structs for further details about the
        // differences between these two APIs.

        // **Loan**:
        // Get a reference to the i+1 element (1-based index).
        LoanedDynamicData loaned_data = sequence_member.loan_value(i + 1);

        // We're setting "a_member" whose type is int32_t, the same type as i.
        // If the value had a different type (e.g. short) a cast or a explicit
        // use of the template parameter would be required to avoid a runtime
        // error.
        loaned_data.get().value("a_member", i);

         // We're returning the loan explicitly; the destructor
         // of loaned_data would do it too.
        loaned_data.return_loan();

        // **Value**:
        // Create a simple_struct for this sequence element.
        DynamicData simple_struct_element(simple_struct_dynamic_type);
        simple_struct_element.value("a_member", i);

        // Add to the sequence.
        std::cout << "Writing sequence element #" << (i + 1) << " :"
                  << std::endl << simple_struct_element;
        sequence_member.value(i + 1, simple_struct_element);
    }

    sample.value("sequence_member", sequence_member);
}

void read_data(const DynamicData& sample)
{
    // Get the sequence member
    DynamicData sequence_member = sample.value<DynamicData>("sequence_member");

    // Get the total amount of elements contained in the sequence by accessing
    // the dynamic data info.
    std::cout << "* Getting sequence member info..." << std::endl;
    const DynamicDataInfo& info = sequence_member.info();
    int seq_len = info.member_count();
    std::cout << "* Sequence contains " << seq_len << " elements" << std::endl;

    for (int i = 0; i < seq_len; i++) {
        // Value getter:
        DynamicData struct_element = sequence_member.value<DynamicData>(i + 1);

        // Loan:
        // The same results can be obtained using 'loan_value' method.
        // The main difference, is that in that case the members are not copied,
        // just referenced. To access to the member value call 'get()' method.
        // The destructor will unloan the member.
        LoanedDynamicData loaned_struct = sequence_member.loan_value(i + 1);

        std::cout << "Reading sequence element #" << (i + 1) << " :"
                  << std::endl << struct_element; // or loaned_struct.get();
    }
}

int main()
{
    try {
        // Create the struct with the sequence member.
        DynamicType struct_sequence_type = create_dynamictype_sequence_struct();
        DynamicData struct_sequence_data(struct_sequence_type);

        std::cout << "***** Writing a sample *****" << std::endl;
        write_data(struct_sequence_data);
        std::cout << std::endl;

        std::cout << "***** Reading a sample *****" << std::endl;
        read_data(struct_sequence_data);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions.
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
