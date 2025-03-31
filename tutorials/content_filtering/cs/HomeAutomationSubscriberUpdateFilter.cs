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
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;

namespace HomeAutomation
{
    public class SubscriberUpdateFilter
    {
        public static async Task MonitorSensors()
        {
            using DomainParticipant participant =
                DomainParticipantFactory.Instance.CreateParticipant(domainId: 0);

            Topic<DeviceStatus> topic =
                participant.CreateTopic<DeviceStatus>("WindowStatus");

            List<String> parameters = new List<String>();
            parameters.Add("'LivingRoom'");

            ContentFilteredTopic<DeviceStatus> contentFilteredTopic = 
                participant.CreateContentFilteredTopic<DeviceStatus>(
                    "FilterRoomAndOpenWindows",
                    topic,
                    new Filter("is_open = true and room_name = %0", parameters));

            DataReader<DeviceStatus> reader = 
                participant.ImplicitSubscriber.CreateDataReader(contentFilteredTopic);

            parameters[0] = "'Kitchen'";
            contentFilteredTopic.FilterParameters = parameters;

            await foreach (var data in reader.TakeAsync().ValidData())
            {
                Console.WriteLine($"WARNING: {data.sensor_name} in {data.room_name} is open!");
            }
        }
    }
}