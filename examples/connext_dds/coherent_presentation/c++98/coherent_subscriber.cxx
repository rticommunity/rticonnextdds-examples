/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "coherent.h"
#include "coherentSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

// Start changes for coherent_presentation
static const unsigned int STATENVALS = 6;
int statevals[STATENVALS] = { 0 };

void print_state()
{
    char c = 'a';

    for (int i = 0; i < STATENVALS; ++i) {
        std::cout << " " << c++ << " = " << statevals[i] << ";";
    }
}

void set_state(char c, int value)
{
    unsigned int idx = c - 'a';
    if (idx < 0 || idx >= STATENVALS) {
        std::cerr << "error: invalid field " << c << std::endl;
        return;
    }
    statevals[idx] = value;
}
// End changes for coherent_presentation

unsigned int process_data(coherentDataReader *typed_reader)
{
    coherentSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    unsigned int samples_read = 0;

    typed_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    /* Start changes for coherent_presentation */

    /* Firstly process all samples */
    for (int i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            set_state(data_seq[i].field, data_seq[i].value);
            samples_read++;
        }
    }

    /* Then, we print the results */
    if (samples_read > 0) {
        std::cout << "Received " << samples_read << " updates\n";
        print_state();
        std::cout << std::endl;
    }
    /* End changes for coherent_presentation */

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
    DDS_Duration_t wait_timeout = { 4, 0 };

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

    // A Subscriber allows an application to create one or more DataReaders
    DDSSubscriber *subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        return shutdown_participant(
                participant,
                "create_subscriber error",
                EXIT_FAILURE);
    }

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_subscriber call above.
     */
    /* Start changes for coherent_presentation */
    /*
        DDS_ReturnCode_t retcode = participant->
                get_default_subscriber_qos(subscriber_qos);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "get_default_subscriber_qos error\n";
            return EXIT_FAILURE;
        }

        subscriber_qos.presentation.access_scope = DDS_TOPIC_PRESENTATION_QOS;
        subscriber_qos.presentation.coherent_access = DDS_BOOLEAN_TRUE;

        DDSSubscriber *subscriber = participant
                ->create_subscriber(subscriber_qos, NULL, DDS_STATUS_MASK_NONE);
        if (subscriber == NULL) {
            return shutdown_participant(
                participant,
                "create_subscriber error",
                EXIT_FAILURE);
        }
    */
    /* End changes for coherent_presentation */

    // Register the datatype to use when creating the Topic
    const char *type_name = coherentTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            coherentTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example coherent",
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

    // This DataReader reads data on "Example coherent" Topic
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_ALL);
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_datareader call above.
     */
    /*    retcode = subscriber->get_default_datareader_qos(datareader_qos);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "get_default_datareader_qos error\n";
            return EXIT_FAILURE;
        }

        datareader_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
        datareader_qos.history.depth = 10;

        DDSDataReader *untyped_reader = subscriber->create_datareader(
                topic,
                datareader_qos,
                reader_listener,
                DDS_STATUS_MASK_ALL);
        if (untyped_reader == NULL) {
            return shutdown_participant(
                    participant,
                    "create_datareader error",
                    EXIT_FAILURE);
        }

    */    /* End changes for coherent_presentation */

    // Narrow casts from a untyped DataReader to a reader of your type
    coherentDataReader *typed_reader =
            coherentDataReader::narrow(untyped_reader);
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

        // Wait for data
        retcode = waitset.wait(active_conditions_seq, wait_timeout);

        if (retcode == DDS_RETCODE_OK) {
            // If the read condition is triggered, process data
            samples_read += process_data(typed_reader);
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
