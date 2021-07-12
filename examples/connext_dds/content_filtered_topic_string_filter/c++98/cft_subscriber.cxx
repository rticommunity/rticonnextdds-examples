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

#include "cft.h"
#include "cftSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

unsigned int process_data(cftDataReader *typed_reader)
{
    cftSeq data_seq;
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
            cftTypeSupport::print_data(&data_seq[i]);
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
        unsigned int sample_count,
        bool normal_topic)
{
    DDS_Duration_t wait_timeout = { 1, 0 };

    /* For this filter we only allow 1 parameter*/
    DDS_StringSeq parameters(1);
    const char *param_list[] = { "SOME_STRING" };

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

    // Register the datatype to use when creating the Topic
    const char *type_name = cftTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            cftTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example cft",
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

    parameters.from_array(param_list, 1);

    DDSContentFilteredTopic *cft = NULL;
    if (!normal_topic) {
        /* create_contentfilteredtopic_with_filter */
        cft = participant->create_contentfilteredtopic_with_filter(
                "ContentFilteredTopic",
                topic,
                "name MATCH %0",
                parameters,
                DDS_STRINGMATCHFILTER_NAME);
        if (cft == NULL) {
            return shutdown_participant(
                    participant,
                    "create_contentfilteredtopic error",
                    EXIT_FAILURE);
        }
    }

    /* Here we create the reader either using a Content Filtered Topic or
     * a normal topic */
    DDSDataReader *untyped_reader = NULL;
    if (!normal_topic) {
        std::cout << "Using ContentFiltered Topic\n";
        untyped_reader = subscriber->create_datareader(
                cft,
                DDS_DATAREADER_QOS_DEFAULT,
                NULL,
                DDS_STATUS_MASK_ALL);
    } else {
        std::cout << "Using Normal Topic\n";
        untyped_reader = subscriber->create_datareader(
                topic,
                DDS_DATAREADER_QOS_DEFAULT,
                NULL,
                DDS_STATUS_MASK_ALL);
    }
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }


    /* If you want to set the reliability and history QoS settings
     * programmatically rather than using the XML, you will need to add
     * the following lines to your code and comment out the
     * create_datareader calls above.
     */
    /*
        DDS_DataReaderQos datareader_qos;
        retcode = subscriber->get_default_datareader_qos(datareader_qos);
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                    participant,
                    "get_default_datareader_qos error",
                    EXIT_FAILURE);
        }

        datareader_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
        datareader_qos.durability.kind = DDS_TRANSIENT_LOCAL_DURABILITY_QOS;
        datareader_qos.history.kind = DDS_KEEP_LAST_HISTORY_QOS;
        datareader_qos.history.depth = 20;

        if (!normal_topic) {
            std::cout << "Using ContentFiltered Topic\n";
            untyped_reader = subscriber->create_datareader(
                cft, datareader_qos, NULL,
                DDS_STATUS_MASK_ALL);
        } else {
            std::cout << "Using Normal Topic\n";
            untyped_reader = subscriber->create_datareader(
                topic, datareader_qos, NULL,
                DDS_STATUS_MASK_ALL);
        }
        if (untyped_reader == NULL) {
            return shutdown_participant(
                    participant,
                    "create_datareader error",
                    EXIT_FAILURE);
        }

    */
    /* Change the filter */
    if (!normal_topic) {
        printf(">>> Now setting a new filter: name MATCH \"EVEN\"\n");
        retcode = cft->append_to_expression_parameter(0, "EVEN");
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                    participant,
                    "append_to_expression_parameter error",
                    EXIT_FAILURE);
        }
    }

    // Narrow casts from a untyped DataReader to a reader of your type
    cftDataReader *typed_reader = cftDataReader::narrow(untyped_reader);
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

        if (normal_topic) {
            continue;
        }
        if (samples_read == 10) {
            std::cout << "\n===========================\n";
            std::cout << "Changing filter parameters\n";
            std::cout << "Append 'ODD' filter\n";
            std::cout << "===========================\n";
            retcode = cft->append_to_expression_parameter(0, "ODD");
            if (retcode != DDS_RETCODE_OK) {
                return shutdown_participant(
                        participant,
                        "append_to_expression_parameter error",
                        EXIT_FAILURE);
            }
        } else if (samples_read == 20) {
            std::cout << "\n===========================\n";
            std::cout << "Changing filter parameters\n";
            std::cout << "Removing 'EVEN' filter \n";
            std::cout << "===========================\n";

            retcode = cft->remove_from_expression_parameter(0, "EVEN");
            if (retcode != DDS_RETCODE_OK) {
                return shutdown_participant(
                        participant,
                        "append_to_expression_parameter error",
                        EXIT_FAILURE);
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
    parse_arguments(arguments, argc, argv, subscriber);
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
            arguments.sample_count,
            arguments.normal_topic);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error" << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
