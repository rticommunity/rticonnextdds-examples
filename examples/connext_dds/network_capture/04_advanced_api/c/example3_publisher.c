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
 * A simple secure HelloWorld using network capture to save DomainParticipant
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

static int publisher_shutdown(
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

static int publisher_main(int domainId, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_DomainParticipant *participant2 = NULL;
    DDS_Publisher *publisher = NULL;
    DDS_Publisher *publisher2 = NULL;
    DDS_Topic *topic = NULL;
    DDS_Topic *topic2 = NULL;
    DDS_DataWriter *writer = NULL;
    DDS_DataWriter *writer2 = NULL;
    example3DataWriter *example3_writer = NULL;
    example3DataWriter *example3_writer2 = NULL;
    example3 *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    const char *type_name2 = NULL;
    int count = 0;
    struct DDS_Duration_t send_period = {0, 10000000}; /* 10 ms */
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

#ifdef RTI_STATIC
    struct DDS_DomainParticipantQos participant_qos =
            DDS_DomainParticipantQos_INITIALIZER;

    retcode = DDS_DomainParticipantFactory_get_participant_qos_from_profile(
            DDS_TheParticipantFactory,
            &participant_qos,
            "SecurityExampleProfiles",
            "B");
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "Unable to get participant qos from profile\n");
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
            "B",
            NULL,
            DDS_STATUS_MASK_NONE);
    participant2 = DDS_DomainParticipantFactory_create_participant_with_profile(
            DDS_TheParticipantFactory,
            domainId,
            "SecurityExampleProfiles",
            "B",
            NULL,
            DDS_STATUS_MASK_NONE);
#endif
    if (participant == NULL || participant2 == NULL) {
        fprintf(stderr, "create_participant error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    publisher = DDS_DomainParticipant_create_publisher(
            participant,
            &DDS_PUBLISHER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        fprintf(stderr, "create_publisher error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    publisher2 = DDS_DomainParticipant_create_publisher(
            participant2,
            &DDS_PUBLISHER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (publisher2 == NULL) {
        fprintf(stderr, "create_publisher error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    type_name = example3TypeSupport_get_type_name();
    retcode = example3TypeSupport_register_type(
            participant,
            type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    type_name2 = example3TypeSupport_get_type_name();
    retcode = example3TypeSupport_register_type(
            participant2,
            type_name2);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
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
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
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
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    writer = DDS_Publisher_create_datawriter(
            publisher,
            topic,
            &DDS_DATAWRITER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        fprintf(stderr, "create_datawriter error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    example3_writer = example3DataWriter_narrow(writer);
    if (example3_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    writer2 = DDS_Publisher_create_datawriter(
            publisher2,
            topic2,
            &DDS_DATAWRITER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (writer2 == NULL) {
        fprintf(stderr, "create_datawriter error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }
    example3_writer2 = example3DataWriter_narrow(writer2);
    if (example3_writer2 == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    instance = example3TypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
    if (instance == NULL) {
        fprintf(stderr, "example3TypeSupport_create_data error\n");
        publisher_shutdown(participant, DDS_BOOLEAN_FALSE);
        publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
        return -1;
    }

    /*
     * Start capturing for all participants and then stop capturing for the
     * first one. Similar to capture only for the second participant.
     * The exception is that an empty capture file will be generated for the
     * first participant, because we started capturing for it.
     */
    params.parse_encrypted_content = DDS_BOOLEAN_TRUE;
    params.dropped_content = NDDS_UTILITY_NETWORK_CAPTURE_CONTENT_MASK_ALL;
    if (!NDDS_Utility_start_network_capture_w_params("publisher", &params)) {
        fprintf(stderr, "Error starting network capture\n");
    }
    if (!NDDS_Utility_stop_network_capture_for_participant(participant)) {
        fprintf(stderr, "Error stopping network capture for 1st participant\n");
    }

    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("Writing example3 Secure, count %d\n", count);

        RTIOsapiUtility_snprintf(
                instance->msg,
                128,
                "Hello World Secure (%d)",
                count);

        retcode = example3DataWriter_write(
                example3_writer,
                instance,
                &instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }
        retcode = example3DataWriter_write(
                example3_writer2,
                instance,
                &instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDS_Utility_sleep(&send_period);
    }

    retcode = example3TypeSupport_delete_data_ex(instance, DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "example3TypeSupport_delete_data error %d\n", retcode);
    }

    /* After a global start, use a global stop. This stops the 2nd participant */
    success = NDDS_Utility_stop_network_capture();
    if (!success) {
        fprintf(stderr, "Error stopping network capture\n");
    }

    /* Finalize the parameters to free allocated memory */
    NDDS_Utility_NetworkCaptureParams_t_finalize(&params);

    return publisher_shutdown(participant, DDS_BOOLEAN_FALSE)
            && publisher_shutdown(participant2, DDS_BOOLEAN_TRUE);
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

    return publisher_main(domainId, sample_count);
}

