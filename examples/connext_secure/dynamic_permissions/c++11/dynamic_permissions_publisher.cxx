/*
 * (c) Copyright, Real-Time Innovations, 2025.  All rights reserved.
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
#include "dynamic_permissions.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(
            domain_id,
            dds::core::QosProvider::Default().participant_qos(
                    "dynamic_permissions_Library::publisher"));

    // Create a Topic with a name and a datatype
    dds::topic::Topic<::DynamicPermissions> topic(
            participant,
            "Example DynamicPermissions");

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter with default QoS
    dds::pub::DataWriter<::DynamicPermissions> writer(publisher, topic);

    ::DynamicPermissions data;
    // Main loop, write data
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify the data to be written here
        data.value(static_cast<int32_t>(samples_written));
        std::cout << "Writing ::DynamicPermissions, count " << samples_written
                  << std::endl;

        writer.write(data);

        // Send once every second
        rti::util::sleep(dds::core::Duration(1));

        // The Permissions Document expires after 1 minute (~60 samples).
        // Let's update it after 70 samples. At this point, the publisher and
        // subscriber lost communication. This will be fixed by updating the
        // Permissions Document.
        if (samples_written == 70) {
            std::cout << "Updating Permissions Document" << std::endl;
            dds::domain::qos::DomainParticipantQos updated_qos =
                    participant.qos();
            updated_qos->property.set(rti::core::policy::Property::Entry(
                    "dds.sec.access.permissions",
                    "security/ecdsa01/xml/signed/signed_Permissions2.p7s"));
            participant << updated_qos;
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
