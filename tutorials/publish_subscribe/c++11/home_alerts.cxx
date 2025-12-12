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

using KeyedString = dds::core::KeyedStringTopicType;

// This version turns the subscriber into an alert service that publishes
// notifications to a HomeAlerts topic using the built-in KeyedString type.
//
// You can subscribe to this new topic by running the following command in a
// terminal:
//
// $ rtiddsspy -printSample COMPACT -topic HomeAlerts
//
int main(int argc, char **argv)
{

    std::cout << "Starting home alert service..." << std::endl;
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<DeviceStatus> status_topic(participant, "WindowStatus");
    dds::sub::DataReader<DeviceStatus> status_reader(status_topic);

    // Create the HomeAlerts topic and DataWriter to publish alert messages on
    // the same domain.
    dds::topic::Topic<KeyedString> alert_topic(participant, "HomeAlerts");
    dds::pub::DataWriter<KeyedString> alert_writer(alert_topic);

    // Publish an alert when a window opens, keeping the original source
    // timestamp from the WindowStatus update.
    rti::sub::SampleProcessor sample_processor;
    sample_processor.attach_reader(
            status_reader,
            [alert_writer](const rti::sub::LoanedSample<DeviceStatus>
                                   &sample) mutable {
                if (!sample.info().valid() || !sample.data().is_open) {
                    return;
                }

                alert_writer.write(
                        KeyedString(
                                sample.data().sensor_name,
                                "Window in " + sample.data().room_name
                                        + " was just opened"),
                        sample.info().source_timestamp());
            });

    while (true) {  // wait in a loop
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
}
