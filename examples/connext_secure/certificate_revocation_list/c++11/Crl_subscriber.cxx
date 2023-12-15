/*
 * (c) Copyright, Real-Time Innovations, 2023.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

#include <algorithm>
#include <iostream>

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "Crl.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void process_data(dds::sub::DataReader<Example> reader)
{
    // Take all samples
    dds::sub::LoanedSamples<Example> samples = reader.take();
    for (auto sample : samples) {
        if (sample.info().valid()) {
            std::cout << sample.data() << std::endl;
        } else {
            std::cout << "Instance state changed to "
                      << sample.info().state().instance_state() << std::endl;
        }
    }
}  // The LoanedSamples destructor returns the loan

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // DDS objects behave like shared pointers or value types
    // (see
    // https://community.rti.com/best-practices/use-modern-c-types-correctly)

    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(
            domain_id,
            dds::core::QosProvider::Default().participant_qos(
                    "full_library::peer2"));

    // Create a Topic with a name and a datatype
    dds::topic::Topic<Example> topic(participant, "Crl Example");

    // Create a Subscriber and DataReader with default Qos
    dds::sub::Subscriber subscriber(participant);
    dds::sub::DataReader<Example> reader(subscriber, topic);

    // Create a Status Condition for the reader
    dds::core::cond::StatusCondition status_condition(reader);

    // Enable statuses configuration for the status that it is desired
    status_condition.enabled_statuses(
            dds::core::status::StatusMask::subscription_matched());

    status_condition->handler([&reader]() {
        dds::core::status::StatusMask status_mask = reader.status_changes();
        // In Case of Liveliness changed
        if ((status_mask
             & dds::core::status::StatusMask::subscription_matched())
                    .any()) {
            dds::core::status::SubscriptionMatchedStatus st =
                    reader.subscription_matched_status();
            std::cout << "Subscription matched: current count: "
                      << st.current_count() << std::endl;
        }
    });

    // Create a ReadCondition for any data received on this reader and set a
    // handler to process the data
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [reader]() { process_data(reader); });

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;
    waitset += status_condition;

    while (!application::shutdown_requested) {
        std::cout << "Example subscriber sleeping up to 1 sec..." << std::endl;

        // Run the handlers of the active conditions. Wait for up to 1 second.
        waitset.dispatch(dds::core::Duration(1));
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
