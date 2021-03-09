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

using System.Threading;
using Rti.Dds.Core;
using Rti.Dds.Core.Policy;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace MarketDataExample
{
    /// <summary>
    /// Example application that publishes MarketDataExample.StockPrice.
    /// </summary>
    public static class StockPricePublisher
    {
        /// <summary>
        /// Runs the publisher example.
        /// </summary>
        public static void RunPublisher(int domainId, int sampleCount, bool useXmlConfig = true)
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

            // A Publisher allows an application to create one or more DataWriters
            // Publisher QoS is configured in USER_QOS_PROFILES.xml
            Publisher publisher = participant.CreatePublisher();

            DataWriterQos writerQos;
            if (useXmlConfig)
            {
                // Gets the default DataWriterQos defined in USER_QOS_PROFILES.xml
                writerQos = QosProvider.Default.GetDataWriterQos();
            }
            else
            {
                // Creates the DataWriterQos programmatically to the same effect
                // as the one defined in XML.
                writerQos = publisher.DefaultDataWriterQos.WithMultiChannel(
                    CreateMultiChannelQosPolicy());
            }

            DataWriter<StockPrice> writer = publisher.CreateDataWriter(topic, writerQos);

            var sample = new StockPrice();
            for (int count = 0; count < sampleCount; count++)
            {
                // Update the symbol and price
                char symbol = (char) ('A' + (count % 26));
                sample.Symbol = symbol.ToString();
                sample.Price = count;

                writer.Write(sample);

                Thread.Sleep(100);
            }
        }

        private static MultiChannel CreateMultiChannelQosPolicy()
            => MultiChannel.Default.With(builder =>
            {
                // The QoS policies are immutable types that can be built from an existing
                // value, using a "With" modifier that returns a new object with the
                // changes specified in this lambda function.
                builder.Channels.Add(CreateChannel("239.255.0.2", "Symbol MATCH '[A-C]*'"));
                builder.Channels.Add(CreateChannel("239.255.0.3", "Symbol MATCH '[D-F]*'"));
                builder.Channels.Add(CreateChannel("239.255.0.4", "Symbol MATCH '[G-I]*'"));
                builder.Channels.Add(CreateChannel("239.255.0.5", "Symbol MATCH '[J-L]*'"));
                builder.Channels.Add(CreateChannel("239.255.0.6", "Symbol MATCH '[M-O]*'"));
                builder.Channels.Add(CreateChannel("239.255.0.7", "Symbol MATCH '[P-S]*'"));
                builder.Channels.Add(CreateChannel("239.255.0.8", "Symbol MATCH '[T-V]*'"));
                builder.Channels.Add(CreateChannel("239.255.0.9", "Symbol MATCH '[W-Z]*'"));
            });

        private static ChannelSettings CreateChannel(string ipAddress, string filterExpression)
            => ChannelSettings.Default.With(builder =>
            {
                builder.MulticastSettings.Add(CreateTransportMulticastSettings(ipAddress));
                builder.FilterExpression = filterExpression;
                builder.Priority = Priority.Undefined;
            });

        private static TransportMulticastSettings CreateTransportMulticastSettings(string ipAddress)
            => TransportMulticastSettings.Default.With(builder =>
            {
                builder.Transports.Clear(); // Use all available transports
                builder.ReceiveAddress = ipAddress;
                builder.ReceivePort = 0; // Port determined automatically
            });
    }
} // namespace MarketDataExample
