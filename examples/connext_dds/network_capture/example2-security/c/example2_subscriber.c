/*
 * A simple secure HelloWorld using network capture to save DomainParticipant
 * traffic.
 *
 * This example is a simple secure hello world running network capture for both
 * a publisher participant (example2_publisher.c).and a subscriber participant
 * (example2_subscriber.c).
 * It shows the basic flow when working with network capture:
 *   - Enabling before anything else.
 *   - Start capturing traffic (for one or all participants).
 *   - Pause/resume capturing traffic (for one or all participants).
 *   - Stop capturing trafffic (for one or all participants).
 *   - Disable after everything else.
 * It also initializes some configuration parameters before starting to capture
 * and finalizes when they are no longer in use.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"
#include "example2.h"
#include "example2Support.h"
#ifdef RTI_STATIC
#include "security/security_default.h"
#endif

void example2Listener_on_requested_deadline_missed(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_RequestedDeadlineMissedStatus *status)
{
}

void example2Listener_on_requested_incompatible_qos(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_RequestedIncompatibleQosStatus *status)
{
}

void example2Listener_on_sample_rejected(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SampleRejectedStatus *status)
{
}

void example2Listener_on_liveliness_changed(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_LivelinessChangedStatus *status)
{
}

void example2Listener_on_sample_lost(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SampleLostStatus *status)
{
}

void example2Listener_on_subscription_matched(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SubscriptionMatchedStatus *status)
{
}

void example2Listener_on_data_available(
        void* listener_data,
        DDS_DataReader* reader)
{
    example2DataReader *example2_reader = NULL;
    struct example2Seq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i;

    example2_reader = example2DataReader_narrow(reader);
    if (example2_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = example2DataReader_take(
            example2_reader,
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

    for (i = 0; i < example2Seq_get_length(&data_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
            printf("Received data:\n");
            example2TypeSupport_print_data(
                    example2Seq_get_reference(&data_seq, i));
            fflush(stdout);
        }
    }

    retcode = example2DataReader_return_loan(
            example2_reader,
            &data_seq,
            &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

static int subscriber_shutdown(
        DDS_DomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    DDS_Boolean success = DDS_BOOLEAN_FALSE;
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
    success = NDDS_Utility_disable_network_capture();
    if (!success) {
        fprintf(stderr, "Error disabling network capture\n");
        status = -1;
    }
    return status;
}

static int subscriber_main(int domainId, int sample_count, const char *profile)
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
    struct DDS_Duration_t poll_period = {1, 0}; /* 1 s */
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
    success = NDDS_Utility_enable_network_capture();
    if (!success) {
        fprintf(stderr, "Error enabling network capture");
        return -1;
    }

    /*
     * Configuration of the capture.
     *   - Parse encrypted content.
     *   - Remove all user data (serialized and encrypted) from the capture.
     *   - Defaults: e.g., capture input and output traffic.
     */
    params.parse_encrypted_content = DDS_BOOLEAN_TRUE;
    params.dropped_content =
            NDDS_UTILITY_NETWORK_CAPTURE_CONTENT_ENCRYPTED_DATA;
    if (!NDDS_Utility_start_network_capture_w_params("subscriber", &params)) {
        fprintf(stderr, "Error enabling network monitor for all participants\n");
    }

    /*
     * The example continues as the usual Connext DDS secure HelloWorld.
     * Create entities, start communication, etc.
     */
#ifdef RTI_STATIC
    struct DDS_DomainParticipantQos participant_qos =
            DDS_DomainParticipantQos_INITIALIZER;

    retcode = DDS_DomainParticipantFactory_get_participant_qos_from_profile(
            DDS_TheParticipantFactory,
            &participant_qos,
            "SecurityExampleProfiles",
            profile);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "Unable to get default participant qos");
        return -1;
    }
    retcode = DDS_PropertyQosPolicyHelper_assert_pointer_property(
            &participant_qos.property,
            RTI_SECURITY_BUILTIN_PLUGIN_PROPERTY_NAME ".create_function_ptr",
            RTI_Security_PluginSuite_create);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "Unable to assert create_function_ptr property");
        return -1;
    }

    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            domainId,
            &participant_qos,
            NULL,
            DDS_STATUS_MASK_NONE);
#else
    participant = DDS_DomainParticipantFactory_create_participant_with_profile(
            DDS_TheParticipantFactory,
            domainId,
            "SecurityExampleProfiles",
            profile,
            NULL,
            DDS_STATUS_MASK_NONE);
#endif
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

    type_name = example2TypeSupport_get_type_name();
    retcode = example2TypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    topic = DDS_DomainParticipant_create_topic(
            participant,
            "Example example2",
            type_name,
            &DDS_TOPIC_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    reader_listener.on_requested_deadline_missed  =
    example2Listener_on_requested_deadline_missed;
    reader_listener.on_requested_incompatible_qos =
    example2Listener_on_requested_incompatible_qos;
    reader_listener.on_sample_rejected =
    example2Listener_on_sample_rejected;
    reader_listener.on_liveliness_changed =
    example2Listener_on_liveliness_changed;
    reader_listener.on_sample_lost =
    example2Listener_on_sample_lost;
    reader_listener.on_subscription_matched =
    example2Listener_on_subscription_matched;
    reader_listener.on_data_available =
    example2Listener_on_data_available;

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

    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("example2 subscriber sleeping for %d sec...\n",
            poll_period.sec);
        fflush(stdout);

        NDDS_Utility_sleep(&poll_period);
    }

    /*
     * Before deleting the participants that are capturing, we must stop
     * network capture for them.
     */
    success = NDDS_Utility_stop_network_capture();
    if (!success) {
        fprintf(stderr, "Error stopping network capture for all participants\n");
    }

    /* Finalize the parameters to free allocated memory */
    NDDS_Utility_NetworkCaptureParams_t_finalize(&params);

#ifdef RTI_STATIC
    DDS_DomainParticipantQos_finalize(&participant_qos);
#endif
    return subscriber_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */
    const char *profile = "A";

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }
    if (argc >= 4) {
        if (strcmp(argv[3], "dsa") == 0) {
            profile = "DSA_A";
        }
        else if (strcmp(argv[3], "rsa") == 0) {
            profile = "RSA_A";
        }
    }

    return subscriber_main(domainId, sample_count, profile);
}

