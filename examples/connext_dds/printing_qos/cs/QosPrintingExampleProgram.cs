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
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;
using Rti.Dds.Core.Policy;

namespace QosPrintingExample
{
    /// <summary>
    /// Runs the QosPrintingExample
    /// </summary>
    public static class HelloWorldProgram
    {
        /// <summary>
        /// The Main function runs the Qos printing example.
        /// </summary>
        public static void Main(string[] args)
        {
            using DomainParticipant participant = DomainParticipantFactory.Instance.CreateParticipant(0);
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>("Example QosPrintingExample_HelloWorld");
            Publisher publisher = participant.CreatePublisher();

            // In order to demonstrate a possible use-case for the Qos printing APIs
            // we create a DataWriter which is a combination of policies set in
            // the XML file, and of policies set in code.
            DataWriterQos writerQos = QosProvider.Default.GetDataWriterQos()
                .WithReliability(policy => policy.Kind = ReliabilityKind.BestEffort);
            DataWriter<HelloWorld> writer = publisher.CreateDataWriter(topic, writerQos);


            // The Qos we have obtained from the XML file inherits from the
            // Generic.StrictReliable built-in profile. It also explicitly sets
            // the Durability Qos policy. We have also explicitly set (in code)
            // the Reliability Qos (overriding the ReliabilityKind set in the
            // built-in profile). Using the Qos printing APIs we can check what
            // is the final Qos in use by an entity.
            Console.WriteLine("Printing DataWriterQos to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(writer.Qos);

            // The output can be configured using QosPrintFormat
            QosPrintFormat format = new QosPrintFormat
            {
                Indent = 1,
                IsStandalone = true
            };
            Console.WriteLine("Printing formatted DataWriterQos to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(writer.Qos.ToString(format));

            // By default, only the differences with respect to the documented
            // QoS default (the default values as stated in the API reference)
            // are printed. This behavior can be overridden by passing another
            // QoS object to use as the baseQos to ToString.
            Console.WriteLine("Printing DataWriterQos differences with respect to \"BuiltinQosLib::Generic.StrictReliable\" to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(writer.Qos.ToString(
                baseQos: QosProvider.Default.GetDataWriterQos("BuiltinQosLib::Generic.StrictReliable")));

            // To print all the Qos policies (not just the differences), call
            // ToString with sentinel QosPrintAll.Value
            Console.WriteLine("Printing entire DataWriterQos object to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(writer.Qos.ToString(baseQos: QosPrintAll.Value));
        }
    }
} // namespace QosPrintingExample
