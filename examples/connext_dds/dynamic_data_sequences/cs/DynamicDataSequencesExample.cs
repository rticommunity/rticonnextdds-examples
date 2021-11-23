/*
 * (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

using System;
using Rti.Types.Dynamic;

namespace DynamicDataSequencesExample
{
    /// <summary>
    /// This example creates a StructType with a complex sequence and shows
    /// how to populate it and inspect it.
    /// </summary>
    public static class Program
    {
        public static void Main(string[] _)
        {
            var type = CreateComplexSequenceType(maxSequenceLength: 5);
            var data = new DynamicData(type);

            Console.WriteLine("*** Populating the data ***");
            PopulateData(data);

            Console.WriteLine("\n*** Inspecting the data:");
            InspectData(data);
        }

        private static StructType CreateComplexSequenceType(uint maxSequenceLength)
        {
            var factory = DynamicTypeFactory.Instance;

            // Create a struct with a single int member:
            //
            // struct SimpleStruct
            // {
            //    int32 a_member;
            // };
            StructType simpleStruct = factory.BuildStruct()
                .WithName("SimpleStruct")
                .AddMember(new StructMember("a_member", factory.GetPrimitiveType<int>()))
                .Create();

            // Create a struct with a complex sequence:
            //
            // struct TypeWithSequence
            // {
            //     sequence<SimpleStruct, maxSequenceLength> sequence_member;
            // };
            SequenceType complexSequenceType = factory.CreateSequence(
                simpleStruct,
                maxSequenceLength);

            return factory.BuildStruct()
                .WithName("TypeWithSequence")
                .AddMember(new StructMember("sequence_member", complexSequenceType))
                .Create();
        }

        private static void PopulateData(DynamicData sample)
        {
            // Get a reference to the sequence member to set its elements
            using (LoanedDynamicData sequenceMember = sample.LoanValue("sequence_member"))
            {
                var sequenceType = (SequenceType) sequenceMember.Data.Type;
                var elementType = (StructType) sequenceType.ContentType;
                var maxElementCount = (int) sequenceType.Bounds;

                // Populate the sequence up to its maximum size
                for (int i = 0; i < maxElementCount; i++)
                {
                    // To access the elements of a sequence it is necessary
                    // to use their id. This parameter allows accessing every
                    // element of the sequence using a 1-based index.
                    //
                    // The sequence is automatically resized when the index
                    // is larger than the current lenght.
                    //
                    // There are two ways of doing this by reference (LoanValue)
                    // and by value (GetValue<DynamicData>, SetValue<DynamicData>).
                    // See the dynamic_data_nested_structs example for further
                    // details about the differences between these two APIs.
                    using (LoanedDynamicData element = sequenceMember.Data.LoanValue(i + 1))
                    {
                        // Set the int member. Note that you must use the SetValue
                        // overload taking an int. Passing an uint or a short
                        // would make the operation fail. If you need type
                        // conversions use SetAnyValue().
                        element.Data.SetValue("a_member", i);
                    }

                    // You can also get the element by value, making a copy:
                    DynamicData elementCopy = sequenceMember.Data.GetValue<DynamicData>(i + 1);
                    Console.WriteLine($"Set element {i + 1}: {elementCopy.Type.Name} [\n{elementCopy}]");
                }
            }

            Console.WriteLine($"Populated data: {sample.Type.Name} [\n{sample}]");
        }

        private static void InspectData(DynamicData sample)
        {
            // Get the sequence member by reference
            using (LoanedDynamicData sequenceMember = sample.LoanValue("sequence_member"))
            {
                int elementCount = sequenceMember.Data.Info.MemberCount;
                Console.WriteLine($"Sequence contains {elementCount} elements");

                for (int i = 0; i < elementCount; i++)
                {
                    // Get each element as a copy (we could also get it as a
                    // reference with LoanValue)
                    DynamicData structElementCopy =
                        sequenceMember.Data.GetValue<DynamicData>(i + 1);

                    int memberValue = structElementCopy.GetValue<int>("a_member");
                    Console.WriteLine($"Element {i + 1}: a_member = {memberValue}");
                }
            }
        }
    }
} // namespace PartitionsExample
