/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

#include "flights.h"
#include "flightsSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

/* We remove all the listener code as we won't use any listener */

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    /* Poll for new samples every second. */
    DDS_Duration_t receive_period = { 1, 0 };

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
    const char *type_name = FlightTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            FlightTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example Flight",
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

    // This DataReader reads data on "Example flights" Topic
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

    FlightDataReader *typed_reader = FlightDataReader::narrow(untyped_reader);
    if (typed_reader == NULL) {
        return shutdown_participant(
                participant,
                "DataReader narrow error",
                EXIT_FAILURE);
    }

    /* Query for company named 'CompanyA' and for flights in cruise
     * (about 30,000ft). The company parameter will be changed in run-time.
     * NOTE: There must be single-quotes in the query parameters around-any
     * strings! The single-quote do NOT go in the query condition itself. */
    DDS_StringSeq query_parameters;
    query_parameters.ensure_length(2, 2);
    query_parameters[0] = (char *) "'CompanyA'";
    query_parameters[1] = (char *) "30000";
    std::cout << "Setting parameter to company " << query_parameters[0]
              << ", altitude bigger or equals to " << query_parameters[1]
              << std::endl;

    /* Create the query condition with an expession to MATCH the id field in
     * the structure and a numeric comparison. Note that you should make a copy
     * of the expression string when creating the query condition - beware it
     * going out of scope! */
    DDSQueryCondition *query_condition = typed_reader->create_querycondition(
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ALIVE_INSTANCE_STATE,
            DDS_String_dup("company MATCH %0 AND altitude >= %1"),
            query_parameters);

    // Main loop. Wait for data to arrive, and process when it arrives
    int iteration_count = 0;
    unsigned int samples_read = 0;
    while (!shutdown_requested && samples_read < sample_count) {
        // Poll for a new samples every second.
        NDDSUtility::sleep(receive_period);
        bool update = false;

        // Change the filter parameter after 5 seconds.
        if ((iteration_count + 1) % 10 == 5) {
            query_parameters[0] = (char *) "'CompanyB'";
            update = true;
        } else if ((iteration_count + 1) % 10 == 0) {
            query_parameters[0] = (char *) "'CompanyA'";
            update = true;
        }
        iteration_count++;

        // Set new parameters.
        if (update) {
            std::cout << "Changing parameter to " << query_parameters[0]
                      << std::endl;
            query_condition->set_query_parameters(query_parameters);
        }

        DDS_SampleInfoSeq info_seq;
        FlightSeq data_seq;

        // Iterate through the samples read using the read_w_condition method
        retcode = typed_reader->read_w_condition(
                data_seq,
                info_seq,
                DDS_LENGTH_UNLIMITED,
                query_condition);
        if (retcode == DDS_RETCODE_NO_DATA) {
            // Not an error
            continue;
        } else if (retcode != DDS_RETCODE_OK) {
            // Is an error
            std::cerr << "take error: " << retcode << std::endl;
            break;
        }

        for (int i = 0; i < data_seq.length(); ++i) {
            if (info_seq[i].valid_data) {
                std::cout << "\t[trackId: " << data_seq[i].trackId
                          << ", company: " << data_seq[i].company
                          << ", altitude: " << data_seq[i].altitude << "]\n";

                samples_read++;
            }
        }

        retcode = typed_reader->return_loan(data_seq, info_seq);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "return loan error " << retcode << std::endl;
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
