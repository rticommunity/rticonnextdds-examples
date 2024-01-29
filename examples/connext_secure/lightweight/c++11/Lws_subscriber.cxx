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

#include "Lws.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void process_data(dds::sub::DataReader<Shape> reader)
{
    // Take all samples
    dds::sub::LoanedSamples<Shape> samples = reader.take();
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
        unsigned int sample_count,
        bool lw)
{
    // DDS objects behave like shared pointers or value types
    // (see
    // https://community.rti.com/best-practices/use-modern-c-types-correctly)

    dds::domain::qos::DomainParticipantQos participant_qos =
            dds::core::QosProvider::Default().participant_qos(
                    lw ? "lightweight_library::peer" : "full_library::peer2");
    dds::domain::DomainParticipant participant(domain_id, participant_qos);

    if (lw) {
        std::cout << "Created a Lightweight Security participant" << std::endl;
    }

    // Create a Topic with a name and a datatype
    dds::topic::Topic<Shape> topic1(participant, "Square");
    dds::topic::Topic<Shape> topic2(participant, "Circle");

    // Create a Subscriber and DataReader with default Qos
    dds::sub::Subscriber subscriber(participant);
    dds::sub::DataReader<Shape> reader1(subscriber, topic1);
    dds::sub::DataReader<Shape> reader2(subscriber, topic2);

    // Create a ReadCondition for any data received on this reader and set a
    // handler to process the data
    dds::sub::cond::ReadCondition read_condition1(
            reader1,
            dds::sub::status::DataState::any(),
            [reader1]() { process_data(reader1); });

    dds::sub::cond::ReadCondition read_condition2(
            reader2,
            dds::sub::status::DataState::any(),
            [reader2]() { process_data(reader2); });

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;
    waitset += read_condition1;
    waitset += read_condition2;


    while (!application::shutdown_requested) {
        std::cout << "Shape subscriber sleeping up to 1 sec..." << std::endl;

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
        run_subscriber_application(
                arguments.domain_id,
                arguments.sample_count,
                arguments.lw);
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
