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
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace PartitionsExample
{
    /// <summary>
    /// Example application that publishes PartitionsExample.HelloWorld.
    /// </summary>
    public static class HelloWorldPublisher
    {
        /// <summary>
        /// Runs the publisher example.
        /// </summary>
        public static void RunPublisher(int domainId, int sampleCount)
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
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>("Example partitions");

            // A Publisher allows an application to create one or more DataWriters
            // Publisher QoS is configured in USER_QOS_PROFILES.xml
            Publisher publisher = participant.CreatePublisher();

            // This DataWriter will write data on Topic "Example HelloWorld"
            // DataWriter QoS is configured in USER_QOS_PROFILES.xml
            DataWriter<HelloWorld> writer = publisher.CreateDataWriter(topic);

            var sample = new HelloWorld();
            for (int count = 0; count < sampleCount; count++)
            {
                // Modify the data to be sent here
                sample.x = count;

                Console.WriteLine($"Writing HelloWorld, count {count}");

                writer.Write(sample);

                // Every 5 samples we will change the partition name.
                // These are the partition expressions we are going to try:
                // "bar", "A*", "A?C", "X*Z", "zzz" and "A*C".
                string[] newPartitions = null;
                if ((count + 1) % 25 == 0)
                {
                    // Matches "ABC", name[1] here can match name[0] there,
                    // as long as there is some overlapping name.
                    newPartitions = new string[] { "zzz", "A*C" };
                }
                else if ((count + 1) % 25 == 20)
                {
                    // Strings that are regular expressions aren't tested for
                    // literal matches, so this won't match "X*Z".
                    newPartitions = new string[] { "X*Z" };
                }
                else if ((count + 1) % 25 == 15)
                {
                    // Matches "ABC".
                    newPartitions = new string[] { "A?C" };
                }
                else if ((count + 1) % 25 == 10)
                {
                    // Matches "ABC".
                    newPartitions = new string[] { "A*" };
                }
                else if ((count + 1) % 25 == 5)
                {
                    // No literal match for "bar".
                    // For the next iterations we are using only one partition.
                    newPartitions = new string[] { "bar" };
                }

                if (newPartitions != null)
                {
                    // Update the publisher QoS with the new partitions
                    publisher.Qos = publisher.Qos.WithPartition(currentPartition =>
                    {
                        currentPartition.Clear(); // remove previous partitions
                        currentPartition.AddRange(newPartitions); // add new partitions
                    });

                    Console.WriteLine(
                        "Publisher partition set to: "
                        + string.Join(separator: ",", values: publisher.Qos.Partition.Name));
                }

                Thread.Sleep(1000);
            }
        }
    }
} // namespace PartitionsExample
