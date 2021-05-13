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
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace FlowControllerExample
{
    /// <summary>
    /// Example application that publishes FlowControllerExample.HelloWorld.
    /// </summary>
    public static class HelloWorldPublisher
    {
        /// <summary>
        /// Runs the publisher example.
        /// </summary>
        public static void RunPublisher(
            int domainId,
            int sampleCount,
            uint tokenBucketPeriodMs,
            CancellationToken cancellationToken)
        {
            // Create a custom flow controller configuration
            DomainParticipantQos participantQos = ConfigureFlowController(
                tokenBucketPeriodMs);

            // Start communicating in a domain, usually one participant per application
            using DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId, participantQos);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic =
                participant.CreateTopic<HelloWorld>("Example cfc");

            // Create a Publisher
            Publisher publisher = participant.CreatePublisher();

            // Create a writer with the QoS specified in the default profile
            // in USER_QOS_PROFILES.xml, which sets up the publish mode policy
            DataWriter<HelloWorld> writer = publisher.CreateDataWriter(topic);

            // Create a sample to write with a long payload
            var sample = new HelloWorld { str = new string('a', 999) };
            for (int count = 0;
                count < sampleCount && !cancellationToken.IsCancellationRequested;
                count++)
            {
                // Simulate a bursty writer by sending 10 samples at a time,
                // after sleeping for a period
                if (count % 10 == 0)
                {
                    Thread.Sleep(1000);
                }

                // Modify the data to be sent here
                sample.x = count;

                Console.WriteLine($"Writing x={sample.x}");

                // With asynchronous publish mode, the Write() puts the sample
                // in the queue but doesn't send it until the flow controller
                // indicates so.
                writer.Write(sample);
            }

            try
            {
                // Wait until all written samples have been actually published
                writer.WaitForAsynchronousPublishing(maxWait: Duration.FromSeconds(10));

                // And wait until the DataReader has acknowledged them
                writer.WaitForAcknowledgments(maxWait: Duration.FromSeconds(10));
            }
            catch (TimeoutException)
            {
                Console.WriteLine("Timed out waiting to publish all samples");
            }
        }

        private static DomainParticipantQos ConfigureFlowController(
            uint tokenBucketPeriodMs)
        {
            // We'll get the QoS defined in the XML profile "cfc_Profile"
            // (the default profile in USER_QOS_PROFILES.xml) and tweak the
            // token bucket period
            DomainParticipantQos baseQos =
                QosProvider.Default.GetDomainParticipantQos();
            if (tokenBucketPeriodMs == 0)
            {
                return baseQos;
            }

            // This is the name we use in the XML file
            const string flowControllerName =
                "dds.flow_controller.token_bucket.custom_flowcontroller";

            // baseQos.WithProperty(...) creates a new DomainParticipantQos
            // object with the changes we specify in the lambda function.
            return baseQos.WithProperty(property =>
            {
                // In WithProperty the input argument 'property' contains the
                // current values of baseQos.Property. We will just modify these
                // two properties.
                var period = Duration.FromMilliseconds(tokenBucketPeriodMs);
                property[$"{flowControllerName}.token_bucket.period.sec"] =
                    period.Seconds.ToString();
                property[$"{flowControllerName}.token_bucket.period.nanosec"] =
                    period.Nanoseconds.ToString();
            });
        }
    }
} // namespace FlowControllerExample
