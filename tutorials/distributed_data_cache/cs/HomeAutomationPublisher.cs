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

using System.Threading;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace HomeAutomation
{
    public class Publisher
    {
        public static void PublishSensor(string sensorName, string roomName)
        {
            DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId: 0);
            Topic<DeviceStatus> topic = participant.CreateTopic<DeviceStatus>("WindowStatus");
            DataWriter<DeviceStatus> writer =
                participant.ImplicitPublisher.CreateDataWriter(topic);

            // Create a DeviceStatus sample
            var deviceStatus = new DeviceStatus
            {
                sensor_name = sensorName,
                room_name = roomName,
                is_open = false
            };

            for (int i = 0; i < 20; i++)
            {
                // Simulate the window opening and closing
                deviceStatus.is_open = !deviceStatus.is_open;
                writer.Write(deviceStatus);
                Thread.Sleep(2000); // Sleep for 2 seconds
            }
        }
    }
}
