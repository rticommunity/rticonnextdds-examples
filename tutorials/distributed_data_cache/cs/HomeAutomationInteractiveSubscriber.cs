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
using System.Collections.Generic;
using Omg.Dds.Subscription;
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace HomeAutomation
{
    public class InteractiveSubscriber
    {
        public static void MonitorSensors()
        {
            using DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId: 0);

            Topic<DeviceStatus> topic =
                participant.CreateTopic<DeviceStatus>("WindowStatus");

            DataReader<DeviceStatus> reader =
                participant.ImplicitSubscriber.CreateDataReader(topic);

            HashSet<string> disposedInstances = new HashSet<string>();

            while (true)
            {
                Thread.Sleep(4000);
                using var samples = reader.Select().WithState(InstanceState.NotAliveDisposed).Read();

                foreach (var sample in samples)
                {
                    var keyHolder = reader.GetKeyValue(new DeviceStatus(), sample.Info.InstanceHandle);
                    disposedInstances.Add(keyHolder.sensor_name);
                }

                Console.WriteLine("Disposed instances:");
                if (disposedInstances.Count == 0)
                {
                    Console.WriteLine("\tNone");
                }
                else
                {
                    foreach (var instance in disposedInstances)
                    {
                        Console.WriteLine($"\t{instance}");
                    }
                    disposedInstances.Clear();
                }
            }
        }
    }
}
