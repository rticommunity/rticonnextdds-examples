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
                std::cout << std::endl << "Total publishers: " << status.current_count()
                        << ", Change: " << status.current_count_change()
                        << std::endl;

                dds::core::InstanceHandleSeq pub_handles =
                        dds::sub::matched_publications(reader);

                for (int i = 0; i < pub_handles.size(); i++) {
                    dds::topic::PublicationBuiltinTopicData builtin_data =
                            dds::sub::matched_publication_data(reader, pub_handles[i]);

                    std::cout << "Publisher " << i << ": " << std::endl;
                    std::cout << "    Publisher Virtual GUID: "
                            << builtin_data->virtual_guid() << std::endl;
                    std::cout << "    Publisher Topic: "
                            << builtin_data->topic_name() << std::endl;
                    std::cout << "    Publisher Type: "
                            << builtin_data->type_name() << std::endl;
                    std::cout << "    Publisher Name: "
                            << (builtin_data->publication_name().name()
                                    ? builtin_data->publication_name().name().value()
                                    : "Null") << std::endl;
                }
            });

    dds::core::cond::WaitSet wait_set;
    wait_set += status_cond;

    while (true) { // wait in a loop
        wait_set.dispatch(dds::core::Duration(4));
    }
}
