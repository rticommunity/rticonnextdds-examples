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
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"  // for command line parsing and ctrl-c
#include "profiles.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Retrieve QoS from custom profile XML and USER_QOS_PROFILES.xml
    dds::core::QosProvider qos_provider("my_custom_qos_profiles.xml");

    // Create a DomainParticipant with the default QoS of the provider.
    dds::domain::DomainParticipant participant(
            domain_id,
            qos_provider.participant_qos());

    // Create a Publisher with default QoS.
    dds::pub::Publisher publisher(participant, qos_provider.publisher_qos());

    // Create a Topic with default QoS.
    dds::topic::Topic<profiles> topic(
            participant,
            "Example profiles",
            qos_provider.topic_qos());

    // Create a DataWriter with the QoS profile "transient_local_profile",
    // from QoS library "profiles_Library".
    dds::pub::DataWriter<profiles> writer_transient_local(
            publisher,
            topic,
            qos_provider.datawriter_qos(
                    "profiles_Library::transient_local_profile"));

    // Create a DataReader with the QoS profile "volatile_profile",
    // from the QoS library "profiles_Library".
    dds::pub::DataWriter<profiles> writer_volatile(
            publisher,
            topic,
            qos_provider.datawriter_qos("profiles_Library::volatile_profile"));

    // Create a data sample for writing.
    profiles instance;

    // Main loop.
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Update the counter value of the sample.
        instance.x(samples_written);

        // Send the sample using the DataWriter with "volatile" durability.
        std::cout << "Writing profile_name = volatile_profile,\t x = "
                  << samples_written << std::endl;
        instance.profile_name("volatile_profile");
        writer_volatile.write(instance);

        // Send the sample using the DataWriter with "transient_local"
        // durability.
        std::cout << "Writing profile_name = transient_local_profile,\t x = "
                  << samples_written << std::endl
                  << std::endl;
        instance.profile_name("transient_local_profile");
        writer_transient_local.write(instance);

        // Send the sample every second.
        rti::util::sleep(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, Entity::Publisher);
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
