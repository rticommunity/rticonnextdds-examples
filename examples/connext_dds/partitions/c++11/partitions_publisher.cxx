/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <iostream>
#include <thread>
#include <rti/rti.hpp>
#include "home_automation.hpp"

void publish_sensor(
    const std::string& sensor_name,
    const std::string& room_name)
{
    dds::domain::DomainParticipant participant(0);
    dds::pub::Publisher publisher(participant);
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");
    dds::pub::DataWriter<DeviceStatus> writer(publisher, topic);

    dds::pub::qos::PublisherQos publisher_qos =
            dds::core::QosProvider::Default().publisher_qos();
    auto &partition = publisher_qos.policy<dds::core::policy::Partition>();
    std::vector<std::string> partition_names = {};

    DeviceStatus device_status { sensor_name, room_name, false };
    for (int i = 0; i < 1000; i++) {
        if (i % 15  == 0) {
            partition_names = {"USA/CA/Sunnyvale", "USA/CA/San Francisco"};
        } else if (i % 15 == 5) {
            partition_names = {"USA/CA/*"};
        } else if (i % 15 == 10) {
            partition_names = {"USA/NV/Las Vegas"};
        }

        if (!partition_names.empty()) {
            partition.name(partition_names);
            publisher.qos(publisher_qos << partition);
            partition_names.clear();
        }

        device_status.is_open(!device_status.is_open());

        std::cout << "Writing " << device_status << " on partition(s) ";
        for (const auto &name : publisher.qos().policy<dds::core::policy::Partition>().name()) {
            std::cout << "'" << name << "' ";
        }
        std::cout << std::endl;

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
