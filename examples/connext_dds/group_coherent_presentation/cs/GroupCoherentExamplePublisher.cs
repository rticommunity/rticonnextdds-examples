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
using System.Threading.Tasks;
using Rti.Dds.Core;
using Rti.Dds.Core.Policy;
using Rti.Dds.Domain;
using Rti.Dds.Publication;

namespace GroupCoherentExample
{
    /// <summary>
    /// Example application that publishes Alarm, HeartRate and Temperature
    /// </summary>
    public sealed class PublisherApplication : IApplication
    {
        private static readonly uint[] heartRates 
            = new uint[] { 35, 56, 60, 70, 80, 85, 110 };
        private static readonly float[] temperatures 
            = new float[] { 95.0f, 98.1f, 98.2f, 98.3f, 99.2f, 101.7f };

        private readonly Random rnd = new Random();
        private readonly DomainParticipant participant;

        private readonly Publisher publisher;
        private readonly DataWriter<Alarm> alarmWriter;
        private readonly DataWriter<HeartRate> heartRateWriter;
        private readonly DataWriter<Temperature> temperatureWriter;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Publisher and DataWriters<Temperature>.
        /// </summary>
        public PublisherApplication(int domainId, bool useXmlQos = true)
        {
            // Start communicating in a domain, usually one participant per application
            // Load QoS profile from USER_QOS_PROFILES.xml file
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // Create the topics
            var alarmTopic = participant.CreateTopic<Alarm>("Alarm");
            var heartRateTopic = participant.CreateTopic<HeartRate>("HeartRate");
            var temperatureTopic = participant.CreateTopic<Temperature>("Temperature");

            // Create a Publisher
            PublisherQos publisherQos = null;
            if (useXmlQos)
            {
                // Retrieve the Publisher QoS, from USER_QOS_PROFILES.xml
                publisherQos = QosProvider.Default.GetPublisherQos();
            }
            else
            {
                // Set the Publisher QoS programatically, to the same effect
                publisherQos = participant.DefaultPublisherQos
                    .WithPresentation(policy =>
                    {
                        policy.AccessScope = PresentationAccessScopeKind.Group;
                        policy.CoherentAccess = true;
                        policy.OrderedAccess = true;
                    });
            }

            publisher = participant.CreatePublisher(publisherQos);

            // Create a DataWriter for each topic
            DataWriterQos writerQos = null;
            if (useXmlQos)
            {
                // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml
                writerQos = QosProvider.Default.GetDataWriterQos();
            }
            else
            {
                // Set the DataWriter QoS programatically, to the same effect
                writerQos = publisher.DefaultDataWriterQos
                    .WithReliability(policy => policy.Kind = ReliabilityKind.Reliable)
                    .WithHistory(policy => policy.Kind = HistoryKind.KeepAll);
            }

            alarmWriter = publisher.CreateDataWriter(alarmTopic, writerQos);
            heartRateWriter = publisher.CreateDataWriter(heartRateTopic, writerQos);
            temperatureWriter = publisher.CreateDataWriter(temperatureTopic, writerQos);
        }

        /// <summary>
        /// Write a coherent set any time the patient's vitals are
        /// abnormal. Otherwise, publish the patient's vitals normally.
        /// </summary>
        public async Task Run(int setCount, CancellationToken cancellationToken)
        {
            int patiendId = rnd.Next(1, 10000);
            Console.WriteLine($"Monitoring patient ID {patiendId}");

            var alarmData = new Alarm
            {
                patient_id = patiendId,
                alarm_code = AlarmCode.PATIENT_OK
            };
            var heartRateData = new HeartRate
            {
                patient_id = patiendId,
                beats_per_minute = 65
            };
            var temperatureData = new Temperature
            {
                patient_id = patiendId,
                temperature = 98.6f
            };

            for (int count = 0;
                count < setCount && !cancellationToken.IsCancellationRequested;
                count++)
            {
                // Modify the data to be sent here
                heartRateData.beats_per_minute = GetPatientHeartRate();
                temperatureData.temperature = GetPatientTemperature();

                if (heartRateData.beats_per_minute >= 100
                    || heartRateData.beats_per_minute <= 40
                    || temperatureData.temperature >= 100.0f
                    || temperatureData.temperature <= 95.0f)
                {
                    // Sound an alarm. In this case, we want all of the patients vitals
                    // along with the alarm to be delivered as a single coherent set of
                    // data so that we can correlate the alarm with the set of vitals
                    // that triggered it
                    using (publisher.BeginCoherentChanges())
                    {
                        alarmData.alarm_code = AlarmCode.ABNORMAL_READING;
                        alarmWriter.Write(alarmData);

                        // Simulate a delay in the publication of the data
                        await Task.Delay(100, cancellationToken);

                        heartRateWriter.Write(heartRateData);
                        temperatureWriter.Write(temperatureData);
                    } // end coherent set
                }
                else
                {
                    // No alarm necessary, publish the patient's vitals as normal
                    heartRateWriter.Write(heartRateData);

                    // Simulate a delay in the publication of the data
                    await Task.Delay(100, cancellationToken);

                    temperatureWriter.Write(temperatureData);
                }

                await Task.Delay(2000, cancellationToken);
            }
        }

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();

        private uint GetPatientHeartRate() => heartRates[rnd.Next(heartRates.Length)];
        private float GetPatientTemperature() => temperatures[rnd.Next(temperatures.Length)];
    }
} // namespace GroupCoherentExample
