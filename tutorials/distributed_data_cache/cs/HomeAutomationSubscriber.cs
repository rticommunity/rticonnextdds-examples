//
// (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
//
//  RTI grants Licensee a license to use, modify, compile, and create derivative
//  works of the Software.  Licensee has the right to distribute object form
//  only for use with RTI products.  The Software is provided "as is", with no
//  warranty of any type, including any warranty for fitness for any purpose.
//  RTI is under no obligation to maintain or support the Software.  RTI shall
//  not be liable for any incidental or consequential damages arising out of the
//  use or inability to use the software.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using Omg.Dds.Subscription;
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace HomeAutomation
{
    public class Subscriber
    {
        private static void WaitForInput(string query)
        {
            Console.WriteLine($"\nPress Enter to {query}");
            Console.ReadLine();
        }

        private static void PrintData(LoanedSamples<DeviceStatus> samples)
        {
            foreach (var sample in samples.ValidData())
            {
                Console.WriteLine($"Sample:\n{sample}");
            }
        }

        public static void MonitorSensors()
        {
            using DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId: 0);

            Topic<DeviceStatus> topic =
                participant.CreateTopic<DeviceStatus>("WindowStatus");

            DataReader<DeviceStatus> reader =
                participant.ImplicitSubscriber.CreateDataReader(topic);

            WaitForInput("obtain all the samples in the cache.");
            using (var samples = reader.Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain all the samples with the attribute is_open set to true.");
            using (var filterCond = reader.CreateQueryCondition("is_open = true"))
            using (var samples = reader.Select().WithCondition(filterCond).Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain all the samples with the attribute sensor_name set to Window1.");
            using (var filterCond = reader.CreateQueryCondition("sensor_name = 'Window1'"))
            using (var samples = reader.Select().WithCondition(filterCond).Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain all the samples with the attribute room_name set to LivingRoom.");
            using (var filterCond = reader.CreateQueryCondition("room_name = 'LivingRoom'"))
            using (var samples = reader.Select().WithCondition(filterCond).Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain all the samples of the instance with the attribute " +
                        "sensor_name set to Window1.");
            var instanceHandle = reader.LookupInstance(new DeviceStatus { sensor_name = "Window1"});
            using (var samples = reader.Select().WithInstance(instanceHandle).Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain all the samples that you have not read yet.");
            using (var samples = reader.Select().WithState(SampleState.NotRead).Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain again, all the samples that you have not read yet.");
            using (var samples = reader.Select().WithState(SampleState.NotRead).Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain the new instances.");
            using (var samples = reader.Select().WithState(ViewState.New).Read())
            {
                PrintData(samples);
            }

            WaitForInput("obtain the sensor names of the instances that are not alive.");
            using (var samples = reader.Select().WithState(InstanceState.NotAlive).Read())
            {
                foreach (var sample in samples)
                {
                    var keyHolder = reader.GetKeyValue(new DeviceStatus(), sample.Info.InstanceHandle);
                    Console.WriteLine($"Sensor name: {keyHolder.sensor_name}");
                }
            }
        }
    }
}
