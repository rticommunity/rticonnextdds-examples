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

    // Create a DomainParticipant and WindowStatus topic using the same domain
    // ID and name as the publisher.
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");

    // Create a DataReader to receive the WindowStatus data.
    dds::sub::DataReader<DeviceStatus> reader(topic);

    // Take each update with its metadata so we can include the source timestamp
    // when the window was opened.
    rti::sub::SampleProcessor sample_processor;
    sample_processor.attach_reader(
            reader,
            [](const rti::sub::LoanedSample<DeviceStatus> &sample) {
                if (!sample.info().valid()) {
                    // ignore samples with only meta-data
                    return;
                }

                if (sample.data().is_open) {
                    // Include the original source timestamp of the WindowStatus
                    // update in the alert output.
                    uint64_t timestamp =
                            sample.info().source_timestamp().to_millisecs();
                    std::cout << "WARNING: " << sample.data().sensor_name
                              << " in " << sample.data().room_name
                              << " is open "
                              << "(" << std::fixed << std::setprecision(2)
                              << timestamp / 1000.0 << "s)" << std::endl;
                }
            });

    while (true) {  // wait in a loop
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
}
