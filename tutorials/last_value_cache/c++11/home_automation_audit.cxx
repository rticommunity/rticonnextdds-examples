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
    dds::sub::DataReader<DeviceStatus> reader(
            topic,
            dds::core::QosProvider::Default().datareader_qos("Last10Cache"));

    rti::sub::SampleProcessor sample_processor;
    sample_processor.attach_reader(
            reader,
            [](const rti::sub::LoanedSample<DeviceStatus>& sample)
            {
                if (!sample.info().valid()) {
                    // ignore samples with only meta-data
                    return;
                }

                uint64_t timestamp =
                        sample.info().source_timestamp().to_millisecs();
                std::cout << std::fixed << std::setprecision(2)
                        << timestamp / 1000.0 << " - "
                        << sample.data() << std::endl;
            });

    while (true) { // wait in a loop
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
}
