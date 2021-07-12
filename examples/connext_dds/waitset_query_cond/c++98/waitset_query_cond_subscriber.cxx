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

#include "ndds/ndds_cpp.h"
#include "waitset_query_cond.h"
#include "waitset_query_condSupport.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

/* We don't need to use listeners as we are going to use WaitSet and Conditions
 * so we have removed the auto generated code for listeners here
 */

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    DDS_Duration_t wait_timeout = { 1, 0 };
    const char *query_expression = DDS_String_dup("name MATCH %0");
    char *odd_string = DDS_String_dup("'ODD'");
    char *even_string = DDS_String_dup("'EVEN'");


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
    const char *type_name = waitset_query_condTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode = waitset_query_condTypeSupport::register_type(
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
            "Example waitset_query_cond",
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

    // This DataReader reads data on "Example waitset_query_cond" Topic
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

    // Narrow casts from a untyped DataReader to a reader of your type
    waitset_query_condDataReader *typed_reader =
            waitset_query_condDataReader::narrow(untyped_reader);
    if (typed_reader == NULL) {
        return shutdown_participant(
                participant,
                "DataReader narrow error",
                EXIT_FAILURE);
    }

    // Create query condition
    DDS_StringSeq query_parameters;
    query_parameters.ensure_length(1, 1);
    // The initial value of the parameters is EVEN string
    query_parameters[0] = even_string;
    DDSQueryCondition *query_condition = typed_reader->create_querycondition(
            DDS_NOT_READ_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE,
            query_expression,
            query_parameters);
    if (query_condition == NULL) {
        return shutdown_participant(
                participant,
                "create_query_condition narrow error",
                EXIT_FAILURE);
    }

    DDSWaitSet *waitset = new DDSWaitSet();
    if (waitset == NULL) {
        return shutdown_participant(
                participant,
                "create waitset error",
                EXIT_FAILURE);
    }

    // Attach Query Conditions
    retcode = waitset->attach_condition((DDSCondition *) query_condition);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "attach_condition error",
                EXIT_FAILURE);
    }

    std::cout << "\n>>>Timeout: " << wait_timeout.sec << " sec & "
              << wait_timeout.nanosec << " nanosec\n";
    std::cout << ">>> Query conditions: name MATCH %%0\n";
    std::cout << "\t%%0 = " << query_parameters[0] << std::endl;
    std::cout << "---------------------------------\n\n";

    // Main loop. Wait for data to arrive, and process when it arrives
    unsigned int samples_read = 0;
    int count = 0;
    DDS_SampleInfoSeq info_seq;
    waitset_query_condSeq data_seq;
    DDSConditionSeq active_conditions_seq;
    while (!shutdown_requested && samples_read < sample_count) {
        count++;
        // We set a new parameter in the Query Condition after 7 secs
        if (count == 7) {
            query_parameters[0] = odd_string;
            std::cout << "CHANGING THE QUERY CONDITION\n";
            std::cout << "\n>>> Query conditions: name MATCH %%0\n";
            std::cout << "\t%%0 = " << query_parameters[0] << std::endl;
            std::cout << ">>> We keep one sample in the history\n";
            std::cout << "-------------------------------------\n\n";
            query_condition->set_query_parameters(query_parameters);
        }

        /* wait() blocks execution of the thread until one or more attached
         * Conditions become true, or until a user-specified timeout expires.
         */
        retcode = waitset->wait(
                active_conditions_seq, /* active conditions sequence */
                wait_timeout);         /* timeout */

        /* We get to timeout if no conditions were triggered */
        if (retcode == DDS_RETCODE_TIMEOUT) {
            std::cout << "Wait timed out!! No conditions were triggered.\n";
            continue;
        } else if (retcode != DDS_RETCODE_OK) {
            std::cerr << "wait returned error: " << retcode << std::endl;
            break;
        }

        retcode = DDS_RETCODE_OK;
        while (retcode != DDS_RETCODE_NO_DATA) {
            retcode = typed_reader->take_w_condition(
                    data_seq,
                    info_seq,
                    DDS_LENGTH_UNLIMITED,
                    query_condition);

            for (int i = 0; i < waitset_query_condSeq_get_length(&data_seq);
                 ++i) {
                if (!info_seq[i].valid_data) {
                    std::cout << "Got metadata\n";
                    continue;
                }
                waitset_query_condTypeSupport::print_data(&data_seq[i]);
                samples_read++;
            }
            typed_reader->return_loan(data_seq, info_seq);
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
