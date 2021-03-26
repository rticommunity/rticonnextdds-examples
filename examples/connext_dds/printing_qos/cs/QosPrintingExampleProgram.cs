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
using Omg.Dds.Core.Policy;

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
            // we create a DataWriter and explicitly supply the Qos. We provide
            // a Qos which differs from that defined in USER_QOS_PROFILES.xml.
            // In the XML file the DataWriterQos is defined to be have a durability
            // of Volatile. Here, we override this, supplying a TransientLocal
            // Durability Qos policy.
            DataWriterQos writerQos = DataWriterQos.Default.WithDurability(
                policy => policy.Kind = DurabilityKind.TransientLocal);
            DataWriter<HelloWorld> writer = publisher.CreateDataWriter(topic, writerQos);

            // There is a defined precedence for Qos, Qos policies set in code
            // always take precedence over those defined in XML files. Using the
            // Qos printing APIs it is possible to demonstrate this.
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
            // QoS object to use as the baseQos to the ToString, alternatively
            // the sentinel value QosPrintAll.Value can be provided, which will
            // printing the entire Qos object (not just the differences).
            Console.WriteLine("Printing entire DataWriterQos object to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(writer.Qos.ToString(baseQos: QosPrintAll.Value));
        }
    }
} // namespace QosPrintingExample
