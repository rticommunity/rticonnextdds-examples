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

/*
 * A simple HelloWorld using network capture to save DomainParticipant traffic.
 *
 * This example is a simple hello world running network capture for both a
 * publisher participant (network_capture_publisher.c).and a subscriber
 * participant (network_capture_subscriber.c). It shows the basic flow when
 * working with network capture:
 *   - Enabling before anything else.
 *   - Start capturing traffic (for one or all participants).
 *   - Pause/resume capturing traffic (for one or all participants).
 *   - Stop capturing trafffic (for one or all participants).
 *   - Disable after everything else.
 */

#include "ndds/ndds_c.h"
#include "network_capture.h"
#include "network_captureSupport.h"
#include <stdio.h>
#include <stdlib.h>

void NetworkCaptureListener_on_requested_deadline_missed(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_RequestedDeadlineMissedStatus *status)
{
}

void NetworkCaptureListener_on_requested_incompatible_qos(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_RequestedIncompatibleQosStatus *status)
{
}

void NetworkCaptureListener_on_sample_rejected(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SampleRejectedStatus *status)
{
}

void NetworkCaptureListener_on_liveliness_changed(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_LivelinessChangedStatus *status)
{
}

void NetworkCaptureListener_on_sample_lost(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SampleLostStatus *status)
{
}

void NetworkCaptureListener_on_subscription_matched(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SubscriptionMatchedStatus *status)
{
}

void NetworkCaptureListener_on_data_available(
        void *listener_data,
        DDS_DataReader *reader)
{
    NetworkCaptureDataReader *NetworkCapture_reader = NULL;
    struct NetworkCaptureSeq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i;

    NetworkCapture_reader = NetworkCaptureDataReader_narrow(reader);
    if (NetworkCapture_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = NetworkCaptureDataReader_take(
            NetworkCapture_reader,
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

    for (i = 0; i < NetworkCaptureSeq_get_length(&data_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
            printf("Received data\n");
            NetworkCaptureTypeSupport_print_data(
                    NetworkCaptureSeq_get_reference(&data_seq, i));
        }
    }

    retcode = NetworkCaptureDataReader_return_loan(
            NetworkCapture_reader,
            &data_seq,
            &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

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

    /*
     * Disable network capture.
     *
     * This must be:
     *   - The last network capture function that is called.
     */
    if (!NDDS_Utility_disable_network_capture()) {
        fprintf(stderr, "Error disabling network capture\n");
        status = -1;
    }
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
    struct DDS_Duration_t poll_period = { 1, 0 }; /* 1 s */

    /*
     * Enable network capture
     *
     * This must be called before:
     *   - Any other network capture function is called.
     *   - Creating the participants for which we want to capture traffic.
     */
    if (!NDDS_Utility_enable_network_capture()) {
        fprintf(stderr, "Error enabling network capture\n");
        return -1;
    }

    /*
     * Start capturing traffic for all participants.
     *
     * All participants: those already created and those yet to be created.
     * Default parameters: all transports and some other sane defaults.
     *
     * A capture file will be created for each participant. The capture file
     * will start with the prefix "subscriber" and continue with a suffix
     * dependent on the participant's GUID.
     */
    if (!NDDS_Utility_start_network_capture("subscriber")) {
        fprintf(stderr,
                "Error starting network capture for all participants\n");
        return -1;
    }

    /*
     * The example continues as the usual Connext DDS HelloWorld.
     * Create entities, start communication, etc.
     */
    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            domainId,
            &DDS_PARTICIPANT_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    subscriber = DDS_DomainParticipant_create_subscriber(
            participant,
            &DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    type_name = NetworkCaptureTypeSupport_get_type_name();
    retcode = NetworkCaptureTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    topic = DDS_DomainParticipant_create_topic(
            participant,
            "Network capture shared memory example",
            type_name,
            &DDS_TOPIC_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    reader_listener.on_requested_deadline_missed =
            NetworkCaptureListener_on_requested_deadline_missed;
    reader_listener.on_requested_incompatible_qos =
            NetworkCaptureListener_on_requested_incompatible_qos;
    reader_listener.on_sample_rejected =
            NetworkCaptureListener_on_sample_rejected;
    reader_listener.on_liveliness_changed =
            NetworkCaptureListener_on_liveliness_changed;
    reader_listener.on_sample_lost = NetworkCaptureListener_on_sample_lost;
    reader_listener.on_subscription_matched =
            NetworkCaptureListener_on_subscription_matched;
    reader_listener.on_data_available =
            NetworkCaptureListener_on_data_available;

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

    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("NetworkCapture subscriber sleeping for %d sec...\n",
               poll_period.sec);

        NDDS_Utility_sleep(&poll_period);
    }

    /*
     * But before deleting the participants that are capturing, we must stop
     * network capture for them.
     */
    if (!NDDS_Utility_stop_network_capture()) {
        fprintf(stderr,
                "Error stopping network capture for all participants\n");
    }

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

    return subscriber_main(domain_id, sample_count);
}
