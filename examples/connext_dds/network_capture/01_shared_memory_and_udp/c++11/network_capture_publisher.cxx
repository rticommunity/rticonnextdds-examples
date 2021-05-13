/*
 * (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
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
#include "network_capture.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Start capturing traffic for all participants.
    //
    // All participants: those already created and those yet to be created.
    // Default parameters: all transports and some other sane defaults.
    //
    // A capture file will be created for each participant. The capture file
    // will start with the prefix "publisher" and continue with a suffix
    // dependent on the participant's GUID.
    if (!rti::util::network_capture::start("publisher")) {
        std::cerr << "Error starting network capture" << std::endl;
    }

    // The example continues as the usual Connext DDS HelloWorld.
    // Create entities, start communication, etc.
    dds::domain::DomainParticipant participant(domain_id);
    dds::topic::Topic<NetworkCapture> topic(
            participant,
            "Network capture shared memory example");
    dds::pub::Publisher publisher(participant);
    dds::pub::DataWriter<NetworkCapture> writer(publisher, topic);

    NetworkCapture data;
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        data.msg("Hello World " + std::to_string(samples_written));
        std::cout << "Writing NetworkCapture, count " << samples_written
                  << std::endl;

        // Here we are going to pause capturing for some samples.
        // The resulting pcap file will not contain them.
        if (samples_written == 4 && !rti::util::network_capture::pause()) {
            std::cerr << "Error pausing network capture" << std::endl;
        } else if (
                samples_written == 6 && !rti::util::network_capture::resume()) {
            std::cerr << "Error resuming network capture" << std::endl;
        }

        // Send once every second
        writer.write(data);
        rti::util::sleep(dds::core::Duration(1));
    }

    // Before deleting the participants that are capturing, we must stop
    // network capture for them.
    if (!rti::util::network_capture::stop()) {
        std::cerr << "Error stopping network capture" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    // Set Connext verbosity to help debugging
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    // Enable network capture.
    //
    // This must be called before:
    //   - Any other network capture function is called.
    //   - Creating the participants for which we want to capture traffic.
    if (!rti::util::network_capture::enable()) {
        std::cerr << "Error enabling network capture" << std::endl;
    }

    try {
        run_publisher_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }
    dds::domain::DomainParticipant::finalize_participant_factory();

    // Disable network capture.
    //
    // This must be:
    //   - The last network capture function that is called.
    if (!rti::util::network_capture::disable()) {
        std::cerr << "Error disabling network capture" << std::endl;
    }

    return EXIT_SUCCESS;
}
