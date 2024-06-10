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

#include "rti/rti.hpp"
#include "rti/sub/SampleProcessor.hpp"
#include "home_automation.hpp"

void wait_for_input(std::string query)
{
    std::cout << std::endl << std::endl
              << "Press Enter to " << query << std::endl;
    std::cin.get();
}

void print_data(const dds::sub::LoanedSamples<DeviceStatus> &samples)
{
    for (auto sample : samples) {
        if (sample.info().valid()) {
            std::cout << sample.data() << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");
    dds::sub::DataReader<DeviceStatus> reader(topic);

    wait_for_input("obtain all the samples in the cache.");
    print_data(reader.read());


    wait_for_input(
            "obtain all the samples with the attribute is_open set to true.");
    print_data(
            reader.select()
                    .content(dds::sub::Query(reader, "is_open = true"))
                    .read());


    wait_for_input(
            "obtain all the samples with the attribute sensor_name set to "
            "Window1.");
    print_data(
            reader.select()
                    .content(dds::sub::Query(reader, "sensor_name = 'Window1'"))
                    .read());


    wait_for_input(
            "obtain all the samples with the attribute room_name set to "
            "LivingRoom.");
    print_data(
            reader.select()
                    .content(dds::sub::Query(reader, "room_name = 'LivingRoom'"))
                    .read());


    wait_for_input(
            "obtain all the samples of the instance with the attribute "
            "sensor_name set to Window1.");
    dds::core::InstanceHandle window1_handle =
            reader.lookup_instance(DeviceStatus("Window1", "LivingRoom", true));
    print_data(reader.select().instance(window1_handle).read());


    wait_for_input("obtain all the samples that you have not read yet.");
    print_data(
            reader.select()
                    .state(dds::sub::status::SampleState::not_read())
                    .read());


    wait_for_input("obtain again, all the samples that you have not read yet.");
    print_data(
            reader.select()
                .state(dds::sub::status::SampleState::not_read())
                .read());


    wait_for_input("obtain the new instances.");
    print_data(
            reader.select()
                    .state(dds::sub::status::ViewState::new_view())
                    .read());


    wait_for_input(
            "obtain the sensor names of the instances that are not alive.");
    DeviceStatus key_holder;
    dds::sub::LoanedSamples<DeviceStatus> samples =
            reader.select()
                    .state(dds::sub::status::InstanceState::not_alive_mask())
                    .read();

    for (auto sample : samples) {
        reader.key_value(key_holder, sample.info().instance_handle());
        std::cout << key_holder.sensor_name() << std::endl;
    }
}
