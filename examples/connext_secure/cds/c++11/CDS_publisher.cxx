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

#include <iostream>

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"  // for command line parsing and ctrl-c
#include "CDS.hpp"

void run_publisher_application(
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
                    "lite_library::lite_peer"));

    // Create a Topic with a name and a datatype
    dds::topic::Topic<Example> topic(participant, "CDS LWS Example");

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter with default QoS
    dds::pub::DataWriter<Example> writer(publisher, topic);

    Example data;
    // Main loop, write data
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify the data to be written here
        data.value = static_cast<int32_t>(samples_written);
        std::cout << "Writing CDS, count " << samples_written << std::endl;

        writer.write(data);

        // Send once every second
        rti::util::sleep(dds::core::Duration(1));
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
