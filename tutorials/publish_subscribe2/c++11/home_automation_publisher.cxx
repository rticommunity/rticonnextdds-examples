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

void publish_sensor(
    const std::string& sensor_name,
    const std::string& room_name)
{
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");
    dds::pub::DataWriter<DeviceStatus> writer(topic);

    DeviceStatus device_status { sensor_name, room_name, false };
    for (int i = 0; i < 1000; i++) {
        device_status.is_open(!device_status.is_open());
        writer.write(device_status);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int main(int argc, char **argv)
{
    std::string sensor_name = (argc > 1) ? argv[1] : "Sensor1";
    std::string room_name = (argc > 2) ? argv[2] : "LivingRoom";
    publish_sensor(sensor_name, room_name);
}
