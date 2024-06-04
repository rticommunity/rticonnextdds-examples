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
#include "temperature.hpp"

void publish_temperature(const std::string& sensor_name)
{
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<Temperature> topic(participant, "Temperature");
    dds::pub::DataWriter<Temperature> writer(topic);

    Temperature temp_reading;
    temp_reading.sensor_name(sensor_name);
    for (int i = 0; i < 1000; i++) {
        temp_reading.degrees((rand() % 10) + 30);
        writer.write(temp_reading);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int argc, char **argv)
{
    std::string sensor_name = (argc > 1) ? argv[1] : "Sensor1";
    publish_temperature(sensor_name);
}
