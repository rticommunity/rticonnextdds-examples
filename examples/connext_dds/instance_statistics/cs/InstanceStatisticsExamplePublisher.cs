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
using Rti.Dds.Core.Status;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace InstanceStatisticsExample
{
    /// <summary>
    /// Example application that publishes HelloWorld
    /// </summary>
    public sealed class HelloWorldPublisher : IHelloWorldApplication
    {
        private readonly DomainParticipant participant;
        private readonly DataWriter<HelloWorld> writer;

        private bool continueRunning = true;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Publisher and DataWriter<HelloWorld>.
        /// </summary>
        public HelloWorldPublisher(int domainId)
        {
            // Start communicating in a domain, usually one participant per application
            // Load QoS profile from USER_QOS_PROFILES.xml file
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>(
                "Example InstanceStatisticsExample_HelloWorld");

            // Create a Publisher
            Publisher publisher = participant.CreatePublisher();

            // Create a DataWriter, loading QoS profile from USER_QOS_PROFILES.xml
            writer = publisher.CreateDataWriter(topic);
        }

        /// <summary>
        /// Publishes the data
        /// </summary>
        public void Run(int sampleCount)
        {
            var sample = new HelloWorld();
            for (int count = 0; count < sampleCount && continueRunning; count++)
            {
                // Modify the data to be sent here
                sample.message =  $"Hello {count}";

                Console.WriteLine($"Writing HelloWorld, count {count}");

                InstanceHandle instanceHandle = writer.RegisterInstance(sample);
                writer.Write(sample, instanceHandle);

                Thread.Sleep(1000);

                if (count % 2 == 0)
                {
                    Console.WriteLine("Unregistering the instance");
                    writer.UnregisterInstance(instanceHandle);
                }
                else if (count % 3 == 0)
                {
                    Console.WriteLine("Disposing the instance");
                    writer.DisposeInstance(instanceHandle);
                }

                var status = writer.DataWriterCacheStatus;
                Console.WriteLine("Instance statistics: ");
                Console.WriteLine($"  * Alive instance count:        {status.AliveInstanceCount}");
                Console.WriteLine($"  * Unregistered instance count: {status.UnregisteredInstanceCount}");
                Console.WriteLine($"  * Disposed instance count:     {status.DisposedInstanceCount}");
            }
        }

        /// <summary>
        /// Signals that Run() should return early.
        /// </summary>
        public void Stop() => continueRunning = false;

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();
    }
} // namespace InstanceStatisticsExample
