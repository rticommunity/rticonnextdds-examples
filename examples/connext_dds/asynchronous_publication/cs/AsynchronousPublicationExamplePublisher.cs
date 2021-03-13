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
using System.Threading.Tasks;
using Omg.Dds.Core;
using Rti.Dds.Core;
using Rti.Dds.Core.Policy;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace AsynchronousPublicationExample
{
    /// <summary>
    /// Example application that publishes HelloWorld
    /// </summary>
    public sealed class HelloWorldPublisher : IHelloWorldApplication
    {
        private readonly DomainParticipant participant;
        private readonly DataWriter<HelloWorld> writer;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Publisher and DataWriter<HelloWorld>.
        /// </summary>
        public HelloWorldPublisher(int domainId, bool useXmlQos = true)
        {
            // Start communicating in a domain, usually one participant per application
            // Load default QoS profile from USER_QOS_PROFILES.xml file
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>("Example async");

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
                        policy.RtpsReliableWriter = policy.RtpsReliableWriter.With(rtpsReliableWriter =>
                        {
                            rtpsReliableWriter.MinSendWindowSize = 50;
                            rtpsReliableWriter.MaxSendWindowSize = 50;
                        });
                    })
                    .WithPublishMode(policy =>
                    {
                        policy.Kind = PublishModeKind.Asynchronous;
                        policy.FlowControllerName = "DDS_FIXED_RATE_FLOW_CONTROLLER_NAME";
                    });
            }

            writer = publisher.CreateDataWriter(topic, writerQos);
        }

        /// <summary>
        /// Publishes the data
        /// </summary>
        public async Task Run(int sampleCount, CancellationToken cancellationToken)
        {
            var sample = new HelloWorld();
            for (int count = 0; 
                count < sampleCount && !cancellationToken.IsCancellationRequested;
                count++)
            {
                // Modify the data to be sent here
                sample.x = count;

                Console.WriteLine($"Writing HelloWorld, count {count}");

                writer.Write(sample);

                await Task.Delay(100, cancellationToken);
            }
        }

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();
    }
} // namespace AsynchronousPublicationExample
