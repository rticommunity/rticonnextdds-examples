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
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace MarketDataExample
{
    /// <summary>
    /// Example application that publishes MarketDataExample.StockPrice.
    /// </summary>
    public static class StockPriceSubscriber
    {

        private static int receivedSamples = 0;

        private static void PrintData(DataReader<StockPrice> reader)
        {
            // Take all samples. Samples are loaned to application; loan is
            // returned when the samples collection is Disposed.
            using (var samples = reader.Take())
            {
                foreach (var data in samples.ValidData())
                {
                    Console.WriteLine(data);
                    receivedSamples++;
                }
            }
        }

        /// <summary>
        /// Runs the subscriber example.
        /// </summary>
        public static void RunSubscriber(
            int domainId = 0,
            int sampleCount = int.MaxValue)
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
            Topic<StockPrice> topic = participant.CreateTopic<StockPrice>("Example market_data");

            // A Subscriber allows an application to create one or more DataReaders
            // Subscriber QoS is configured in USER_QOS_PROFILES.xml
            Subscriber subscriber = participant.CreateSubscriber();

            // Create a ContentFiltered Topic and specify the STRINGMATCH filter name
            // to use a built-in filter for matching multiple strings.
            // More information can be found in the example
            var symbolFilter = new Filter(
                expression: "Symbol MATCH 'A'",
                parameters: Array.Empty<string>(),
                name: Filter.StringMatchFilterName);

            ContentFilteredTopic<StockPrice> filteredTopic =
                participant.CreateContentFilteredTopic(
                    name: "Filtered Example market_data",
                    relatedTopic: topic,
                    filter: symbolFilter);

            // Create a reader for the content-filtered topic, and set a
            // handler for the DataAvailable event that simply prints the data.
            DataReader<StockPrice> reader = subscriber.CreateDataReader(
                filteredTopic,
                preEnableAction: reader =>
                {
                    reader.DataAvailable += _ => PrintData(reader);
                });

            int t = 0;
            while (receivedSamples < sampleCount)
            {
                if (t == 3)
                {
                    // On t = 3 we add the symbol 'D' to the filter parameter
                    // to match 'A' and 'D'.
                    filteredTopic.AppendToExpressionParameter(0, "D");
                    Console.WriteLine("Changed filter to Symbol MATCH 'A,D'");
                }
                else if (t == 6)
                {
                    // On t = 6 we remove the symbol 'A' to the filter parameter
                    // to match only 'D'.
                    filteredTopic.RemoveFromExpressionParameter(0, "A");
                    Console.WriteLine("Changed filter to Symbol MATCH 'D'");
                }

                Thread.Sleep(4000);
                t++;
            }
        }
    }
} // namespace MarketDataExample
