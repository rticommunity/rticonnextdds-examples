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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "waitsets.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default QoS.
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<waitsets> topic(participant, "Example waitsets");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the DataWriter QoS programmatically rather
    // than using the XML file, you will need to uncomment these lines.

    // reader_qos << Reliability::Reliable()
    //            << History::KeepAll()
    //            << Liveliness::Automatic().lease_duration(Duration(2));

    // Create a Subscriber
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader.
    dds::sub::DataReader<waitsets> reader(subscriber, topic, reader_qos);

    // Create a Read Condition.
    // Note that the Read Conditions are dependent on both incoming data as
    // well as sample state.
    dds::sub::cond::ReadCondition read_cond(
            reader,
            dds::sub::status::DataState(
                    dds::sub::status::SampleState::not_read(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::any()));

    // Get status conditions.
    // Each entity may have an attached Status Condition. To modify the
    // statuses we need to get the reader's Status Conditions first.
    dds::core::cond::StatusCondition status_condition(reader);

    // Set enabled statuses.
    // Now that we have the Status Condition, we are going to enable the
    // statuses we are interested in:
    status_condition.enabled_statuses(
            dds::core::status::StatusMask::subscription_matched()
            | dds::core::status::StatusMask::liveliness_changed());

    // Create and attach conditions to the WaitSet
    // Finally, we create the WaitSet and attach both the Read Conditions and
    // the Status Condition to it. We can use the operator '+=' or the method
    // 'attach_condition'.
    dds::core::cond::WaitSet waitset;
    waitset += read_cond;
    waitset.attach_condition(status_condition);

    int samples_read = 0;
    // Main loop.
    while (!application::shutdown_requested && samples_read < sample_count) {
        // 'wait()' blocks execution of the thread until one or more attached
        // Conditions become true, or until a user-specified timeout expires.
        dds::core::cond::WaitSet::ConditionSeq active_conditions =
                waitset.wait(dds::core::Duration::from_millisecs(1500));

        if (active_conditions.size() == 0) {
            std::cout << "Wait timed out!! No conditions were triggered."
                      << std::endl;
            continue;
        }

        std::cout << "Got " << active_conditions.size() << " active conditions"
                  << std::endl;
        for (std::vector<int>::size_type i = 0; i < active_conditions.size();
             i++) {
            // Now we compare the current condition with the Status Conditions
            // and the Read Conditions previously defined. If they match, we
            // print the condition that was triggered.

            if (active_conditions[i] == status_condition) {
                // Get the status changes so we can check witch status condition
                // triggered.
                dds::core::status::StatusMask status_mask =
                        reader.status_changes();

                // Liveliness changed
                if ((status_mask
                     & dds::core::status::StatusMask::liveliness_changed())
                            .any()) {
                    dds::core::status::LivelinessChangedStatus st =
                            reader.liveliness_changed_status();
                    std::cout << "Liveliness changed => Active writers = "
                              << st.alive_count() << std::endl;
                }

                // Subscription matched
                if ((status_mask
                     & dds::core::status::StatusMask::subscription_matched())
                            .any()) {
                    dds::core::status::SubscriptionMatchedStatus st =
                            reader.subscription_matched_status();
                    std::cout << "Subscription matched => Cumulative matches = "
                              << st.total_count() << std::endl;
                }
            } else if (active_conditions[i] == read_cond) {
                // Current conditions match our conditions to read data, so we
                // can read data just like we would do in any other example.
                dds::sub::LoanedSamples<waitsets> samples = reader.take();
                for (const auto &sample : samples) {
                    if (!sample.info().valid()) {
                        std::cout << "Got metadata" << std::endl;
                    } else {
                        std::cout << sample.data() << std::endl;
                        samples_read++;
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
