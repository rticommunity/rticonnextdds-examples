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

#include <string>

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"
#include "batch_data.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count,
        bool turbo_mode_on)
{
    // Seconds to wait between samples published.
    dds::core::Duration send_period(1);

    // We pick the profile name if the turbo_mode is selected or not.
    // If turbo_mode is not selected, the batching profile will be used.
    std::string profile_name = "batching_Library::";
    if (turbo_mode_on) {
        std::cout << "Turbo Mode enable" << std::endl;
        profile_name.append("turbo_mode_profile");

        // If turbo_mode, we do not want to wait to send samples.
        send_period.sec(0);
    } else {
        std::cout << "Manual batching enable" << std::endl;
        profile_name.append("batch_profile");
    }

    // To customize entities QoS use the file USER_QOS_PROFILES.xml
    dds::domain::DomainParticipant participant(
            domain_id,
            dds::core::QosProvider::Default().participant_qos(profile_name));

    dds::topic::Topic<batch_data> topic(participant, "Example batch_data");

    dds::pub::Publisher publisher(
            participant,
            dds::core::QosProvider::Default().publisher_qos(profile_name));

    dds::pub::DataWriter<batch_data> writer(
            publisher,
            topic,
            dds::core::QosProvider::Default().datawriter_qos(profile_name));

    // Create data sample for writing.
    batch_data sample;

    // For a data type that has a key, if the same instance is going to be
    // written multiple times, initialize the key here and register the keyed
    // instance prior to writing.
    dds::core::InstanceHandle instance_handle =
            dds::core::InstanceHandle::nil();
    // instance_handle = writer.register_instance(sample);

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify the data to be written here
        sample.x(samples_written);

        std::cout << "Writing batch_data, count " << samples_written
                  << std::endl;
        writer.write(sample);

        rti::util::sleep(send_period);
    }

    // writer.unregister_instance(sample);
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
        run_publisher_application(
                arguments.domain_id,
                arguments.sample_count,
                arguments.turbo_mode);
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
