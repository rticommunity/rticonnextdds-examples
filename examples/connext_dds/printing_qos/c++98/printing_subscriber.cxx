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

#include "printing.h"
#include "printingSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

// Process data. Returns number of samples processed.
unsigned int process_data(printingDataReader *typed_reader)
{
    printingSeq data_seq;        // Sequence of received data
    DDS_SampleInfoSeq info_seq;  // Metadata associated with samples in data_seq
    unsigned int samples_read = 0;

    // Take available data from DataReader's queue
    typed_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    // Iterate over all available data
    for (int i = 0; i < data_seq.length(); ++i) {
        // Check if a sample is an instance lifecycle event
        if (info_seq[i].valid_data) {
            // Print data
            std::cout << "Received data" << std::endl;
            printingTypeSupport::print_data(&data_seq[i]);
            samples_read++;
        } else {  // This is an instance lifecycle event with no data payload.
            std::cout << "Received instance state notification" << std::endl;
        }
    }
    // Data loaned from Connext for performance. Return loan when done.
    DDS_ReturnCode_t retcode = typed_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "return loan error " << retcode << std::endl;
    }

    return samples_read;
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
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

    DDS_DomainParticipantQos participant_qos;
    DDS_ReturnCode_t retcode = participant->get_qos(participant_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(participant, "get_qos error", EXIT_FAILURE);
    }
    char *str = NULL;
    DDS_UnsignedLong strSize = 0;
    DDS_QosPrintFormat printFormat = DDS_QosPrintFormat_INITIALIZER;
    /*
     * First, we pass NULL for the str argument. This will cause the API to
     * update the strSize argument to contain the required size of the str
     * buffer. We then allocate a buffer of that size and obtain the QoS string.
     * The DDS_DomainParticipantQos_to_string_w_params API prints all the QoS
     * values for the DomainParticipantQos object.
     */
    participant_qos.to_string(str, strSize, DDS_QOS_PRINT_ALL, printFormat);
    str = DDS_String_alloc(strSize);
    if (str == NULL) {
        return shutdown_participant(
                participant,
                "String allocation error",
                EXIT_FAILURE);
    }
    participant_qos.to_string(str, strSize, DDS_QOS_PRINT_ALL, printFormat);
    std::cout << str << std::endl;
    DDS_String_free(str);
    str = NULL;

    // A Subscriber allows an application to create one or more DataReaders
    DDSSubscriber *subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL,  // listener
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        return shutdown_participant(
                participant,
                "create_subscriber error",
                EXIT_FAILURE);
    }

    DDS_SubscriberQos subscriber_qos;
    retcode = subscriber->get_qos(subscriber_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(participant, "get_qos error", EXIT_FAILURE);
    }
    /*
     * First, we pass NULL for the str argument. This will cause the API to
     * update the strSize argument to contain the required size of the str
     * buffer. We then allocate a buffer of that size and obtain the QoS string.
     * The DDS_SubscriberQos_to_string_w_params API prints all the QoS values
     * for the SubscriberQos object.
     */
    subscriber_qos.to_string(str, strSize, printFormat);
    str = DDS_String_alloc(strSize);
    if (str == NULL) {
        return shutdown_participant(
                participant,
                "String allocation error",
                EXIT_FAILURE);
    }
    subscriber_qos.to_string(str, strSize, printFormat);
    std::cout << str << std::endl;
    DDS_String_free(str);
    str = NULL;

    // Register the datatype to use when creating the Topic
    const char *type_name = printingTypeSupport::get_type_name();
    retcode = printingTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example printing",
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

    // This DataReader reads data on "Example printing" Topic
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_NONE);
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    DDS_DataReaderQos reader_qos;
    retcode = untyped_reader->get_qos(reader_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(participant, "get_qos error", EXIT_FAILURE);
    }
    reader_qos.print();

    // Narrow casts from a untyped DataReader to a reader of your type
    printingDataReader *typed_reader =
            printingDataReader::narrow(untyped_reader);
    if (typed_reader == NULL) {
        return shutdown_participant(
                participant,
                "DataReader narrow error",
                EXIT_FAILURE);
    }

    // Create ReadCondition that triggers when unread data in reader's queue
    DDSReadCondition *read_condition = typed_reader->create_readcondition(
            DDS_NOT_READ_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);
    if (read_condition == NULL) {
        return shutdown_participant(
                participant,
                "create_readcondition error",
                EXIT_FAILURE);
    }

    // WaitSet will be woken when the attached condition is triggered
    DDSWaitSet waitset;
    retcode = waitset.attach_condition(read_condition);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "attach_condition error",
                EXIT_FAILURE);
    }

    // Main loop. Wait for data to arrive, and process when it arrives
    unsigned int samples_read = 0;
    while (!shutdown_requested && samples_read < sample_count) {
        DDSConditionSeq active_conditions_seq;

        // Wait for data and report if it does not arrive in 1 second
        DDS_Duration_t wait_timeout = { 1, 0 };
        retcode = waitset.wait(active_conditions_seq, wait_timeout);

        if (retcode == DDS_RETCODE_OK) {
            // If the read condition is triggered, process data
            samples_read += process_data(typed_reader);
        } else {
            if (retcode == DDS_RETCODE_TIMEOUT) {
                std::cout << "No data after 1 second" << std::endl;
            }
        }
    }

    // Cleanup
    return shutdown_participant(participant, "Shutting down", 0);
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
            std::cerr << "delete_contained_entities error" << retcode
                      << std::endl;
            status = EXIT_FAILURE;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_participant error" << retcode << std::endl;
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

    int status = run_subscriber_application(
            arguments.domain_id,
            arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error" << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
