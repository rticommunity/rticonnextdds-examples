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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "profiles.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(dds::sub::DataReader<HelloWorld> reader)
{
    // Take all samples
    int count = 0;
    dds::sub::LoanedSamples<HelloWorld> samples = reader.take();

    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            count++;
            std::cout << sample.data() << std::endl;
        } else {
            std::cout << "Instance state changed to "
                      << sample.info().state().instance_state() << std::endl;
        }
    }

    return count;
}

void run_subscriber_application(int domain_id, int sample_count)
{
    // Retrieve the Participant QoS from USER_QOS_PROFILES.xml
    dds::domain::qos::DomainParticipantQos participant_qos =
            dds::core::QosProvider::Default().participant_qos();

    // This example uses a built-in QoS profile to enable monitoring on the
    // DomainParticipant. This profile is specified in USER_QOS_PROFILES.xml.
    // To enable it programmatically uncomment these lines.

    // participant_qos = QosProvider::Default().participant_qos(
    //     "BuiltinQosLib::Generic.Monitoring.Common");

    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id, participant_qos);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<HelloWorld> topic(participant, "Example profiles");

    // Retrieve the DataReader QoS from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // This example uses a built-in QoS profile to tune QoS for reliable
    // streaming data. To enable it programmatically uncomment these lines.

    // reader_qos = QosProvider::Default().datareader_qos(
    //     "BuiltinQosLibExp::Pattern.ReliableStreaming");

    // Create a Subscriber and DataReader with a Qos
    dds::sub::Subscriber subscriber(participant);
    dds::sub::DataReader<HelloWorld> reader(subscriber, topic, reader_qos);

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
        std::cout << "profiles subscriber sleeping up to 4 sec..." << std::endl;
        waitset.dispatch(dds::core::Duration(4));
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
