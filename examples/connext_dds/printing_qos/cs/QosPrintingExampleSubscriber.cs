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
using Omg.Dds.Core;
using Rti.Dds.Core;
using Rti.Dds.Core.Status;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace QosPrintingExample
{
    /// <summary>
    /// Example application that publishes QosPrintingExample.HelloWorld.
    /// </summary>
    public static class HelloWorldSubscriber
    {
        private static int ProcessData(DataReader<HelloWorld> reader)
        {
            // Take all samples. Samples are loaned to application; loan is
            // returned when the samples collection is Disposed.
            int samplesRead = 0;
            using (var samples = reader.Take())
            {
                foreach (var sample in samples)
                {
                    if (sample.Info.ValidData)
                    {
                        Console.WriteLine(sample.Data);
                        samplesRead++;
                    }
                    else
                    {
                        Console.WriteLine($"Received instance update: {sample.Info.State.Instance}");
                    }
                }
            }

            return samplesRead;
        }

        /// <summary>
        /// Runs the subscriber example.
        /// </summary>
        public static void RunSubscriber(int domainId = 0, int sampleCount = int.MaxValue)
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
            Topic<HelloWorld> topic = participant.CreateTopic<HelloWorld>("Example QosPrintingExample_HelloWorld");

            // A Subscriber allows an application to create one or more DataReaders
            // Subscriber QoS is configured in USER_QOS_PROFILES.xml
            Subscriber subscriber = participant.CreateSubscriber();

            // This DataReader reads data on Topic "Example HelloWorld".
            // DataReader QoS is configured in USER_QOS_PROFILES.xml
            DataReader<HelloWorld> reader = subscriber.CreateDataReader(topic);

            // --- QosPrintingExample ---

            // Qos objects override ToString so can be printed directly
            Console.WriteLine(subscriber.Qos);

            // The output can be configured using QosPrintFormat
            QosPrintFormat format = new QosPrintFormat
            {
                IsStandalone = true
            };
            Console.WriteLine(subscriber.Qos.ToString(format));

            // By default, only the differences with respect to the documented
            // QoS default (the default values as stated in the API reference)
            // are printed. This behavior can be overridden by passing another
            // QoS object to use as the baseQos to the ToString
            var readerQos = reader.Qos.WithSubscriptionName(n => n.Name = "Other QoS");
            Console.WriteLine(reader.Qos.ToString(baseQos: readerQos));
            // Alternatively, pass QosPrintAll.Value as the baseQos to print the entire QoS object
            Console.WriteLine(reader.Qos.ToString(baseQos: QosPrintAll.Value));

            // --- QosPrintingExample ---

            // Obtain the DataReader's Status Condition
            StatusCondition statusCondition = reader.StatusCondition;

            // Enable the 'data available' status.
            statusCondition.EnabledStatuses = StatusMask.DataAvailable;

            // Associate an event handler with the status condition.
            // This will run when the condition is triggered, in the context of
            // the dispatch call (see below)
            int samplesRead = 0;
            statusCondition.Triggered += _ => samplesRead += ProcessData(reader);

            // Create a WaitSet and attach the StatusCondition
            var waitset = new WaitSet();
            waitset.AttachCondition(statusCondition);
            while (samplesRead < sampleCount)
            {
                // Dispatch will call the handlers associated to the WaitSet
                // conditions when they activate
                Console.WriteLine("HelloWorld subscriber sleeping for 4 sec...");
                waitset.Dispatch(Duration.FromSeconds(4));
            }
        }
    }
} // namespace QosPrintingExample
