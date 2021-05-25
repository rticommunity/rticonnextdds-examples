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

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"  // for command line parsing and ctrl-c
#include "profiles.hpp"

void run_publisher_application(int domain_id, int sample_count)
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

    // Retrieve the DataWriter QoS from USER_QOS_PROFILES.xml
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // This example uses a built-in QoS profile to tune QoS for reliable
    // streaming data. To enable it programmatically uncomment these lines.

    // writer_qos = QosProvider::Default().datawriter_qos(
    //     "BuiltinQosLibExp::Pattern.ReliableStreaming");

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create a Datawriter.
    dds::pub::DataWriter<HelloWorld> writer(publisher, topic, writer_qos);

    // Create a data sample for writing.
    HelloWorld instance;

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify the data to be written here
        instance.msg("Hello World!");

        std::cout << "Writing HelloWord, count " << samples_written
                  << std::endl;

        writer.write(instance);

        // Send once every second
        rti::util::sleep(dds::core::Duration(4));
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
