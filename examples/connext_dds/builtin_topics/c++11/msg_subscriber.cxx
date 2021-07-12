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

#include "msg.hpp"
#include "application.hpp"


int process_data(dds::sub::DataReader<msg> reader)
{
    int count = 0;
    dds::sub::LoanedSamples<msg> samples = reader.take();
    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            count++;
            std::cout << sample.data() << std::endl;
        }
    }

    return count;
}  // The LoanedSamples destructor returns the loan

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count,
        std::string participant_auth)
{
    // Retrieve the default participant QoS, from USER_QOS_PROFILES.xml
    dds::domain::qos::DomainParticipantQos participant_qos =
            dds::core::QosProvider::Default().participant_qos();
    auto resource_limits_qos = participant_qos.policy<
            rti::core::policy::DomainParticipantResourceLimits>();

    // If you want to change the Participant's QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // resource_limits_qos.participant_user_data_max_length(1024);
    // participant_qos << resource_limits_qos;

    unsigned int max_participant_user_data =
            resource_limits_qos.participant_user_data_max_length();
    if (participant_auth.size() > max_participant_user_data) {
        std::cout << "error, participant user_data exceeds resource limits"
                  << std::endl;
    } else {
        participant_qos << dds::core::policy::UserData(dds::core::ByteSeq(
                participant_auth.begin(),
                participant_auth.end()));
    }

    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id, participant_qos);

    // Participant is disabled by default in USER_QOS_PROFILES. We enable it now
    participant.enable();

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<msg> topic(participant, "Example msg");

    // Create a DataReader with default QoS
    // Create a Subscriber and DataReader with default Qos
    dds::sub::Subscriber subscriber(participant);
    dds::sub::DataReader<msg> reader(subscriber, topic);

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
        std::cout << "builtin_topics subscriber sleeping for 1 sec...\n";

        // Wait for data and report if it does not arrive in 1 second
        waitset.dispatch(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, ApplicationKind::subscriber);
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
                arguments.participant_password);
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
