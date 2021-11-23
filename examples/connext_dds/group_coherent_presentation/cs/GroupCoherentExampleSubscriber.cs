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
using Omg.Dds.Core;
using Omg.Dds.Subscription;
using Rti.Dds.Core;
using Rti.Dds.Core.Policy;
using Rti.Dds.Core.Status;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;

namespace GroupCoherentExample
{
    /// <summary>
    /// Example application that subscribes to Alarm, HeartRate and Temperature.
    /// </summary>
    public sealed class SubscriberApplication : IApplication
    {
        private readonly DomainParticipant participant;

        private readonly Subscriber subscriber;
        private readonly DataReader<Alarm> alarmReader;
        private readonly DataReader<HeartRate> heartRateReader;
        private readonly DataReader<Temperature> temperatureReader;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Subscriber and DataReader<Temperature>.
        /// </summary>
        public SubscriberApplication(int domainId, bool useXmlQos = true)
        {
            // Start communicating in a domain, usually one participant per application
            // Load QoS profile from USER_QOS_PROFILES.xml file
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // Create the topics
            var alarmTopic = participant.CreateTopic<Alarm>("Alarm");
            var heartRateTopic = participant.CreateTopic<HeartRate>("HeartRate");
            var temperatureTopic = participant.CreateTopic<Temperature>("Temperature");

            // Create a subscriber
            SubscriberQos subscriberQos = null;
            if (useXmlQos)
            {
                // Retrieve the Subscriber QoS, from USER_QOS_PROFILES.xml
                subscriberQos = QosProvider.Default.GetSubscriberQos();
            }
            else
            {
                // Set the Subscriber QoS programatically, to the same effect
                subscriberQos = participant.DefaultSubscriberQos
                    .WithPresentation(policy =>
                    {
                        policy.AccessScope = PresentationAccessScopeKind.Group;
                        policy.CoherentAccess = true;
                        policy.OrderedAccess = true;

                        // Uncomment this line to keep and deliver incomplete
                        // coherent sets to the application
                        // policy.DropIncompleteCoherentSet = false;
                    });
            }
            subscriber = participant.CreateSubscriber(subscriberQos);

            // Create a DataReader for each topic
            DataReaderQos readerQos = null;
            if (useXmlQos)
            {
                // Retrieve the DataReader QoS, from USER_QOS_PROFILES.xml
                readerQos = QosProvider.Default.GetDataReaderQos();
            }
            else
            {
                // Set the DataReader QoS programatically, to the same effect
                readerQos = subscriber.DefaultDataReaderQos
                    .WithReliability(policy => policy.Kind = ReliabilityKind.Reliable)
                    .WithHistory(policy => policy.Kind = HistoryKind.KeepAll);
            }

            alarmReader = subscriber.CreateDataReader(alarmTopic, readerQos);
            heartRateReader = subscriber.CreateDataReader(heartRateTopic, readerQos);
            temperatureReader = subscriber.CreateDataReader(temperatureTopic, readerQos);

            // We are monitoring the sample lost status in case an
            // incomplete coherent set is received and dropped (assuming the
            // Presentation.DropIncompleteCoherentSet is true (the default))
            alarmReader.SampleLost += OnSampleLost;
            heartRateReader.SampleLost += OnSampleLost;
            temperatureReader.SampleLost += OnSampleLost;
        }

        /// <summary>
        /// Process the data received by the readers
        /// </summary>
        public Task Run(int _, CancellationToken cancellationToken)
        {
            var condition = subscriber.StatusCondition;
            condition.EnabledStatuses = StatusMask.DataOnReaders;
            condition.Triggered += ProcessData;

            var waitSet = new WaitSet();
            waitSet.AttachCondition(condition);

            return Task.Run(() =>
            {
                while (!cancellationToken.IsCancellationRequested)
                {
                    waitSet.Dispatch(Duration.FromSeconds(1));
                }
            }, cancellationToken);
        }

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();

        private void ProcessData(Condition _)
        {
            Console.WriteLine("---- Data available ----\n");

            using (subscriber.BeginAccess()) // Begin coherent access
            {
                // Get the list of readers with samples that have not been read yet
                var readers = subscriber.GetDataReaders(SampleState.NotRead);

                // Iterate through the returned readers list and take their samples
                foreach (AnyDataReader reader in readers)
                {
                    var topicName = reader.TopicDescription.Name;

                    // We're reading the data polymorphically as objects, since
                    // we're only calling the ToString() method. To read the
                    // concrete type, cast down each reader, for example:
                    //   if (reader is DataReader<Alarm> alarmReader) { ... }
                    using (LoanedSamples<object> samples = reader.TakeUntyped())
                    {
                        foreach (LoanedSample<object> sample in samples)
                        {
                            PrintCoheretSetInfo(sample.Info);
                            Console.WriteLine(topicName + " {\n" + sample + "}\n");
                        }
                    }
                }
            } // end coherent access
        }

        private static void PrintCoheretSetInfo(SampleInfo info)
        {
            // The coherent_set_info in the SampleInfo is only present if the sample is
            // part of a coherent set. We therefore need to check if it is present
            // before accessing any of the members
            var setInfo = info.CoherentSetInfo;
            if (setInfo != null)
            {
                var complete = setInfo.IncompleteCoherentSet ? "an incomplete" : "a complete";
                var sn = setInfo.GroupCoherentSetSequenceNumber;
                Console.WriteLine($"The following sample is part of {complete} group coherent set with SN {sn}:");
            }
            else
            {
                Console.WriteLine("The following sample is not part of a coherent set:");
            }
        }

        private void OnSampleLost(AnyDataReader reader, SampleLostStatus status)
        {
            if (status.LastReason == SampleLostStatusKind.LostByIncompleteCoherentSet)
            {
                Console.WriteLine(
                    $"Lost {status.TotalCount.Change} samples of topic {reader.TopicDescription.Name} in an incomplete coherent set.");
            }
        }
    }
} // namespace GroupCoherentExample
