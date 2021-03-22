/*
* (c) Copyright, Real-Time Innovations, 2012.  All rights reserved.
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
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace FlowControllerExample
{
    /// <summary>
    /// Example application that publishes FlowControllerExample.HelloWorld.
    /// </summary>
    public static class HelloWorldSubscriber
    {
        /// <summary>
        /// Runs the subscriber example.
        /// </summary>
        public static async Task RunSubscriber(
            int domainId,
            int sampleCount,
            CancellationToken cancellationToken)
        {
            // Start communicating in a domain, usually one participant per application
            // Load default QoS profile from USER_QOS_PROFILES.xml file
            using DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic =
                participant.CreateTopic<HelloWorld>("Example cfc");

            // Create a subscriber
            Subscriber subscriber = participant.CreateSubscriber();

            // Create a DataReader with the default QoS profile defined in
            // USER_QOS_PROFILES.xml
            DataReader<HelloWorld> reader = subscriber.CreateDataReader(topic);

            var beginTime = DateTime.Now;

            // TakeAsync provides an IAsyncEnumerable that returns new data
            // samples when they are available, awaiting as necessary. The
            // cancellation token allows stopping the loop.
            int samplesRead = 0;
            await foreach (var sample in reader
                .TakeAsync()
                .WithCancellation(cancellationToken))
            {
                if (sample.Info.ValidData)
                {
                    samplesRead++;
                    var elapsedTime = (DateTime.Now - beginTime).TotalSeconds;
                    Console.WriteLine($"At t={elapsedTime}s, got x={sample.Data.x}");

                    if (samplesRead == sampleCount)
                    {
                        break;
                    }
                }
            }
        }
    }
} // namespace FlowControllerExample
