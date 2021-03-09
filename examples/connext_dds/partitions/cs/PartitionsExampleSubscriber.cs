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
using System.Threading.Tasks;
using Omg.Dds.Core.Policy;
using Omg.Dds.Subscription;
using Rti.Dds.Core;
using Rti.Dds.Core.Policy;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace PartitionsExample
{
    /// <summary>
    /// Example application that publishes PartitionsExample.HelloWorld.
    /// </summary>
    public static class HelloWorldSubscriber
    {
        /// <summary>
        /// Runs the subscriber example.
        /// </summary>
        public static async Task RunSubscriber(int domainId = 0, int sampleCount = int.MaxValue, bool useXmlQos = true)
        {
            // A DomainParticipant allows an application to begin communicating in
            // a DDS domain. Typically there is one DomainParticipant per application.
            // DomainParticipant QoS is configured in USER_QOS_PROFILES.xml
            //
            // A participant needs to be Disposed to release middleware resources.
            // The 'using' keyword indicates that it will be Disposed when this
            // scope ends.
            using DomainParticipant participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // A Topic has a name and a datatype.
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>("Example PartitionsExample_HelloWorld");

            // Configure the Subscriber's Partition QoS policy.
            SubscriberQos subscriberQos;
            if (useXmlQos)
            {
                // Retrieve the default Subscriber QoS, from USER_QOS_PROFILES.xml
                subscriberQos = QosProvider.Default.GetSubscriberQos();
            }
            else
            {
                subscriberQos = participant.DefaultSubscriberQos.WithPartition(p =>
                {
                    p.Add("ABC");
                    p.Add("X*Z");
                });
            }

            Subscriber subscriber = participant.CreateSubscriber(subscriberQos);

            Console.WriteLine(
                "Subscriber partition set to: " 
                + string.Join(separator: ",", values: subscriber.Qos.Partition.Name));

            DataReaderQos readerQos;
            if (useXmlQos)
            {
                // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
                readerQos = QosProvider.Default.GetDataReaderQos();
            }
            else
            {
                readerQos = subscriber.DefaultDataReaderQos
                    .WithReliability(policy => policy.Kind = ReliabilityKind.Reliable)
                    .WithHistory(policy => policy.Kind = HistoryKind.KeepAll)
                    .WithDurability(policy => policy.Kind = DurabilityKind.TransientLocal);
            }

            DataReader<HelloWorld> reader = subscriber.CreateDataReader(topic, readerQos);

            // Take all samples as they are received. Stop after taking sampleCount samples.
            await foreach (var sample in reader.TakeAsync().Take(sampleCount))
            {
                if (sample.Info.ValidData)
                {
                    if (sample.Info.State.View == ViewState.New)
                    {
                        // When we receive a new instance because a partition
                        // update made it possible, the View state is
                        // ViewState.New
                        Console.WriteLine("Found new instance");
                    }

                    Console.WriteLine(sample.Data);
                }
            }
        }
    }
} // namespace PartitionsExample
