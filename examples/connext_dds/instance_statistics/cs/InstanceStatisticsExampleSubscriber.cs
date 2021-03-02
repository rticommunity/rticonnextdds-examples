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
using Omg.Dds.Core;
using Rti.Dds.Core;
using Rti.Dds.Core.Status;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace InstanceStatisticsExample
{
    /// <summary>
    /// Example application that subscribes to HelloWorld.
    /// </summary>
    public sealed class HelloWorldSubscriber : IHelloWorldApplication
    {
        private readonly DomainParticipant participant;
        private readonly DataReader<HelloWorld> reader;

        private readonly WaitSet waitset = new WaitSet();

        private bool continueRunning = true;

        private int samplesRead;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Subscriber and DataReader<HelloWorld>.
        /// </summary>
        public HelloWorldSubscriber(int domainId)
        {
            // Start communicating in a domain, usually one participant per application
            // Load QoS profile from USER_QOS_PROFILES.xml file
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>("Example InstanceStatisticsExample_HelloWorld");

            // Create a subscriber
            Subscriber subscriber = participant.CreateSubscriber();

            // Create a DataReader, loading QoS profile from USER_QOS_PROFILES.xml.
            reader = subscriber.CreateDataReader(topic);

            // Obtain the DataReader's Status Condition
            StatusCondition statusCondition = reader.StatusCondition;

            // Enable the 'data available' status.
            statusCondition.EnabledStatuses = StatusMask.DataAvailable;

            // Associate an event handler with the status condition.
            // This will run when the condition is triggered, in the context of
            // the dispatch call (see below)
            statusCondition.Triggered += ProcessData;

            // Create a WaitSet and attach the StatusCondition
            waitset.AttachCondition(statusCondition);
        }

        /// <summary>
        /// Processes the data received by the DataReader.
        /// </summary>
        public void Run(int sampleCount)
        {
            while (samplesRead < sampleCount && continueRunning)
            {
                // Dispatch will call the handlers associated to the WaitSet
                // conditions when they activate
                Console.WriteLine("HelloWorld subscriber sleeping up to 1 sec...");
                waitset.Dispatch(Duration.FromSeconds(1));

                var status = reader.DataReaderCacheStatus;
                Console.WriteLine("Instance statistics: ");
                Console.WriteLine($"  * Alive instance count:      {status.AliveInstanceCount}");
                Console.WriteLine($"  * No-writers instance count: {status.NoWritersInstanceCount}");
                Console.WriteLine($"  * Disposed instance count:   {status.DisposedInstanceCount}");
                Console.WriteLine($"  * Detached instance count:   {status.DetachedInstanceCount}");
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

        private void ProcessData(Condition _)
        {
            // Take all samples. Samples are loaned to application; loan is
            // returned when the samples collection is Disposed.
            using (var samples = reader.Take())
            {
                foreach (var sample in samples)
                {
                    if (sample.Info.ValidData)
                    {
                        samplesRead++;
                        Console.WriteLine(sample.Data);
                    }
                    else
                    {
                        Console.WriteLine($"Instance state changed to: {sample.Info.State.Instance}");
                    }
                }
            }
        }
    }
} // namespace InstanceStatisticsExample
