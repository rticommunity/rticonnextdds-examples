/*******************************************************************************
 (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <thread>

#include <rti/recording/ServiceProperty.hpp>
#include <rti/recording/RecordingService.hpp>


void print_usage(const char *executable)
{
    std::cout <<
            "Usage: " << executable << " {record,replay} <Domain ID (int)> " <<
            "[running secs (uint, optional, default: 60)]"
            << std::endl;
}

void process_role(
        int argc,
        char *argv[],
        rti::recording::ServiceProperty& service_property,
        uint32_t& running_seconds)
{
    const std::string recorder_role("record");
    const std::string replay_role("replay");

    // Check if a role argument has been provided
    if (argc > 1) {
        if (recorder_role.compare(argv[1]) == 0) {
            service_property.application_role(
                    rti::recording::ApplicationRoleKind::RECORD_APPLICATION);
            service_property.cfg_file("recorder_config.xml");
        } else if (replay_role.compare(argv[1]) == 0) {
            service_property.application_role(
                    rti::recording::ApplicationRoleKind::REPLAY_APPLICATION);
            service_property.cfg_file("replay_config.xml");
        } else {
            throw std::runtime_error(
                    "Unsupported role arg: one of {record,replay} expected");
        }
    } else {
        throw std::runtime_error("Service role arg not provided");
    }
    // Check if a DDS domain ID has been provided
    if (argc > 2) {
        std::stringstream domain_id_stream(argv[2]);
        int32_t domain_id = 0;
        domain_id_stream >> domain_id;
        if (domain_id_stream.bad()) {
            throw std::runtime_error("Invalid arg value provided for domain ID");
        }
        // Set up domain ID base, admin domain ID and monitoring domain ID
        service_property.administration_domain_id(domain_id);
        service_property.domain_id_base(domain_id);
        service_property.monitoring_domain_id(domain_id);
    } else {
        throw std::runtime_error("Domain ID arg not provided");
    }
    // Check if the running seconds have been provided
    if (argc > 3) {
        std::stringstream running_secs_stream(argv[3]);
        running_secs_stream >> running_seconds;
        if (running_secs_stream.bad()) {
            throw std::runtime_error(
                    "Invalid arg value provided for running secs");
        }
    }
}

int main(int argc, char *argv[])
{
    /*
     * The ServiceProperty class defines runtime parameters for the service
     * instance, like the configuration file, the configuration name,
     * administration and monitoring DDS domain IDs, the DDS domain ID base
     * (offset) for the domain participants defined in the configuration, etc
     */
    rti::recording::ServiceProperty service_property;
    uint32_t running_seconds = 60;
    try {
        process_role(argc, argv, service_property, running_seconds);
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    // The service configuration, regardless of the role, is always the same
    service_property.service_name("service_as_lib");
    try {
        /*
         * Create the instance of the Recording Service. It won't start
         * executing until we call the start() method.
         */
        rti::recording::RecordingService embedded_service(service_property);
        embedded_service.start();
        // Wait for 'running_seconds' seconds
        std::this_thread::sleep_for(std::chrono::seconds(running_seconds));
        embedded_service.stop();
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

