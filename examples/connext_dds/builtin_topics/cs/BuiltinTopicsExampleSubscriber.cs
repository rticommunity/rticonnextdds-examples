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
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Rti.Dds.Core.Policy;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace BuiltinTopicsExample
{
    /// <summary>
    /// Example application that subscribes to HelloWorld.
    /// </summary>
    public sealed class HelloWorldSubscriber : IHelloWorldApplication
    {
        private readonly DomainParticipant participant;
        private readonly DataReader<HelloWorld> reader;
        private int samplesRead;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Subscriber and DataReader<HelloWorld>.
        /// </summary>
        public HelloWorldSubscriber(int domainId, string password, string topicName)
        {
            // Configure the DomainParticipantQos add the UserData policy to it.
            // We're simply converting the input string into bytes, assuming
            // that only ASCII characters are used to keep the example simple.
            // We also set the maximum user data length.
            DomainParticipantQos participantQos =
                DomainParticipantFactory.Instance.DefaultParticipantQos
                .WithUserData(new UserData(password.Select(c => (byte) c)))
                .WithResourceLimits(policy => policy.ParticipantUserDataMaxLength = 1024);

            // Create the participant with the QoS
            participant = DomainParticipantFactory.Instance
                .CreateParticipant(domainId, participantQos);

            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>(topicName);

            // Create a subscriber and a DataReader
            Subscriber subscriber = participant.CreateSubscriber();
            reader = subscriber.CreateDataReader(topic);
        }

        /// <summary>
        /// Processes the data received by the DataReader.
        /// </summary>
        public async Task Run(int sampleCount, CancellationToken cancellationToken)
        {
            // TakeAsync provides an IAsyncEnumerable that returns new data
            // samples when they are available, awaiting as necessary. The
            // cancellation token allows stopping the loop.
            await foreach (var sample in reader
                .TakeAsync()
                .WithCancellation(cancellationToken))
            {
                if (sample.Info.ValidData)
                {
                    samplesRead++;
                    Console.WriteLine(sample.Data);

                    if (samplesRead == sampleCount)
                    {
                        break;
                    }
                }
                else
                {
                    Console.WriteLine($"Instance state changed to: {sample.Info.State.Instance}");
                }
            }
        }

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();
    }
} // namespace BuiltinTopicsExample
