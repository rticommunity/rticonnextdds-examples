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

#include <iostream>
#include <thread>
#include <rti/rti.hpp>
#include "home_automation.hpp"

// This publisher simulates a window that opens and closes over time on the
// WindowStatus topic.
void publish_sensor(
        const std::string &sensor_name,
        const std::string &room_name)
{
    // Create a DomainParticipant within a domain ID. A domain ID is a logical
    // partition for your applications; the DomainParticipant joins that domain
    // and contains all other entities.
    dds::domain::DomainParticipant participant(0);

    // The data shared by a publisher and subscriber is described by a Topic.
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");

    // Create a DataWriter to publish the WindowStatus topic.
    dds::pub::DataWriter<DeviceStatus> writer(topic);

    DeviceStatus device_status { sensor_name, room_name, false };

    // Publish an update every two seconds, simulating the window opening and
    // closing repeatedly.
    for (int i = 0; i < 1000; i++) {
        device_status.is_open = !device_status.is_open;
        writer.write(device_status);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int main(int argc, char **argv)
{
    std::string sensor_name = (argc > 1) ? argv[1] : "Window1";
    std::string room_name = (argc > 2) ? argv[2] : "LivingRoom";
    publish_sensor(sensor_name, room_name);
}
