/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <cstdlib>
#include <iostream>

#include <dds/dds.hpp>
#include "waitset_statuscond.hpp"

using namespace dds::core;
using namespace dds::core::cond;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant.
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<Foo> topic(participant, "Example waitset_statuscond");

    // Retrieve the subscriber QoS from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather
    // than using the XML file, you will need to comment out the previous
    // publisher_qos assignment and uncomment these files.

    // reader_qos << Reliability::Reliable()
    //            << History::KeepAll();

    // Create a DataReader.
    DataReader<Foo> reader(Subscriber(participant), topic, reader_qos);

    // Get status conditions.
    // Each entity may have an attached Status Condition. To modify the
    // statuses we need to get the reader's Status Conditions first.
    // By instantiating a StatusCondition we are obtaining a reference to this
    // reader's StatusCondition
    StatusCondition status_condition(reader);

    // Set enabled statuses.
    // Now that we have the Status Condition, we are going to enable the
    // statuses we are interested in:
    status_condition.enabled_statuses(StatusMask::data_available());

     // Create the waitset and attach the condition.
     WaitSet waitset;
     waitset += status_condition;

     // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        // 'wait()' blocks execution of the thread until one or more attached
        // Conditions become true, or until a user-specified timeout expires.
        // Another way would be to use 'dispatch()' to wait and dispatch the
        // events as it's done in the "waitset_query_cond" example.
        WaitSet::ConditionSeq active_conditions = waitset.wait(
            Duration::from_millisecs(1500));

        if (active_conditions.size() == 0) {
            std::cout << "Wait timed out!! No conditions were triggered."
                      << std::endl;
            continue;
        }

        std::cout << "Got " << active_conditions.size() << " active conditions"
                  << std::endl;

        // In this case, we have only a single condition, but if we had
        // multiple, we would need to iterate over them and check which one is
        // true. Leaving the logic for the more complex case.
        for (std::vector<int>::size_type i=0; i<active_conditions.size(); i++) {
            // Compare with Status Condition
            if (active_conditions[i] == status_condition) {
                // Get the status changes so we can check witch status condition
                // triggered.
                StatusMask triggeredmask = reader.status_changes();

                // Subscription matched
                if ((triggeredmask & StatusMask::data_available()).any()){
                    // Current conditions match our conditions to read data, so
                    // we can read data just like we would do in any other
                    // example.
                    LoanedSamples<Foo> samples = reader.take();
                    for (LoanedSamples<Foo>::iterator sampleIt=samples.begin();
                        sampleIt != samples.end();
                        ++sampleIt) {

                        if (!sampleIt->info().valid()) {
                            std::cout << "Got metadata" << std::endl;
                        } else {
                            std::cout << sampleIt->data() << std::endl;
                        }
                    }
                }
            }
        }

    }
}

int main(int argc, char *argv[])
{
    int domain_id    = 0;
    int sample_count = 0; // Infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
