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

// A simple HelloWorld using network capture to save DomainParticipant traffic.
//
// This example is a simple hello world running network capture for both a
// publisher participant (network_capturePublisher.cs).and a subscriber
// participant (network_captureSubscriber.cs). It shows the basic flow when
// working with network capture:
//   - Enabling before anything else.
//   - Start capturing traffic (for one or all participants).
//   - Pause/resume capturing traffic (for one or all participants).
//   - Stop capturing trafffic (for one or all participants).
//     This is automatically done when a NetworkCapture instance goes out of
//     scope. Participants for which we are capturing traffic must outlive the
//     instance.
//   - Disable after everything else.

using System;
using System.Threading;
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;
using Rti.Utility;

namespace NetworkCaptureExample
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
            // Enable network capture.
            // This must be called before any other network capture function, and
            // before creating any participant for which we want to capture traffic.
            NetworkCapture.Enable();

            // Start communicating in a domain, usually one participant per application
            // Load QoS profile from USER_QOS_PROFILES.xml file
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>(
                    "Network capture shared memory example");

            // Create a Publisher
            Publisher publisher = participant.CreatePublisher();

            // Create a DataWriter, loading QoS profile from USER_QOS_PROFILES.xml, and
            writer = publisher.CreateDataWriter(topic);
        }

        /// <summary>
        /// Publishes the data
        /// </summary>
        public void Run(int sampleCount)
        {
            // Start capturing traffic for all participants.
            NetworkCapture capture = NetworkCapture.Start("publisher");

            var sample = new HelloWorld();
            for (int count = 0; count < sampleCount && continueRunning; count++)
            {
                // Modify the data to be sent here
                sample.msg = $"Hello {count}";

                Console.WriteLine($"Writing HelloWorld, count {count}");

                if (count == 4)
                {
                    Console.WriteLine("Pausing the capture at sample 4");
                    capture.Pause();
                }
                else if (count == 6)
                {
                    Console.WriteLine("Resuming the capture at sample 6");
                    capture.Resume();
                }

                writer.Write(sample);

                Thread.Sleep(1000);
            }
        }

        /// <summary>
        /// Signals that Run() should return early.
        /// </summary>
        public void Stop() => continueRunning = false;

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose()
        {
            participant.Dispose();

            // Disable network capture.
            // This must be the last network capture operation that is called.
            NetworkCapture.Disable();
        }
    }
} // namespace NetworkCaptureExample
