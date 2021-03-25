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
using System.Threading;
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;
using Omg.Dds.Core.Policy;

namespace QosPrintingExample
{
    /// <summary>
    /// Example application that publishes QosPrintingExample.HelloWorld.
    /// </summary>
    public static class HelloWorldPublisher
    {
        /// <summary>
        /// Runs the publisher example.
        /// </summary>
        public static void RunPublisher(int domainId)
        {
            // A DomainParticipant allows an application to begin communicating in
            // a DDS domain. Typically there is one DomainParticipant per application.
            // DomainParticipant QoS is configured in USER_QOS_PROFILES.xml
            //
            // A participant needs to be Disposed to release middleware resources.
            // The 'using' keyword indicates that it will be Disposed when this
            // scope ends.
            using DomainParticipant participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>("Example QosPrintingExample_HelloWorld");

            // A Publisher allows an application to create one or more DataWriters
            // Publisher QoS is configured in USER_QOS_PROFILES.xml
            Publisher publisher = participant.CreatePublisher();

            // This DataWriter will write data on Topic "Example HelloWorld"
            // DataWriter QoS is configured in USER_QOS_PROFILES.xml
            DataWriterQos volatileWriterQos = DataWriterQos.Default.WithDurability(
                policy => policy.Kind = DurabilityKind.Volatile);
            DataWriter<HelloWorld> volatileWriter = publisher.CreateDataWriter(topic, volatileWriterQos);
            DataWriterQos durableWriterQos = DataWriterQos.Default.WithDurability(
                policy => policy.Kind = DurabilityKind.TransientLocal);
            DataWriter<HelloWorld> durableWriter = publisher.CreateDataWriter(topic, durableWriterQos);

            // --- QosPrintingExample ---

            // Qos objects override ToString so can be printed directly
            Console.WriteLine("Printing TopicQos to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(topic.Qos);

            // The output can be configured using QosPrintFormat
            QosPrintFormat format = new QosPrintFormat
            {
                Indent = 1,
                IsStandalone = true
            };
            Console.WriteLine("Printing WriterQos with format to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(durableWriter.Qos.ToString(format));

            // By default, only the differences with respect to the documented
            // QoS default (the default values as stated in the API reference)
            // are printed. This behavior can be overridden by passing another
            // QoS object to use as the baseQos to the ToString
            Console.WriteLine("Printing WriterQos with respect to another WriterQos object to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(durableWriter.Qos.ToString(baseQos: volatileWriter.Qos));

            // Alternatively, pass QosPrintAll.Value as the baseQos to print the entire QoS object
            Console.WriteLine("Printing all contents of WriterQos to stdout");
            Console.WriteLine("Press enter to continue");
            Console.ReadLine();
            Console.WriteLine(durableWriter.Qos.ToString(baseQos: QosPrintAll.Value));

            // --- QosPrintingExample ---
        }
    }
} // namespace QosPrintingExample
