/*
 * (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
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
#include "application.hpp"        // for command line parsing and ctrl-c
#include "ShapeType.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    rti::domain::register_type<ShapeType>("ShapeType");

    // IMPORTANT: If you plan to use a domain_id different than 0, remember
    // to change the value of the domain_id attribute of the domain tag in
    // the USER_QOS_PROFILES.xml
    rti::domain::DomainParticipantConfigParams params(domain_id);

    auto default_provider = dds::core::QosProvider::Default();

    // Create the participant and all its entities from the XML file
    // (USER_QOS_PROFILES.xml)
    dds::domain::DomainParticipant participant =
            default_provider.extensions().create_participant_from_config(
                    "ShapeType_DomainParticipantLibrary::"
                    "ShapeTypePublisherParticipant",
                    params);

    // Retrieve both DataWriters
    dds::pub::DataWriter<ShapeType> red_writer =
            rti::pub::find_datawriter_by_name<dds::pub::DataWriter<ShapeType>>(
                    participant,
                    "Pub::DW");
    dds::pub::DataWriter<ShapeType> blue_writer =
            rti::pub::find_datawriter_by_name<dds::pub::DataWriter<ShapeType>>(
                    participant,
                    "Pub::DW#1");

    // Declare both ShapeType instances and set the fields that don't change
    ShapeType red_data, blue_data;
    red_data.shapesize(30);
    red_data.color("RED");
    blue_data.shapesize(30);
    blue_data.color("BLUE");

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify (X, Y) coordinates of both instances
        red_data.x(samples_written);
        red_data.y(samples_written);
        blue_data.x(samples_written);
        blue_data.y(samples_written);

        // Write
        std::cout << "Writing RED ShapeType sample, count " << samples_written
                  << std::endl;
        red_writer.write(red_data);

        std::cout << "Writing BLUE ShapeType sample, count " << samples_written
                  << std::endl;
        blue_writer.write(blue_data);

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
