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

int main(int argc, char **argv)
{
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");
    dds::topic::ContentFilteredTopic<DeviceStatus> content_filtered_topic(
            topic,
            "FilterRoomAndOpenWindows",
            dds::topic::Filter("is_open = true and room_name = 'LivingRoom'"));
    dds::sub::DataReader<DeviceStatus> reader(content_filtered_topic);

    rti::sub::SampleProcessor sample_processor;
    sample_processor.attach_reader(
            reader,
            [](const rti::sub::LoanedSample<DeviceStatus>& sample)
            {
                if (sample.info().valid()) { // ignore samples with only meta-data
                    std::cout << "WARNING: " << sample.data().sensor_name()
                            << " in " << sample.data().room_name()
                            << " is open!" << std::endl;
                }
            });

    while (true) { // wait in a loop
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
}
