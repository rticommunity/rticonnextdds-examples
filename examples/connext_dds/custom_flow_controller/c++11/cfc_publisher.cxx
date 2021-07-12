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

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>           // for sleep()
#include <rti/config/Logger.hpp>       // for logging
#include <rti/pub/FlowController.hpp>  // for FlowController

#include "application.hpp"  // for command line parsing and ctrl-c
#include "cfc.hpp"

const std::string flow_controller_name = "custom_flowcontroller";

rti::pub::FlowControllerProperty define_flowcontroller(
        dds::domain::DomainParticipant &participant)
{
    // Create a custom FlowController based on tocken-bucket mechanism.
    // First specify the token-bucket configuration.
    // In summary, each token can be used to send about one message,
    // and we get 2 tokens every 100 ms, so this limits transmissions to
    // about 20 messages per second.
    rti::pub::FlowControllerTokenBucketProperty flowcontroller_tokenbucket;

    // Don't allow too many tokens to accumulate.
    flowcontroller_tokenbucket.max_tokens(2);
    flowcontroller_tokenbucket.tokens_added_per_period(2);
    flowcontroller_tokenbucket.tokens_leaked_per_period(
            dds::core::LENGTH_UNLIMITED);

    // Period of 100 ms.
    flowcontroller_tokenbucket.period(dds::core::Duration::from_millisecs(100));

    // The sample size is 1000, but the minimum bytes_per_token is 1024.
    // Furthermore, we want to allow some overhead.
    flowcontroller_tokenbucket.bytes_per_token(1024);

    // Create a FlowController Property to set the TokenBucket definition.
    rti::pub::FlowControllerProperty flowcontroller_property(
            rti::pub::FlowControllerSchedulingPolicy::EARLIEST_DEADLINE_FIRST,
            flowcontroller_tokenbucket);

    return flowcontroller_property;
}

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Retrieve the default Participant QoS, from USER_QOS_PROFILES.xml
    dds::domain::qos::DomainParticipantQos participant_qos =
            dds::core::QosProvider::Default().participant_qos();

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    //
    // By default, data will be sent via shared memory "and" UDPv4.
    // Because the flowcontroller limits writes across all interfaces,
    // this halves the effective send rate. To avoid this, we enable only the
    // UDPv4 transport.

    // participant_qos << TransportBuiltin::UDPv4();

    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id, participant_qos);

    // Create the FlowController by code instead of from USER_QOS_PROFILES.xml.
    rti::pub::FlowControllerProperty flow_controller_property =
            define_flowcontroller(participant);
    rti::pub::FlowController flowcontroller(
            participant,
            flow_controller_name,
            flow_controller_property);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<cfc> topic(participant, "Example cfc");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer_qos << History::KeepAll()
    //            << PublishMode::Asynchronous(flow_controller_name);

    // Create a publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter (Publisher created in-line)
    dds::pub::DataWriter<cfc> writer(publisher, topic, writer_qos);

    // Create a sample to write with a long payload.
    cfc sample;
    sample.str(std::string(999, 'A'));

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Simulate bursty writer.
        rti::util::sleep(dds::core::Duration(1));

        for (int i = 0; i < 10; i++) {
            sample.x(samples_written * 10 + i);

            std::cout << "Writing cfc, sample " << sample.x() << std::endl;
            writer.write(sample);
        }
    }

    // This new sleep is to give the subscriber time to read all the samples.
    rti::util::sleep(dds::core::Duration(4));
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
        run_publisher_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
