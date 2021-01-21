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
 * A simple secure Hello World using network capture to save DomainParticipant
 * traffic from several participants.
 *
 * It shows how, if we have several participants, we can control from which
 * we want to capture traffic and when.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"
#include "example3.h"
#include "example3Support.h"
#ifdef RTI_STATIC
#include "security/security_default.h"
#endif

void example3Listener_on_requested_deadline_missed(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_RequestedDeadlineMissedStatus *status)
{
}

void example3Listener_on_requested_incompatible_qos(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_RequestedIncompatibleQosStatus *status)
{
}

void example3Listener_on_sample_rejected(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SampleRejectedStatus *status)
{
}

void example3Listener_on_liveliness_changed(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_LivelinessChangedStatus *status)
{
}

void example3Listener_on_sample_lost(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SampleLostStatus *status)
{
}

void example3Listener_on_subscription_matched(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SubscriptionMatchedStatus *status)
{
}

void example3Listener_on_data_available(
        void* listener_data,
        DDS_DataReader* reader)
{
    example3DataReader *example3_reader = NULL;
    struct example3Seq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i;

    example3_reader = example3DataReader_narrow(reader);
    if (example3_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = example3DataReader_take(
            example3_reader,
            &data_seq,
            &info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);
    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "take error %d\n", retcode);
        return;
    }

    for (i = 0; i < example3Seq_get_length(&data_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
            printf("Received data:\n");
            example3TypeSupport_print_data(
                    example3Seq_get_reference(&data_seq, i));
        }
    }

    retcode = example3DataReader_return_loan(
            example3_reader,
            &data_seq,
            &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

static int subscriber_shutdown(
        DDS_DomainParticipant *participant,
        DDS_Boolean last_participant)
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

    if (last_participant) {
        retcode = DDS_DomainParticipantFactory_finalize_instance();
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "finalize_instance error %d\n", retcode);
            status = -1;
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
    }
    return status;
}

static int subscriber_main(int domainId, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_DomainParticipant *participant2 = NULL;
    DDS_Subscriber *subscriber = NULL;
    DDS_Subscriber *subscriber2 = NULL;
    DDS_Subscriber *subscriber3 = NULL;
    DDS_Topic *topic = NULL;
    DDS_Topic *topic2 = NULL;
    struct DDS_DataReaderListener reader_listener =
    DDS_DataReaderListener_INITIALIZER;
    DDS_DataReader *reader = NULL;
    DDS_DataReader *reader2 = NULL;
    DDS_DataReader *reader3 = NULL;
    DDS_ReturnCode_t retcode;
    const char *type_name = NULL;
    const char *type_name2 = NULL;
    int count = 0;
    struct DDS_Duration_t poll_period = {0, 10000000}; /* 10 ms */
    DDS_Boolean success = DDS_BOOLEAN_FALSE;
    NDDS_Utility_NetworkCaptureParams_t params =
            NDDS_UTILITY_NETWORK_CAPTURE_PARAMETERS_DEFAULT;

    /*
     * Enable network capture.
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
     * Configuration of the capture for the second participant.
     *   - Parse encrypted content.
     *   - Only capture shared memory transport.
     */
    params.parse_encrypted_content = DDS_BOOLEAN_TRUE;
    DDS_StringSeq_ensure_length(&params.transports, 1, 128);
    *DDS_StringSeq_get_reference(&params.transports, 0) =
            DDS_String_dup("shmem");

    /*
     * Or you could add also udpv4 by using instead:
     * DDS_StringSeq_ensure_length(&params.transports, 2, 128);
     * *DDS_StringSeq_get_reference(&params.transports, 0) =
     *         DDS_String_dup("shmem");
     * *DDS_StringSeq_get_reference(&params.transports, 1) =
     *         DDS_String_dup("udpv4");
     */

    /*
     * The example continues as the usual Connext DDS secure Hello World.
     * Create entities, start communication, etc.
     *
     * The peculiarity here is that we are creating two participants, each one
     * with its own subscriber (participant2 has two) and joining to a different
     * topic.
     */
#ifdef RTI_STATIC
    struct DDS_DomainParticipantQos participant_qos =
            DDS_DomainParticipantQos_INITIALIZER;

    retcode = DDS_DomainParticipantFactory_get_participant_qos_from_profile(
            DDS_TheParticipantFactory,
            &participant_qos,
            "SecurityExampleProfiles",
            "A");
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "Unable to get default participant qos\n");
        return -1;
    }
    retcode = DDS_PropertyQosPolicyHelper_assert_pointer_property(
            &participant_qos.property,
            RTI_SECURITY_BUILTIN_PLUGIN_PROPERTY_NAME ".create_function_ptr",
            RTI_Security_PluginSuite_create);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "Unable to assert create_function_ptr property\n");
        return -1;
    }

    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            domainId,
            &participant_qos,
            NULL,
            DDS_STATUS_MASK_NONE);
    participant2 = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            domainId,
            &participant_qos,
            NULL,
            DDS_STATUS_MASK_NONE);
    DDS_DomainParticipantQos_finalize(&participant_qos);
#else
    participant = DDS_DomainParticipantFactory_create_participant_with_profile(
            DDS_TheParticipantFactory,
            domainId,
            "SecurityExampleProfiles",
            "A",
            NULL,
            DDS_STATUS_MASK_NONE);
    participant2 = DDS_DomainParticipantFactory_create_participant_with_profile(
            DDS_TheParticipantFactory,
            domainId,
            "SecurityExampleProfiles",
            "A",
            NULL,
            DDS_STATUS_MASK_NONE);
#endif
    if (participant == NULL || participant2 == NULL) {
        fprintf(stderr, "create_participant error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    subscriber = DDS_DomainParticipant_create_subscriber(
            participant,
            &DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    subscriber2 = DDS_DomainParticipant_create_subscriber(
            participant2,
            &DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (subscriber2 == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    subscriber3 = DDS_DomainParticipant_create_subscriber(
            participant2,
            &DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (subscriber3 == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    type_name = example3TypeSupport_get_type_name();
    retcode = example3TypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    type_name2 = example3TypeSupport_get_type_name();
    retcode = example3TypeSupport_register_type(participant2, type_name2);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    topic = DDS_DomainParticipant_create_topic(
            participant,
            "Example3 [1st Participant]",
            type_name,
            &DDS_TOPIC_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    topic2 = DDS_DomainParticipant_create_topic(
            participant2,
            "Example3 [2nd Participant]",
            type_name2,
            &DDS_TOPIC_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (topic2 == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    /*
     * Start capturing traffic.
     * Default parameters for first participant and configured ones for the
     * second participant.
     */
    success = NDDS_Utility_start_network_capture_for_participant(
            participant,
            "subscriberParticipant1");
    if (!success) {
        fprintf(stderr, "Error starting network capture for 1st participant\n");
    }

    success = NDDS_Utility_start_network_capture_w_params_for_participant(
            participant2,
            "subscriberParticipant2",
            &params);
    if (!success) {
        fprintf(stderr, "Error starting network monitor for 2nd participant\n");
    }

    reader_listener.on_requested_deadline_missed  =
    example3Listener_on_requested_deadline_missed;
    reader_listener.on_requested_incompatible_qos =
    example3Listener_on_requested_incompatible_qos;
    reader_listener.on_sample_rejected =
    example3Listener_on_sample_rejected;
    reader_listener.on_liveliness_changed =
    example3Listener_on_liveliness_changed;
    reader_listener.on_sample_lost =
    example3Listener_on_sample_lost;
    reader_listener.on_subscription_matched =
    example3Listener_on_subscription_matched;
    reader_listener.on_data_available =
    example3Listener_on_data_available;

    reader = DDS_Subscriber_create_datareader(
        subscriber, DDS_Topic_as_topicdescription(topic),
        &DDS_DATAREADER_QOS_DEFAULT, &reader_listener, DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        fprintf(stderr, "create_datareader error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    reader2 = DDS_Subscriber_create_datareader(
        subscriber2, DDS_Topic_as_topicdescription(topic2),
        &DDS_DATAREADER_QOS_DEFAULT, &reader_listener, DDS_STATUS_MASK_ALL);
    if (reader2 == NULL) {
        fprintf(stderr, "create_datareader error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    reader3 = DDS_Subscriber_create_datareader(
        subscriber3, DDS_Topic_as_topicdescription(topic2),
        &DDS_DATAREADER_QOS_DEFAULT, &reader_listener, DDS_STATUS_MASK_ALL);
    if (reader3 == NULL) {
        fprintf(stderr, "create_datareader error\n");
        subscriber_shutdown(participant, DDS_BOOLEAN_FALSE);
        subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("example3 subscriber sleeping for %d sec...\n", poll_period.sec);

        /*
         * Here we are going to pause capturing for the second participant
         * The resulting pcap file will not contain them.
         *
         * Note: the count here is independent of the count in the publisher, so
         * the numbers will not exactly match.
         */
        if (count == 4 && !NDDS_Utility_pause_network_capture_for_participant(
                participant2)) {
            fprintf(stderr, "Error pausing network capture\n");
        } else if (count == 6 && !NDDS_Utility_resume_network_capture()) {
            /*
             * This will resume network capture for the second participant
             * (the first is already running).
             */
            fprintf(stderr, "Error resuming network capture\n");
        }

        NDDS_Utility_sleep(&poll_period);
    }

    /*
     * Before deleting the participants that are capturing, we must stop
     * network capture for them.
     */
    if (!NDDS_Utility_stop_network_capture_for_participant(participant)) {
        fprintf(stderr, "Error stopping network capture for 1st participant\n");
    }
    if (!NDDS_Utility_stop_network_capture) {
        /* First participant is already stopped, so this stops only second */
        fprintf(stderr, "Error stopping network capture\n");
    }

    /* Finalize the parameters to free allocated memory */
    NDDS_Utility_NetworkCaptureParams_t_finalize(&params);

    return subscriber_shutdown(participant, DDS_BOOLEAN_FALSE)
            && subscriber_shutdown(participant2, DDS_BOOLEAN_TRUE);
}

int main(int argc, char *argv[])
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    return subscriber_main(domainId, sample_count);
}

