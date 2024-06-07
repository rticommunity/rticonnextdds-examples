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

namespace HomeAutomation
{
    public class Program
    {
        public static async Task Main(string[] args)
        {
            string appKind = args.Length > 0 ? args[0] : "pub";

            if (appKind == "sub")
            {
                Console.WriteLine("Starting subscriber...");
                await Subscriber.MonitorSensors();
            }
            else if (appKind == "sub_timestamp")
            {
                Console.WriteLine("Starting subscriber with timestamp...");
                await SubscriberWithTimestamp.MonitorSensors();
            }
            else
            {
                string sensorName = args.Length > 1 ? args[1] : "Window1";
                string roomName = args.Length > 2 ? args[2] : "LivingRoom";

                Console.WriteLine($"Starting publisher for {sensorName} in {roomName}...");
                Publisher.PublishSensor(sensorName, roomName);
            }
        }
    }
}