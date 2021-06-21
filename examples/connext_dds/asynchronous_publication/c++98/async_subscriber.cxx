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

#include "async.h"
#include "asyncSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

#include <time.h>
clock_t init;

using namespace application;

static int shutdown_participant(
    DDSDomainParticipant *participant,
    const char *shutdown_message,
    int status);

// Process data. Returns number of samples processed.
unsigned int process_data(asyncDataReader *typed_reader)
{
    asyncSeq data_seq;     // Sequence of received data
    DDS_SampleInfoSeq info_seq; // Metadata associated with samples in data_seq
    unsigned int samples_read = 0;

    // Take available data from DataReader's queue
    typed_reader->take(
        data_seq,
        info_seq,
        DDS_LENGTH_UNLIMITED,
        DDS_ANY_SAMPLE_STATE,
        DDS_ANY_VIEW_STATE,
        DDS_ANY_INSTANCE_STATE);

    for (int i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            samples_read++;
            //// Start changes for Asynchronous_Publication
            // print the time we get each sample.
            double elapsed_ticks = clock() - init;
            double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

            std::cout << "@ t=" << elapsed_secs << ", got x = " << data_seq[i].x << std::endl;

            //// End changes for Asynchronous_Publication
        }
    }

    // Data loaned from Connext for performance. Return loan when done.
    DDS_ReturnCode_t retcode = typed_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "return loan error " << retcode << std::endl;
    }

    return samples_read;
}

int run_subscriber_application(unsigned int domain_id, unsigned int sample_count)
{
    //// Changes for Asynchronous_Publication
    // for timekeeping
    init = clock();

    // To customize the participant QoS, use the configuration file
    // USER_QOS_PROFILES.xml
    DDSDomainParticipant *participant =
    DDSTheParticipantFactory->create_participant(
        domain_id,
        DDS_PARTICIPANT_QOS_DEFAULT,
        NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(participant, "create_participant error", EXIT_FAILURE);
    }

    // To customize the subscriber QoS, use the configuration file 
    // USER_QOS_PROFILES.xml
    DDSSubscriber *subscriber = participant->create_subscriber(
        DDS_SUBSCRIBER_QOS_DEFAULT,
        NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        return shutdown_participant(participant, "create_subscriber error", EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = asyncTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
    asyncTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(participant, "register_type error", EXIT_FAILURE);
    }

    // To customize the topic QoS, use the configuration file
    // USER_QOS_PROFILES.xml
    DDSTopic *topic = participant->create_topic(
        "Example async",
        type_name,
        DDS_TOPIC_QOS_DEFAULT,
        NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(participant, "create_topic error", EXIT_FAILURE);
    }

    // To customize the data reader QoS, use the configuration file
    // USER_QOS_PROFILES.xml
    DDSDataReader *untyped_reader = subscriber->create_datareader(
        topic,
        DDS_DATAREADER_QOS_DEFAULT,
        NULL,
        DDS_STATUS_MASK_NONE);
    if (untyped_reader == NULL) {
        return shutdown_participant(participant, "create_datareader error", EXIT_FAILURE);
    }

    // Narrow casts from a untyped DataReader to a reader of your type
    asyncDataReader *typed_reader =
    asyncDataReader::narrow(untyped_reader);
    if (typed_reader == NULL) {
        return shutdown_participant(participant, "DataReader narrow error", EXIT_FAILURE);
    }

    // Create ReadCondition that triggers when unread data in reader's queue
    DDSReadCondition *read_condition = typed_reader->create_readcondition(
        DDS_NOT_READ_SAMPLE_STATE,
        DDS_ANY_VIEW_STATE,
        DDS_ANY_INSTANCE_STATE);
    if (read_condition == NULL) {
        return shutdown_participant(participant, "create_readcondition error", EXIT_FAILURE);
    }

    // WaitSet will be woken when the attached condition is triggered
    DDSWaitSet waitset;
    retcode = waitset.attach_condition(read_condition);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(participant, "attach_condition error", EXIT_FAILURE);
    }

    // Main loop. Wait for data to arrive, and process when it arrives
    unsigned int samples_read = 0;
    while (!shutdown_requested && samples_read < sample_count) {
        DDSConditionSeq active_conditions_seq;

        // Wait for data and report if it does not arrive in 4 seconds
        DDS_Duration_t wait_timeout = { 4, 0 };
        std::cout << "async subscriber sleeping for " << wait_timeout.sec << " sec...\n";
        retcode = waitset.wait(active_conditions_seq, wait_timeout);

        if (retcode == DDS_RETCODE_OK) {
            // If the read condition is triggered, process data
            samples_read += process_data(typed_reader);
        } else if (retcode == DDS_RETCODE_TIMEOUT) {
            std::cout << "No data after 4 seconds" << std::endl;
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

    int status = run_subscriber_application(arguments.domain_id, arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error" << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
