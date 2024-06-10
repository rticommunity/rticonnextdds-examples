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
#include <set>

#include "rti/rti.hpp"
#include "rti/sub/SampleProcessor.hpp"
#include "home_automation.hpp"

int main(int argc, char **argv)
{
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");
    dds::sub::DataReader<DeviceStatus> reader(topic);
    dds::sub::LoanedSamples<DeviceStatus> samples {};


    std::set<std::string> disposed_instances;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(4));
        samples = reader.select().state(dds::sub::status::InstanceState::not_alive_disposed()).read();
        std::for_each(samples.begin(),
                samples.end(),
                [&disposed_instances, &reader](const rti::sub::LoanedSample<DeviceStatus>& sample)
                {
                    DeviceStatus key_holder;
                    reader.key_value(key_holder, sample.info().instance_handle());
                    disposed_instances.insert(key_holder.sensor_name());
                });

        std::cout << "Disposed instances: " << std::endl;
        if (disposed_instances.empty()) {
            std::cout << "\tNone" << std::endl;
        } else {
            for (const auto& sensor_name : disposed_instances) {
                std::cout << "\t" << sensor_name << std::endl;
            }
        }
        disposed_instances.clear();
    }
}
