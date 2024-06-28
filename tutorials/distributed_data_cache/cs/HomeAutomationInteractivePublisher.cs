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
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;

namespace HomeAutomation
{

    public class WindowSensor
    {
        private DeviceStatus status;
        private DataWriter<DeviceStatus> writer;
        private InstanceHandle instance;

        public WindowSensor(string sensorName, string roomName)
        {
            status = new DeviceStatus
            {
                sensor_name = sensorName,
                room_name = roomName,
                is_open = false
            };

            DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId: 0);
            Topic<DeviceStatus> topic = participant.CreateTopic<DeviceStatus>("WindowStatus");
            writer = participant.ImplicitPublisher.CreateDataWriter(topic);

            instance = writer.RegisterInstance(status);
            writer.Write(status, instance);
        }

        public void OpenWindow()
        {
            Console.WriteLine("Opening the window...");
            status.is_open = true;
            writer.Write(status, instance);
        }

        public void CloseWindow()
        {
            Console.WriteLine("Closing the window...");
            status.is_open = false;
            writer.Write(status, instance);
        }

        public void TurnOff()
        {
            Console.WriteLine("Turning off the sensor...");
            writer.DisposeInstance(instance);
        }

        public void TurnOn()
        {
            Console.WriteLine("Turning on the sensor...");
            writer.Write(status, instance);
        }
    }

    public class InteractivePublisher
    {
        public static void ControlSensor(string sensorName, string roomName)
        {
            WindowSensor sensor = new WindowSensor(sensorName, roomName);

            while (true)
            {
                Console.WriteLine("\nEnter one of the following options:");
                Console.WriteLine("\t- open");
                Console.WriteLine("\t- close");
                Console.WriteLine("\t- off");
                Console.WriteLine("\t- on");
                Console.WriteLine("\t- exit");

                string option = Console.ReadLine().ToLower();

                switch (option)
                {
                    case "open":
                        sensor.OpenWindow();
                        break;
                    case "close":
                        sensor.CloseWindow();
                        break;
                    case "off":
                        sensor.TurnOff();
                        break;
                    case "on":
                        sensor.TurnOn();
                        break;
                    case "exit":
                        return;
                    default:
                        Console.WriteLine(
                            "Invalid option, please enter a valid option [open|close|off|on|exit]."
                        );
                        break;
                }
            }
        }
    }
}
