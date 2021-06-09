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

#include "keys.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(dds::sub::DataReader<keys> reader)
{
    int count = 0;
    // Take all samples
    dds::sub::LoanedSamples<keys> samples = reader.take();

    for (const auto &sample : samples) {
        const dds::sub::SampleInfo &info = sample.info();
        if (info.valid()) {
            dds::sub::status::ViewState view_state = info.state().view_state();
            if (view_state == dds::sub::status::ViewState::new_view()) {
                std::cout << "Found new instance; code = "
                          << sample.data().code() << std::endl;
            }

            std::cout << "Instance " << sample.data().code()
                      << ", x: " << sample.data().x()
                      << ", y: " << sample.data().y() << std::endl;
        } else {
            // Since there is not valid data, it may include metadata.
            keys sample;
            reader.key_value(sample, info.instance_handle());

            // Here we print a message if the instance state is
            // 'not_alive_no_writers' or 'not_alive_disposed'.
            const dds::sub::status::InstanceState &state =
                    info.state().instance_state();
            if (state
                == dds::sub::status::InstanceState::not_alive_no_writers()) {
                std::cout << "Instance " << sample.code() << " has no writers"
                          << std::endl;
            } else if (
                    state
                    == dds::sub::status::InstanceState::not_alive_disposed()) {
                std::cout << "Instance " << sample.code() << " disposed"
                          << std::endl;
            }
        }

        count++;
    }

    return count;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<keys> topic(participant, "Example keys");

    // Create a subscriber
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader with default Qos
    dds::sub::DataReader<keys> reader(subscriber, topic);

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;

    // Create a ReadCondition for any data on this reader, and add to WaitSet
    unsigned int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [reader, &samples_read]() {
                samples_read += process_data(reader);
            });

    waitset += read_condition;

    // Main loop
    while (!application::shutdown_requested && samples_read < sample_count) {
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
