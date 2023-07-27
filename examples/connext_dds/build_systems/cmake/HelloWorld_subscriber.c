/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 *
 * A subscription example.
 *
 * (1) Compile this file and the example publication.
 *
 * (2) Start the subscription on the same domain used for RTI Data Distribution
 * Service  with the command
 * HelloWorld_subscriber <domain_id> <sample_count>
 *
 * (3) Start the publication on the same domain used for RTI Data Distribution
 * Service with the command
 * HelloWorld_publisher <domain_id> <sample_count>
 *
 * (4) [Optional] Specify the list of discovery initial peers and
 * multicast receive addresses via an environment variable or a file
 * (in the current working directory) called NDDS_DISCOVERY_PEERS.
 *
 * You can run any number of publishers and subscribers programs, and can
 * add and remove them dynamically from the domain.
 *
 */

#include "HelloWorld.h"
#include "HelloWorldSupport.h"
#include "ndds/ndds_c.h"
#include <stdio.h>
#include <stdlib.h>

void HelloWorldListener_on_requested_deadline_missed(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_RequestedDeadlineMissedStatus *status)
{
}

void HelloWorldListener_on_requested_incompatible_qos(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_RequestedIncompatibleQosStatus *status)
{
}

void HelloWorldListener_on_sample_rejected(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SampleRejectedStatus *status)
{
}

void HelloWorldListener_on_liveliness_changed(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_LivelinessChangedStatus *status)
{
}

void HelloWorldListener_on_sample_lost(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SampleLostStatus *status)
{
}

void HelloWorldListener_on_subscription_matched(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SubscriptionMatchedStatus *status)
{
}

void HelloWorldListener_on_data_available(
        void *listener_data,
        DDS_DataReader *reader)
{
    HelloWorldDataReader *HelloWorld_reader = NULL;
    struct HelloWorldSeq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i;

    HelloWorld_reader = HelloWorldDataReader_narrow(reader);
    if (HelloWorld_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = HelloWorldDataReader_take(
            HelloWorld_reader,
            &data_seq,
            &info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);
    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "take error %d\n", retcode);
        return;
    }

    for (i = 0; i < HelloWorldSeq_get_length(&data_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
            printf("Received data\n");
            HelloWorldTypeSupport_print_data(
                    HelloWorldSeq_get_reference(&data_seq, i));
        }
    }

    retcode = HelloWorldDataReader_return_loan(
            HelloWorld_reader,
            &data_seq,
            &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

/* Delete all entities */
static int subscriber_shutdown(DDS_DomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = DDS_DomainParticipant_delete_contained_entities(participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDS_DomainParticipantFactory_delete_participant(
                DDS_TheParticipantFactory,
                participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Data Distribution Service provides the finalize_instance() method on
     * domain participant factory for users who want to release memory used
     * by the participant factory. Uncomment the following block of code for
     * clean destruction of the singleton.
     *
     * retcode = DDS_DomainParticipantFactory_finalize_instance();
     * if (retcode != DDS_RETCODE_OK) {
     *    fprintf(stderr, "finalize_instance error %d\n", retcode);
     *    status = -1;
     * }
     */

    return status;
}

int subscriber_main(int domainId, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Subscriber *subscriber = NULL;
    DDS_Topic *topic = NULL;
    struct DDS_DataReaderListener reader_listener =
            DDS_DataReaderListener_INITIALIZER;
    DDS_DataReader *reader = NULL;
    DDS_ReturnCode_t retcode;
    const char *type_name = NULL;
    int count = 0;
    struct DDS_Duration_t poll_period = { 4, 0 };

    /*
     * To customize participant QoS, use the configuration file
     * USER_QOS_PROFILES.xml
     */
    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            domainId,
            &DDS_PARTICIPANT_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /*
     * To customize subscriber QoS, use the configuration file
     * USER_QOS_PROFILES.xml
     */
    subscriber = DDS_DomainParticipant_create_subscriber(
            participant,
            &DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Register the type before creating the topic */
    type_name = HelloWorldTypeSupport_get_type_name();
    retcode = HelloWorldTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    /*
     * To customize topic QoS, use the configuration file USER_QOS_PROFILES.xml
     */
    topic = DDS_DomainParticipant_create_topic(
            participant,
            "Example HelloWorld",
            type_name,
            &DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Set up a data reader listener */
    reader_listener.on_requested_deadline_missed =
            HelloWorldListener_on_requested_deadline_missed;
    reader_listener.on_requested_incompatible_qos =
            HelloWorldListener_on_requested_incompatible_qos;
    reader_listener.on_sample_rejected = HelloWorldListener_on_sample_rejected;
    reader_listener.on_liveliness_changed =
            HelloWorldListener_on_liveliness_changed;
    reader_listener.on_sample_lost = HelloWorldListener_on_sample_lost;
    reader_listener.on_subscription_matched =
            HelloWorldListener_on_subscription_matched;
    reader_listener.on_data_available = HelloWorldListener_on_data_available;

    /*
     * To customize data reader QoS, use the configuration file
     * USER_QOS_PROFILES.xml
     */
    reader = DDS_Subscriber_create_datareader(
            subscriber,
            DDS_Topic_as_topicdescription(topic),
            &DDS_DATAREADER_QOS_DEFAULT,
            &reader_listener,
            DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        fprintf(stderr, "create_datareader error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Main loop */
    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("HelloWorld subscriber sleeping for %d sec...\n",
               poll_period.sec);

        NDDS_Utility_sleep(&poll_period);
    }

    /* Cleanup and delete all entities */
    return subscriber_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /*
     * Uncomment this to turn on additional logging
     * NDDS_Config_Logger_set_verbosity_by_category(
     *         NDDS_Config_Logger_get_instance(),
     *         NDDS_CONFIG_LOG_CATEGORY_API,
     *         NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return subscriber_main(domain_id, sample_count);
}
