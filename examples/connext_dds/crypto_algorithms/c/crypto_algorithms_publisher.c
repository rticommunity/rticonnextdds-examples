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

static int publisher_shutdown(
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

int publisher_main(int domainId, int sample_count, const char *profile)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Publisher *publisher = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataWriter *writer = NULL;
    CryptoAlgorithmsDataWriter *CryptoAlgorithms_writer = NULL;
    CryptoAlgorithms *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;
    struct DDS_Duration_t send_period = {4,0};

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
        publisher_shutdown(participant);
        return -1;
    }

    /*
     * To customize publisher QoS, use the configuration file
     * USER_QOS_PROFILES.xml
     */
    publisher = DDS_DomainParticipant_create_publisher(
            participant,
            &DDS_PUBLISHER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        fprintf(stderr, "create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Register type before creating topic */
    type_name = CryptoAlgorithmsTypeSupport_get_type_name();
    retcode = CryptoAlgorithmsTypeSupport_register_type(
            participant,
            type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant);
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
        publisher_shutdown(participant);
        return -1;
    }

    /*
     * To customize data writer QoS, use the configuration file
     * USER_QOS_PROFILES.xml
     */
    writer = DDS_Publisher_create_datawriter(
            publisher,
            topic,
            &DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        fprintf(stderr, "create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }
    CryptoAlgorithms_writer = CryptoAlgorithmsDataWriter_narrow(writer);
    if (CryptoAlgorithms_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create data sample for writing */
    instance = CryptoAlgorithmsTypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
    if (instance == NULL) {
        fprintf(stderr, "CryptoAlgorithmsTypeSupport_create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {

        printf("Writing CryptoAlgorithms, count %d\n", count);

        RTIOsapiUtility_snprintf(
                instance->msg,
                128,
                "Hello World Secure (%d)",
                count);

        retcode = CryptoAlgorithmsDataWriter_write(
                CryptoAlgorithms_writer,
                instance,
                &instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDS_Utility_sleep(&send_period);

    }

    /*
    retcode = CryptoAlgorithmsDataWriter_unregister_instance(
        CryptoAlgorithms_writer, instance, &instance_handle);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "unregister instance error %d\n", retcode);
    }
     */

    /* Delete data sample */
    retcode = CryptoAlgorithmsTypeSupport_delete_data_ex(
            instance,
            DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(
                stderr,
                "CryptoAlgorithmsTypeSupport_delete_data error %d\n",
                retcode);
    }

    return publisher_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */
    const char *profile = "ECDSA_P256_A";

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }
    if (argc >= 4) {
        if (strcmp(argv[3], "p256") == 0) {
            profile = "ECDSA_P256_A";
        } else if (strcmp(argv[3], "p384") == 0) {
            profile = "ECDSA_P384_A";
        } else {
            fprintf(
                    stderr,
                    "invalid profile. Valid profiles: <unspecified "
                    "(defaults to ECDSA_P256_A)>, ECDSA_P256_A, "
                    "and ECDSA_P384_A.\n");
            return -1;
        }
    }

    return publisher_main(domain_id, sample_count, profile);
}

