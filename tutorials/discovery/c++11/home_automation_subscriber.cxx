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

class SensorListener
        : public dds::sub::NoOpDataReaderListener<DeviceStatus> {

    void on_subscription_matched(
            dds::sub::DataReader<DeviceStatus> &reader,
            const dds::core::status::SubscriptionMatchedStatus &status) override
    {
        std::cout << std::endl << "Total publishers: " << status.current_count()
                << ", Change: " << status.current_count_change()
                << std::endl;

        dds::core::InstanceHandleSeq publications =
                dds::sub::matched_publications(reader);

        for (int i = 0; i < publications.size(); i++) {
            dds::topic::PublicationBuiltinTopicData pub_data =
                    dds::sub::matched_publication_data(reader, publications[i]);

            std::cout << "Publisher " << i << ": " << std::endl;
            std::cout << "    Publisher Virtual GUID: "
                    << pub_data.extensions().virtual_guid() << std::endl;
            std::cout << "    Publisher Name: "
                    << (pub_data.extensions().publication_name().name()
                            ? pub_data->publication_name().name().value()
                            : "(N/A)") << std::endl;
        }
    }
};

int main(int argc, char **argv)
{
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<DeviceStatus> topic(participant, "WindowStatus");
    dds::sub::DataReader<DeviceStatus> reader(topic);
    std::shared_ptr<SensorListener> sensor_listener =
            std::make_shared<SensorListener>();

    reader.set_listener(
            sensor_listener,
            dds::core::status::StatusMask::subscription_matched());

    rti::sub::SampleProcessor sample_processor;
    sample_processor.attach_reader(
            reader,
            [](const rti::sub::LoanedSample<DeviceStatus>& sample)
            {
                if (sample.info().valid()) { // ignore samples with only meta-data
                    if (sample.data().is_open()) {
                        std::cout << "WARNING: " << sample.data().sensor_name()
                                << " in " << sample.data().room_name()
                                << " is open!" << std::endl;
                    }
                }
            });

    while (true) { // wait in a loop
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
}
