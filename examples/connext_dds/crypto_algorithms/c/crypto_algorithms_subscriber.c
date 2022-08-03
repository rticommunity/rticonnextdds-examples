/*
 * (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
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
 * TODO SEC-1750: Add brief description.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"
#include "crypto_algorithms.h"
#include "crypto_algorithmsSupport.h"

void CryptoAlgorithmsListener_on_requested_deadline_missed(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_RequestedDeadlineMissedStatus *status)
{
}

void CryptoAlgorithmsListener_on_requested_incompatible_qos(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_RequestedIncompatibleQosStatus *status)
{
}

void CryptoAlgorithmsListener_on_sample_rejected(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SampleRejectedStatus *status)
{
}

void CryptoAlgorithmsListener_on_liveliness_changed(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_LivelinessChangedStatus *status)
{
}

void CryptoAlgorithmsListener_on_sample_lost(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SampleLostStatus *status)
{
}

void CryptoAlgorithmsListener_on_subscription_matched(
        void* listener_data,
        DDS_DataReader* reader,
        const struct DDS_SubscriptionMatchedStatus *status)
{
}

void CryptoAlgorithmsListener_on_data_available(
        void* listener_data,
        DDS_DataReader* reader)
{
    CryptoAlgorithmsDataReader *CryptoAlgorithms_reader = NULL;
    struct CryptoAlgorithmsSeq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i;

    CryptoAlgorithms_reader = CryptoAlgorithmsDataReader_narrow(reader);
    if (CryptoAlgorithms_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = CryptoAlgorithmsDataReader_take(
            CryptoAlgorithms_reader,
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

    for (i = 0; i < CryptoAlgorithmsSeq_get_length(&data_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
            printf("Received data\n");
            CryptoAlgorithmsTypeSupport_print_data(
                CryptoAlgorithmsSeq_get_reference(&data_seq, i));
        }
    }

    retcode = CryptoAlgorithmsDataReader_return_loan(
        CryptoAlgorithms_reader,
        &data_seq, &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

/* Delete all entities */
static int subscriber_shutdown(
        DDS_DomainParticipant *participant)
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
            DDS_TheParticipantFactory, participant);
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

    return status;
}

int subscriber_main(int domainId, int sample_count, const char *profile)
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
    struct DDS_Duration_t poll_period = {4,0};

#ifdef RTI_STATIC
    struct DDS_DomainParticipantQos participant_qos =
            DDS_DomainParticipantQos_INITIALIZER;

    retcode = DDS_DomainParticipantFactory_get_participant_qos_from_profile(
            DDS_TheParticipantFactory,
            &participant_qos,
            "crypto_algorithms_Library",
            profile);
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
    DDS_DomainParticipantQos_finalize(&participant_qos);
#else
    participant = DDS_DomainParticipantFactory_create_participant_with_profile(
            DDS_TheParticipantFactory,
            domainId,
            "crypto_algorithms_Library",
            profile,
            NULL,
            DDS_STATUS_MASK_NONE);
#endif

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
    type_name = CryptoAlgorithmsTypeSupport_get_type_name();
    retcode = CryptoAlgorithmsTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    /*
     * To customize topic QoS, use the configuration file USER_QOS_PROFILES.xml.
     */
    topic = DDS_DomainParticipant_create_topic(
            participant,
            "Example CryptoAlgorithms",
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
    reader_listener.on_requested_deadline_missed  =
            CryptoAlgorithmsListener_on_requested_deadline_missed;
    reader_listener.on_requested_incompatible_qos =
            CryptoAlgorithmsListener_on_requested_incompatible_qos;
    reader_listener.on_sample_rejected =
            CryptoAlgorithmsListener_on_sample_rejected;
    reader_listener.on_liveliness_changed =
            CryptoAlgorithmsListener_on_liveliness_changed;
    reader_listener.on_sample_lost =
            CryptoAlgorithmsListener_on_sample_lost;
    reader_listener.on_subscription_matched =
            CryptoAlgorithmsListener_on_subscription_matched;
    reader_listener.on_data_available =
            CryptoAlgorithmsListener_on_data_available;

    /*
     * To customize data reader QoS, use the configuration file
     * USER_QOS_PROFILES.xml.
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

    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
        printf(
                "CryptoAlgorithms subscriber sleeping for %d sec...\n",
                poll_period.sec);

        NDDS_Utility_sleep(&poll_period);
    }

    return subscriber_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */
    const char *profile = "ECDSA_P256_B";

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }
    if (argc >= 4) {
        if (strcmp(argv[3], "p256") == 0) {
            profile = "ECDSA_P256_B";
        } else if (strcmp(argv[3], "p384") == 0) {
            profile = "ECDSA_P384_B";
        } else {
            fprintf(
                    stderr,
                    "invalid profile. Valid profiles: <unspecified "
                    "(defaults to ECDSA_P256_B)>, ECDSA_P256_B, "
                    "and ECDSA_P384_B.\n");
            return -1;
        }
    }

    return subscriber_main(domain_id, sample_count, profile);
}

