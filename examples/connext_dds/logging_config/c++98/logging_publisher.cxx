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
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "loggingSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    // Start communicating in a domain, usually one participant per application
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    /*
     * Start - modifying the generated example to showcase the usage of
     * the Logging API.
     */

    /*
     * Set NDDS_CONFIG_LOG_VERBOSITY_WARNING NDDS_Config_LogVerbosity.
     * Set NDDS_CONFIG_LOG_PRINT_FORMAT_MAXIMAL NDDS_Config_LogPrintFormat
     * for NDDS_CONFIG_LOG_LEVEL_WARNING.
     */
    NDDSConfigLogger *logger = NULL;
    logger = NDDSConfigLogger::get_instance();
    if (logger == NULL) {
        return shutdown_participant(
                participant,
                "loggingTypeSupport::NDDSConfigLogger::get_instance error",
                EXIT_FAILURE);
    }
    logger->set_verbosity(NDDS_CONFIG_LOG_VERBOSITY_WARNING);
    if (!logger->set_print_format_by_log_level(
                NDDS_CONFIG_LOG_PRINT_FORMAT_MAXIMAL,
                NDDS_CONFIG_LOG_LEVEL_WARNING)) {
        return shutdown_participant(
                participant,
                "loggingTypeSupport::NDDSConfigLogger::set_print_format_by_log_"
                "level error",
                EXIT_FAILURE);
    }

    // A Publisher allows an application to create one or more DataWriters
    DDSPublisher *publisher = participant->create_publisher(
            DDS_PUBLISHER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        return shutdown_participant(
                participant,
                "create_publisher error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = loggingTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            loggingTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example logging",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    // This DataWriter writes data on "Example logging" Topic
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    // Narrow casts from an untyped DataWriter to a writer of your type
    loggingDataWriter *typed_writer = loggingDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Create data for writing, allocating all members
    logging *data = loggingTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "loggingTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    // Write data
    retcode = typed_writer->write(*data, DDS_HANDLE_NIL);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "write error " << retcode << std::endl;
    }

    /*
     * Force a warning by writing a sample where the source time stamp is older
     * than that of a previously sent sample. When using
     * DDS_BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS: If source timestamp is
     * older than in previous write a warnings message will be logged.
     */
    struct DDS_Time_t sourceTimestamp = { 0, 0 };
    retcode = typed_writer->write_w_timestamp(
            *data,
            DDS_HANDLE_NIL,
            sourceTimestamp);
    if (retcode == DDS_RETCODE_OK) {
        std::cerr << "write error " << retcode << std::endl;
    }
    /*
     * End - modifying the generated example to showcase the usage of
     * the Logging API.
     */

    // Delete previously allocated logging, including all contained elements
    retcode = loggingTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "loggingTypeSupport::delete_data error " << retcode
                  << std::endl;
    }

    NDDSConfigLogger::finalize_instance();
    // Delete all entities (DataWriter, Topic, Publisher, DomainParticipant)
    return shutdown_participant(participant, "Shutting down", EXIT_SUCCESS);
}

// Delete all entities
static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status)
{
    DDS_ReturnCode_t retcode;

    std::cout << shutdown_message << std::endl;

    if (participant != NULL) {
        // Cleanup everything created by this Participant
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_contained_entities error " << retcode
                      << std::endl;
            status = EXIT_FAILURE;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_participant error " << retcode << std::endl;
            status = EXIT_FAILURE;
        }
    }

    return status;
}

int main(int argc, char *argv[])
{
    // Parse arguments and handle control-C
    ApplicationArguments arguments;
    parse_arguments(arguments, argc, argv);
    if (arguments.parse_result == PARSE_RETURN_EXIT) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == PARSE_RETURN_FAILURE) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    NDDSConfigLogger::get_instance()->set_verbosity(arguments.verbosity);

    int status = run_publisher_application(
            arguments.domain_id,
            arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error " << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
