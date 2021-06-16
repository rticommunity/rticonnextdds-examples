/*******************************************************************************
 (c) 2005-2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <algorithm>
#include <iostream>

#include <dds/core/ddscore.hpp>
#include <dds/sub/ddssub.hpp>
// Or simply include <dds/dds.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "waitset_cond_modern.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<Foo> topic(participant, "Example Foo");

    // Create a DataReader with default Qos (Subscriber created in-line)
    dds::sub::DataReader<Foo> reader(dds::sub::Subscriber(participant), topic);

    // Create a Status Condition for the reader
    dds::core::cond::StatusCondition status_condition(reader);

    // Enable statuses configuration for the status that it is desired
    status_condition.enabled_statuses(
            dds::core::status::StatusMask::liveliness_changed());

    // Lambda function for the status_condition
    // Handler register a custom handler with the condition
    status_condition->handler([&reader]() {
        // Get the status changes so we can check witch status condition
        // triggered
        dds::core::status::StatusMask status_mask = reader.status_changes();
        // In Case of Liveliness changed
        if ((status_mask & dds::core::status::StatusMask::liveliness_changed())
                    .any()) {
            dds::core::status::LivelinessChangedStatus st =
                    reader.liveliness_changed_status();
            std::cout << "Liveliness changed => Active writers = "
                      << st.alive_count() << std::endl;
        }
    });  // Create a ReadCondition for any data on this reader and associate a
         // handler

    int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [&reader, &samples_read]() {
                // Take all samples
                dds::sub::LoanedSamples<Foo> samples = reader.take();
                for (auto sample : samples) {
                    if (sample.info().valid()) {
                        samples_read++;
                        std::cout << sample.data() << std::endl;
                    }
                }
            }  // The LoanedSamples destructor returns the loan
    );

    // Create a WaitSet and attach both ReadCondition and StatusCondition
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;
    waitset += status_condition;

    while (!application::shutdown_requested && samples_read < sample_count) {
        // Dispatch will call the handlers associated to the WaitSet conditions
        // when they activate
        waitset.dispatch(dds::core::Duration(4));  // Wait up to 4s each time
    }
    return 1;
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
