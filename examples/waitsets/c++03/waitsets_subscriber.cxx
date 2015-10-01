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

#include <iostream>
#include <cstdlib>

#include <dds/dds.hpp>
#include "waitsets.hpp"

using namespace dds::core;
using namespace dds::core::cond;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::cond;
using namespace dds::sub::qos;
using namespace dds::sub::status;

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default QoS.
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<waitsets> topic(participant, "Example waitsets");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataWriter QoS programmatically rather
    // than using the XML file, you will need to uncomment these lines.

    // reader_qos << Reliability::Reliable()
    //            << History::KeepAll()
    //            << Liveliness::Automatic().lease_duration(Duration(2));

    // Create a DataReader.
    DataReader<waitsets> reader(Subscriber(participant), topic, reader_qos);

    // Create a Read Condition.
    // Note that the Read Conditions are dependent on both incoming data as
    // well as sample state.
    ReadCondition read_cond(
        reader, DataState(
            SampleState::not_read(),
            ViewState::any(),
            InstanceState::any()));

    // Get status conditions.
    // Each entity may have an attached Status Condition. To modify the
    // statuses we need to get the reader's Status Conditions first.
    StatusCondition status_condition(reader);

    // Set enabled statuses.
    // Now that we have the Status Condition, we are going to enable the
    // statuses we are interested in:
    status_condition.enabled_statuses(
        StatusMask::subscription_matched() | StatusMask::liveliness_changed());

    // Create and attach conditions to the WaitSet
    // Finally, we create the WaitSet and attach both the Read Conditions and
    // the Status Condition to it. We can use the operator '+=' or the method
    // 'attach_condition'.
    WaitSet waitset;
    waitset += read_cond;
    waitset.attach_condition(status_condition);

    // Main loop.
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        // 'wait()' blocks execution of the thread until one or more attached
        // Conditions become true, or until a user-specified timeout expires.
        WaitSet::ConditionSeq active_conditions = waitset.wait(
            Duration::from_millisecs(1500));

        if (active_conditions.size() == 0) {
            std::cout << "Wait timed out!! No conditions were triggered."
                      << std::endl;
            continue;
        }

        std::cout << "Got " << active_conditions.size() << " active conditions"
                  << std::endl;
        for (std::vector<int>::size_type i=0; i<active_conditions.size(); i++) {
            // Now we compare the current condition with the Status Conditions
            // and the Read Conditions previously defined. If they match, we
            // print the condition that was triggered.

            if (active_conditions[i] == status_condition) {
                // Get the status changes so we can check witch status condition
                // triggered.
                StatusMask status_mask = reader.status_changes();

                // Liveliness changed
                if ((status_mask & StatusMask::liveliness_changed()).any()) {
                    LivelinessChangedStatus st =
                        reader.liveliness_changed_status();
                    std::cout << "Liveliness changed => Active writers = "
                              << st.alive_count() << std::endl;
                }

                // Subscription matched
                if ((status_mask & StatusMask::subscription_matched()).any()) {
                    SubscriptionMatchedStatus st =
                        reader.subscription_matched_status();
                    std::cout << "Subscription matched => Cumulative matches = "
                              << st.total_count() << std::endl;
                }
            }
            else if (active_conditions[i] == read_cond) {
                // Current conditions match our conditions to read data, so we
                // can read data just like we would do in any other example.
                LoanedSamples<waitsets> samples = reader.take();
                for (LoanedSamples<waitsets>::iterator sampleIt=samples.begin();
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

int main(int argc, char *argv[])
{
    int domain_id    = 0;
    int sample_count = 0; // infinite loop

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
