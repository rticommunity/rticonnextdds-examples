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

#include "ordered_group.h"
#include "ordered_groupSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

class ordered_groupSubscriberListener : public DDSSubscriberListener {
public:
    virtual void on_data_on_readers(DDSSubscriber *subscriber);
};

void ordered_groupSubscriberListener::on_data_on_readers(
        DDSSubscriber *subscriber)
{
    DDSDataReaderSeq MyDataReaders;
    DDS_ReturnCode_t retcode;
    int i;

    /* IMPORTANT for GROUP access scope: Invoking begin_access() */
    subscriber->begin_access();

    /* Obtain DataReaders. We obtain a sequence of DataReaders that specifies
                the order in which each sample should be read */
    retcode = subscriber->get_datareaders(
            MyDataReaders,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "ERROR error " << retcode << std::endl;
        /* IMPORTANT. Remember to invoke end_access() before a return call.
                        Also reset DataReaders sequence */
        MyDataReaders.ensure_length(0, 0);
        subscriber->end_access();
        return;
    }

    /* Read the samples received, following the DataReaders sequence */
    for (i = 0; i < MyDataReaders.length(); i++) {
        ordered_groupDataReader *ordered_group_reader = NULL;
        ordered_group data;
        DDS_SampleInfo info;
        ordered_group_initialize(&data);

        ordered_group_reader =
                ordered_groupDataReader::narrow(MyDataReaders.get_at(i));
        if (ordered_group_reader == NULL) {
            std::cerr << "DataReader narrow error\n";
            /* IMPORTANT. Remember to invoke end_access() before a return call.
                                Also reset DataReaders sequence */
            MyDataReaders.ensure_length(0, 0);
            subscriber->end_access();
            return;
        }

        /* IMPORTANT. Use take_next_sample(). We need to take only
         * one sample each time, as we want to follow the sequence of
         * DataReaders. This way the samples will be returned in the
         * order in which they were modified */
        retcode = ordered_group_reader->take_next_sample(data, info);

        /* In case there is no data in current DataReader,
                        check next in the sequence */
        if (retcode == DDS_RETCODE_NO_DATA) {
            continue;
        } else if (retcode != DDS_RETCODE_OK) {
            std::cerr << "take error %d\n", retcode;
            continue;
        }

        /* Print data sample */
        if (info.valid_data) {
            ordered_groupTypeSupport::print_data(&data);
        }
    }
    /* Reset DataReaders sequence */
    MyDataReaders.ensure_length(0, 0);

    /* IMPORTANT for GROUP access scope: Invoking end_access() */
    subscriber->end_access();
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
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

    ordered_groupSubscriberListener *subscriber_listener =
            new ordered_groupSubscriberListener();

    // A Subscriber allows an application to create one or more DataReaders
    DDSSubscriber *subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            subscriber_listener,
            DDS_DATA_ON_READERS_STATUS);
    if (subscriber == NULL) {
        return shutdown_participant(
                participant,
                "create_subscriber error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = ordered_groupTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            ordered_groupTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    /* TOPICS */

    DDSTopic *topic1 = participant->create_topic(
            "Topic1",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic1 == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    DDSTopic *topic2 = participant->create_topic(
            "Topic2",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic2 == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    DDSTopic *topic3 = participant->create_topic(
            "Topic3",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic3 == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    /* DATAREADERS */

    DDSDataReader *untyped_reader1 = subscriber->create_datareader(
            topic1,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_NONE);
    if (untyped_reader1 == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    DDSDataReader *untyped_reader2 = subscriber->create_datareader(
            topic2,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_NONE);
    if (untyped_reader2 == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    DDSDataReader *untyped_reader3 = subscriber->create_datareader(
            topic3,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_NONE);
    if (untyped_reader3 == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    // Main loop. Wait for data to arrive, and process when it arrives
    for (int count = 0; count < sample_count && !shutdown_requested; ++count) {
        std::cout << "ordered_group subscriber sleeping for "
                  << receive_period.sec << " sec...\n";

        NDDSUtility::sleep(receive_period);
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
