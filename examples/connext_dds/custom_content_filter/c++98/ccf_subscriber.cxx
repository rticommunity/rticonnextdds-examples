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

#include "ccf.h"
#include "ccfSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

/* Custom filter defined here */
#include "filter.cxx"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        custom_filter_type *custom_filter,
        const char *shutdown_message,
        int status);

unsigned int process_data(ccfDataReader *typed_reader)
{
    ccfSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    unsigned int samples_read = 0;

    typed_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    for (int i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            ccfTypeSupport::print_data(&data_seq[i]);
            samples_read++;
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
    DDS_Duration_t wait_timeout = { 1, 0 };

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
                NULL,
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
                NULL,
                "create_subscriber error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = ccfTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            ccfTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                NULL,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example ccf",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(
                participant,
                NULL,
                "create_topic error",
                EXIT_FAILURE);
    }
    /* Start changes for Custom Content Filter */
    /* Create and register custom filter */

    custom_filter_type *custom_filter = new custom_filter_type();
    retcode =
            participant->register_contentfilter("CustomFilter", custom_filter);

    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                custom_filter,
                "Failed to register custom content filter",
                EXIT_FAILURE);
    }

    DDS_StringSeq parameters(2);
    const char *param_list[] = { "2", "divides" };
    parameters.from_array(param_list, 2);

    // Create content filtered topic
    DDSContentFilteredTopic *cft =
            participant->create_contentfilteredtopic_with_filter(
                    "ContentFilteredTopic",
                    topic,
                    "%0 %1 x",
                    parameters,
                    "CustomFilter");
    if (cft == NULL) {
        return shutdown_participant(
                participant,
                custom_filter,
                "create_contentfilteredtopic error",
                EXIT_FAILURE);
    }

    std::cout << "Filter: 2 divides x\n";

    /* Also note that we pass 'cft' rather than 'topic' to the datareader
     * below
     */
    /* End changes for Custom_Content_Filter */

    /*
     * NOTE THAT WE USE THE PREVIOUSLY CREATED CUSTOM FILTERED TOPIC TO READ
     * NEW SAMPLES
     */
    // This DataReader reads data on "Example ccf" Topic
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            cft,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_ALL);
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                custom_filter,
                "create_datareader error",
                EXIT_FAILURE);
    }

    // Narrow casts from a untyped DataReader to a reader of your type
    ccfDataReader *typed_reader = ccfDataReader::narrow(untyped_reader);
    if (typed_reader == NULL) {
        return shutdown_participant(
                participant,
                custom_filter,
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
                custom_filter,
                "create_readcondition error",
                EXIT_FAILURE);
    }

    // WaitSet will be woken when the attached condition is triggered
    DDSWaitSet waitset;
    retcode = waitset.attach_condition(read_condition);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                custom_filter,
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

        if (samples_read == 4) {
            std::cout << "changing filter parameters\n";
            std::cout << "Filter: 15 greater-than x\n";
            DDS_String_free(parameters[0]);
            DDS_String_free(parameters[1]);
            parameters[0] = DDS_String_dup("15");
            parameters[1] = DDS_String_dup("greater-than");
            retcode = cft->set_expression_parameters(parameters);
            if (retcode != DDS_RETCODE_OK) {
                return shutdown_participant(
                        participant,
                        custom_filter,
                        "set_expression_parameters error",
                        EXIT_FAILURE);
            }
        } else if (samples_read == 10) {
            std::cout << "changing filter parameters\n";
            std::cout << "Filter: 3 divides x\n";
            DDS_StringSeq oldParameters;
            cft->get_expression_parameters(oldParameters);

            DDS_String_free(oldParameters[0]);
            DDS_String_free(oldParameters[1]);
            oldParameters[0] = DDS_String_dup("3");
            oldParameters[1] = DDS_String_dup("divides");
            retcode = cft->set_expression_parameters(oldParameters);
            if (retcode != DDS_RETCODE_OK) {
                return shutdown_participant(
                        participant,
                        custom_filter,
                        "set_expression_parameters error",
                        EXIT_FAILURE);
            }
        }
    }
    /* End Topics/ContentFilter changes */

    // delete custom_filter;

    // Cleanup
    return shutdown_participant(participant, custom_filter, "Shutting down", 0);
}

// Delete all entities
static int shutdown_participant(
        DDSDomainParticipant *participant,
        custom_filter_type *custom_filter,
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

    if (custom_filter != NULL)
        delete custom_filter;

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
