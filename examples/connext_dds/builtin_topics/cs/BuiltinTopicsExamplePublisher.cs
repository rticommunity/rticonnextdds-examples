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
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Omg.Dds.Subscription;
using Rti.Dds.Core.Policy;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace BuiltinTopicsExample
{
    /// <summary>
    /// Example application that publishes HelloWorld and monitors the built-in
    /// Participant and Subscription DataReaders.
    /// </summary>
    public sealed class HelloWorldPublisher : IHelloWorldApplication
    {
        private readonly DomainParticipant participant;
        private readonly DataWriter<HelloWorld> writer;
        private DataReader<ParticipantBuiltinTopicData> participantReader;
        private DataReader<SubscriptionBuiltinTopicData> subscriptionReader;
        private readonly string expectedPassword;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Publisher and DataWriter.
        /// </summary>
        public HelloWorldPublisher(
            int domainId,
            string expectedPassword,
            string topicName)
        {
            // Save the expected password. We will only communicate with
            // subscriptions that send this password via discovery.
            this.expectedPassword = expectedPassword;

            // Configure the participant QoS to increase the user data max length
            DomainParticipantQos participantQos =
                DomainParticipantFactory.Instance.DefaultParticipantQos
                .WithResourceLimits(policy => policy.ParticipantUserDataMaxLength = 1024);

            // Create the participant
            participant = DomainParticipantFactory.Instance.CreateParticipant(
                domainId,
                qos: participantQos,
                preEnableAction: p =>
                {
                    // The preEnableAction is executed right before the participant
                    // is enabled and communication starts. By looking up the
                    // built-in discovery readers here we ensure that
                    // that they will receive all the discovery information.
                    participantReader = p.BuiltinSubscriber
                        .LookupDataReader<ParticipantBuiltinTopicData>(
                            Subscriber.ParticipantBuiltinTopicName);

                    // The DataAvailable event is called when another participant
                    // is discovered and before this participant has started
                    // communicating with it
                    participantReader.DataAvailable += OnNewParticipant;

                    subscriptionReader = p.BuiltinSubscriber
                        .LookupDataReader<SubscriptionBuiltinTopicData>(
                            Subscriber.SubscriptionBuiltinTopicName);

                    subscriptionReader.DataAvailable += OnNewSubscription;
                }
            );

            // Create a topic, a publisher and a writer
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>(topicName);
            Publisher publisher = participant.CreatePublisher();
            writer = publisher.CreateDataWriter(topic);
        }

        /// <summary>
        /// Publishes the data, which will only be received by the subscriptions
        /// whose participants have set the right password
        /// </summary>
        public async Task Run(int sampleCount, CancellationToken cancellationToken)
        {
            var sample = new HelloWorld();
            for (int count = 0;
                count < sampleCount && !cancellationToken.IsCancellationRequested;
                count++)
            {
                // Modify the data to be sent here
                sample.x = (short)count;

                Console.WriteLine($"Writing HelloWorld, count {count}");

                writer.Write(sample);

                await Task.Delay(1000, cancellationToken);
            }
        }

        private void OnNewParticipant(AnyDataReader _)
        {
            // Take all the data about new participants
            using LoanedSamples<ParticipantBuiltinTopicData> samples = participantReader
                .Select()
                .WithState(ViewState.New.And(InstanceState.Alive))
                .Take();

            foreach (var sample in samples)
            {
                if (!sample.Info.ValidData)
                {
                    continue;
                }

                Console.WriteLine($"Discovered participant: {sample.Data}");
                if (!IsAuthorized(sample.Data.UserData))
                {
                    // Disallow communication with participants that don't
                    // provide the correct password
                    participant.IgnoreParticipant(sample.Info.InstanceHandle);
                    Console.WriteLine("    * Authorization DENIED");
                }
                else
                {
                    Console.WriteLine("    * Authorization OK");
                }
            }
        }

        private void OnNewSubscription(AnyDataReader _)
        {
            // Take all the data about new subscriptions
            using LoanedSamples<SubscriptionBuiltinTopicData> samples = subscriptionReader
                .Select()
                .WithState(ViewState.New.And(InstanceState.Alive))
                .Take();

            foreach (var sample in samples)
            {
                if (!sample.Info.ValidData)
                {
                    continue;
                }

                Console.WriteLine($"Discovered subscription: {sample.Data}");
            }
        }

        private bool IsAuthorized(UserData discoveredUserData)
        {
            // Convert the bytes back to char (assuming ASCII only)
            var password = new string(discoveredUserData.Value
                .Select(b => (char)b)
                .ToArray());
            return password == expectedPassword;
        }

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();
    }
} // namespace BuiltinTopicsExample
