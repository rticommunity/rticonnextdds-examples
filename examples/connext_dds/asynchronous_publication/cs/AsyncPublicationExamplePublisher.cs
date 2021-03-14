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
using Omg.Dds.Core;
using Rti.Dds.Core;
using Rti.Dds.Core.Policy;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace AsyncPublicationExample
{
    /// <summary>
    /// Example application that publishes AsyncPublicationExample.HelloWorld.
    /// </summary>
    public static class HelloWorldPublisher
    {
        /// <summary>
        /// Runs the publisher example.
        /// </summary>
        public static void RunPublisher(
            int domainId,
            int sampleCount,
            CancellationToken cancellationToken,
            bool useXmlQos = false)
        {
            // Start communicating in a domain, usually one participant per application
            // Load default QoS profile from USER_QOS_PROFILES.xml file
            using DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic =
                participant.CreateTopic<HelloWorld>("Example async");

            // Create a Publisher
            Publisher publisher = participant.CreatePublisher();

            DataWriterQos writerQos;
            if (useXmlQos)
            {
                // Retrieve the DataWriterQos from the default profile in
                // USER_QOS_PROFILES.xml
                writerQos = QosProvider.Default.GetDataWriterQos();
            }
            else
            {
                // Configure the DataWriterQos in code, to the same effect as
                // the XML file.
                writerQos = publisher.DefaultDataWriterQos
                    .WithReliability(policy =>
                    {
                        policy.Kind = ReliabilityKind.Reliable;
                        policy.MaxBlockingTime = Duration.FromSeconds(60);
                    })
                    .WithHistory(policy =>
                    {
                        policy.Kind = HistoryKind.KeepLast;
                        policy.Depth = 12;
                    })
                    .WithProtocol(policy =>
                    {
                        policy.RtpsReliableWriter.MinSendWindowSize = 50;
                        policy.RtpsReliableWriter.MaxSendWindowSize = 50;
                    })
                    .WithPublishMode(policy =>
                    {
                        policy.Kind = PublishModeKind.Asynchronous;

                        // The built-in fixed-rate flow controller publishes
                        // all written samples once per second
                        policy.FlowControllerName = PublishMode.FixedRateFlowControllerName;
                    });
            }

            DataWriter<HelloWorld> writer =
                publisher.CreateDataWriter(topic, writerQos);

            var sample = new HelloWorld();
            for (int count = 0;
                count < sampleCount && !cancellationToken.IsCancellationRequested;
                count++)
            {
                // Modify the data to be sent here
                sample.x = count;

                Console.WriteLine($"Writing x={sample.x}");

                // With asynchronous publish mode, the Write() puts the sample
                // in the queue but doesn't send it until the flow controller
                // indicates so.
                writer.Write(sample);

                Thread.Sleep(100);
            }

            // You can wait until all written samples have been actually published
            // (note that this doesn't ensure that they have actually been received
            // if the sample required retransmission)
            writer.WaitForAsynchronousPublishing(Duration.FromSeconds(10));
        }
    }
} // namespace AsyncPublicationExample
