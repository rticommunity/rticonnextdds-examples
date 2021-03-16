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

#include "DroppedSamplesExample.h"
#include "DroppedSamplesExampleSupport.h"
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
                    NULL,  // listener
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
            NULL,  // listener
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        return shutdown_participant(
                participant,
                "create_publisher error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = DroppedSamplesExampleTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode = DroppedSamplesExampleTypeSupport::register_type(
            participant,
            type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example DroppedSamplesExample",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL,  // listener
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    // Start changes for detecting samples DroppedSamplesExampleped

    DDS_DataWriterQos writer_qos;
    retcode = publisher->get_default_datawriter_qos(writer_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "get_default_datawriter_qos error",
                EXIT_FAILURE);
    }
    /* Use batching in order to evaluate the CFT in the reader side */
    writer_qos.batch.enable = DDS_BOOLEAN_TRUE;
    writer_qos.batch.max_samples = 1;
    writer_qos.ownership.kind = DDS_EXCLUSIVE_OWNERSHIP_QOS;
    writer_qos.ownership_strength.value = 1;

    // This DataWriter writes data on "Example DroppedSamplesExample" Topic
    DDSDataWriter *untyped_writer1 = publisher->create_datawriter(
            topic,
            writer_qos,
            NULL,  // listener
            DDS_STATUS_MASK_NONE);
    if (untyped_writer1 == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    // Narrow casts from an untyped DataWriter to a writer of your type
    DroppedSamplesExampleDataWriter *typed_writer1 =
            DroppedSamplesExampleDataWriter::narrow(untyped_writer1);
    if (typed_writer1 == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    writer_qos.ownership_strength.value = 2;

    // This DataWriter writes data on "Example DroppedSamplesExample" Topic
    DDSDataWriter *untyped_writer2 = publisher->create_datawriter(
            topic,
            writer_qos,
            NULL,  // listener
            DDS_STATUS_MASK_NONE);
    if (untyped_writer2 == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    // Narrow casts from an untyped DataWriter to a writer of your type
    DroppedSamplesExampleDataWriter *typed_writer2 =
            DroppedSamplesExampleDataWriter::narrow(untyped_writer2);
    if (typed_writer2 == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Create data for writing, allocating all members
    DroppedSamplesExample *data =
            DroppedSamplesExampleTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "DroppedSamplesExampleTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        // Modify the data to be written here
        data->x = static_cast<DDS_Short>(samples_written);

        std::cout << "Writing DroppedSamplesExample, count " << samples_written
                  << std::endl;
        retcode = typed_writer1->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        retcode = typed_writer2->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        // Send once every second
        DDS_Duration_t send_period = { 1, 0 };
        NDDSUtility::sleep(send_period);
    }

    // End changes for detecting samples DroppedSamplesExampleped

    // Delete previously allocated DroppedSamplesExample, including all
    // contained elements
    retcode = DroppedSamplesExampleTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "DroppedSamplesExampleTypeSupport::delete_data error "
                  << retcode << std::endl;
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
