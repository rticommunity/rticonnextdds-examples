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

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "compression.h"
#include "compressionSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count,
        const std::string &compression_id,
        const std::string &input_file)
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
    const char *type_name = StringLineTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            StringLineTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example StringLine",
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

    DDS_DataWriterQos datawriter_qos;
    retcode = publisher->get_default_datawriter_qos(datawriter_qos);
    if (retcode != DDS_RETCODE_OK) {
        printf("get_default_datawriter_qos error\n");
        return -1;
    }

    if (compression_id.find("NONE") != std::string::npos) {
        datawriter_qos.representation.compression_settings.compression_ids =
                DDS_COMPRESSION_ID_MASK_NONE;
    }
    if (compression_id.find("ZLIB") != std::string::npos) {
        datawriter_qos.representation.compression_settings.compression_ids =
                DDS_COMPRESSION_ID_ZLIB_BIT;
    }
    if (compression_id.find("BZIP2") != std::string::npos) {
        datawriter_qos.representation.compression_settings.compression_ids =
                DDS_COMPRESSION_ID_BZIP2_BIT;
    }
    if (compression_id.find("LZ4") != std::string::npos) {
        datawriter_qos.representation.compression_settings.compression_ids =
                DDS_COMPRESSION_ID_LZ4_BIT;
    }

    // This DataWriter writes data on "Example StringLine" Topic
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            datawriter_qos,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    // Narrow casts from an untyped DataWriter to a writer of your type
    StringLineDataWriter *typed_writer =
            StringLineDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    std::vector<StringLine> samples;
    if (!input_file.empty()) {
        // Open file to compress
        std::ifstream fileToCompress(input_file.c_str());
        if (!fileToCompress) {
            printf("open file %s error\n", input_file.c_str());
            return -1;
        }

        std::string new_line;
        while (!std::getline(fileToCompress, new_line).eof()) {
            StringLine new_sample;
            new_sample.str = DDS_String_dup(new_line.c_str());
            samples.push_back(new_sample);
        }
    } else {
        // Create a sample fill with 1024 zeros to send if no file has been
        // provided
        StringLine new_sample;
        new_sample.str = DDS_String_dup(std::string(1024, '0').c_str());
        samples.push_back(new_sample);
    }

    // Wait at least 1 second for discovery
    DDS_Duration_t discovery_period = { 1, 0 };
    NDDSUtility::sleep(discovery_period);

    std::vector<StringLine>::iterator it = samples.begin();

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written, ++it) {
        /* Loop over the lines on the file */
        if (it == samples.end()) {
            it = samples.begin();
        }
        if (!(samples_written % 10)) {
            std::cout << "Writing StringLine, count " << samples_written
                      << std::endl;
        }

        retcode = typed_writer->write((*it), DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        // Send once every 100 millisec
        DDS_Duration_t send_period = { 0, 100000 };
        NDDSUtility::sleep(send_period);
    }

    for (std::vector<StringLine>::iterator it = samples.begin();
         it != samples.end();
         ++it) {
        // Delete previously allocated strings
        DDS_String_free((*it).str);
    }

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
            arguments.sample_count,
            arguments.compression_id,
            arguments.input_file);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error " << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
