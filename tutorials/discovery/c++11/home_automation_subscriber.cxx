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
    dds::sub::DataReader<DeviceStatus> reader(topic);

    dds::core::cond::StatusCondition status_cond(reader);
    status_cond.enabled_statuses(dds::core::status::StatusMask::subscription_matched());
    status_cond->handler([&reader](dds::core::cond::Condition c)
            {
                dds::core::status::SubscriptionMatchedStatus status =
                        reader.subscription_matched_status();
                if (status.current_count_change() > 0) {
                    std::cout << "Publisher matched ";
                } else {
                    std::cout << "Publisher unmatched ";
                }
                std::cout << "(Total " << status.current_count() << ")" << std::endl;

                dds::core::InstanceHandleSeq publications =
                        dds::sub::matched_publications(reader);

                std::cout << "Matched Publishers = [ ";
                for (int i = 0; i < publications.size(); i++) {
                    dds::topic::PublicationBuiltinTopicData publication_data =
                            dds::sub::matched_publication_data(reader, publications[i]);

                    if (publication_data->publication_name().name()) {
                        std::cout << publication_data->publication_name().name().value() << " ";
                    }
                }
                std::cout << "]" << std::endl << std::endl;
            });

    dds::core::cond::WaitSet waitset;
    waitset += status_cond;

    while (true) { // wait in a loop
        waitset.dispatch(dds::core::Duration(4));
    }
}
