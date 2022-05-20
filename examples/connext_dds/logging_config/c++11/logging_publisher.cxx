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
// alternatively, to include all the standard APIs:
//  <dds/dds.hpp>
// or to include both the standard APIs and extensions:
//  <rti/rti.hpp>
//
// For more information about the headers and namespaces, see:
//    https://community.rti.com/static/documentation/connext-dds/6.1.1/doc/api/connext_dds/api_cpp2/group__DDSNamespaceModule.html
// For information on how to use extensions, see:
//    https://community.rti.com/static/documentation/connext-dds/6.1.1/doc/api/connext_dds/api_cpp2/group__DDSCpp2Conventions.html

#include "application.hpp"  // for command line parsing and ctrl-c
#include "logging.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // DDS objects behave like shared pointers or value types
    // (see
    // https://community.rti.com/static/documentation/connext-dds/6.1.1/doc/api/connext_dds/api_cpp2/group__DDSCpp2Conventions.html)

    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic with a name and a datatype
    dds::topic::Topic<logging> topic(participant, "Example logging");

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    /*
     * Start - modifying the generated example to showcase the usage of
     * the Logging API.
     */

    /*
     * Set NDDS_CONFIG_LOG_VERBOSITY_WARNING NDDS_Config_LogVerbosity.
     * Set NDDS_CONFIG_LOG_PRINT_FORMAT_MAXIMAL NDDS_Config_LogPrintFormat
     * for NDDS_CONFIG_LOG_LEVEL_WARNING.
     */
    rti::config::Logger::instance().verbosity(rti::config::Verbosity::WARNING);
    rti::config::Logger::instance().print_format_by_log_level(
            rti::config::PrintFormat::MAXIMAL,
            rti::config::LogLevel::WARNING);

    // Create a DataWriter with default QoS
    dds::pub::DataWriter<logging> writer(publisher, topic);

    logging data;

    writer.write(data);

    /*
     * Force a warning by writing a sample where the source time stamp is older
     * than that of a previously sent sample. When using
     * DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS: If source timestamp is
     * older than in previous write a warnings message will be logged.
     */
    writer.write(data, dds::core::null_type(), dds::core::Time(0));
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
