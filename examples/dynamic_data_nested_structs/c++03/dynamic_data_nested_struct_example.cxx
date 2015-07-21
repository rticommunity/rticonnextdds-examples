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

DynamicType create_typecode_inner_struct()
{
    // First create the type code for a struct
    StructType inner_struct("InnerStruct");

    // Member 1 will be a double named x
    inner_struct.add_member(Member("x", primitive_type<double>()));

    // Member 2 will be a double named y
    inner_struct.add_member(Member("y", primitive_type<double>()));

    return inner_struct;
}

DynamicType create_typecode_outer_struct()
{
    // First create the type code for a struct
    StructType outer_struct("OuterStruct");

    // Member 1 of outer struct will be a struct of type InnerStruct
    outer_struct.add_member(Member("inner", create_typecode_inner_struct()));

    return outer_struct;
}

int main()
{
    // Create the type code of the InnerStruct and OuterStruct
    DynamicType inner_type = create_typecode_inner_struct();
    DynamicType outer_type = create_typecode_outer_struct();

    // Create a DynamicData instance for each type
    DynamicData outer_data(outer_type);
    DynamicData inner_data(inner_type);

    std::cout << " Connext Dynamic Data Nested Struct Example " << std::endl
              << "--------------------------------------------" << std::endl
              << " Data Type " << std::endl
              << "-----------" << std::endl;
    print_idl(inner_type);
    print_idl(outer_type);
    std::cout << std::endl;

    // Setting the inner data
    inner_data.value("x", 3.14159);
    inner_data.value("y", 2.71828);

    // Copy inner_data values in inner_struct of outer_data
    std::cout << " Setting the initial values of struct " << std::endl
              << "--------------------------------------" << std::endl;
    outer_data.value("inner", inner_data);
    std::cout << outer_data;

    // Clear inner_data members to copy data from outer_data
    inner_data.clear_all_members();
    inner_data = outer_data.value<DynamicData>("inner");
    std::cout << std::endl
              << " + copied struct from outer_data" << std::endl
              << " + inner_data value" << std::endl;
    std::cout << inner_data;

    // inner_data is a copy of the values. If we modify it, outer_data inner
    // field WILL NOT be modified.
    std::cout << std::endl
              << " + setting new values to inner_data" << std::endl;
    inner_data.value("x", 1.00000);
    inner_data.value("y", 0.00001);

    std::cout << inner_data << std::endl
              << " + current outer_data value " << std::endl
              << outer_data << std::endl << std::endl;

    // Using loan_value, we do not copy inner, but bind it.
    // So, if we modify loaned_inner, the inner member inside outer_data WILL
    // also be modified.
    std::cout << " loan/unloan API"  << std::endl
              << "-----------------" << std::endl
              << " + loan member called" << std::endl;
    LoanedDynamicData loaned_inner = outer_data.loan_value("inner");

    std::cout << loaned_inner.get() << std::endl
              << " + setting new values to loaned_data" << std::endl;
    loaned_inner.get().value("x", 1.00000);
    loaned_inner.get().value("y", 0.00001);

    std::cout << loaned_inner.get() << std::endl
              << " + current outer_data value" << std::endl;
    std::cout << outer_data;

    // The destructor will unloan the member
    return 0;
}
