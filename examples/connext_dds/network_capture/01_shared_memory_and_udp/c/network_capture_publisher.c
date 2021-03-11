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

static int publisher_shutdown(DDS_DomainParticipant *participant)
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
    return status;
}

int publisher_main(int domainId, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Publisher *publisher = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataWriter *writer = NULL;
    NetworkCaptureDataWriter *NetworkCapture_writer = NULL;
    NetworkCapture *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;
    struct DDS_Duration_t send_period = { 1, 0 }; /* 1 s */

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
     * Start capturing traffic for all participants.
     *
     * All participants: those already created and those yet to be created.
     * Default parameters: all transports and some other sane defaults.
     *
     * A capture file will be created for each participant. The capture file
     * will start with the prefix "publisher" and continue with a suffix
     * dependent on the participant's GUID.
     */
    if (!NDDS_Utility_start_network_capture("publisher")) {
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
        publisher_shutdown(participant);
        return -1;
    }

    publisher = DDS_DomainParticipant_create_publisher(
            participant,
            &DDS_PUBLISHER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        fprintf(stderr, "create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    type_name = NetworkCaptureTypeSupport_get_type_name();
    retcode = NetworkCaptureTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant);
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
        publisher_shutdown(participant);
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
        publisher_shutdown(participant);
        return -1;
    }
    NetworkCapture_writer = NetworkCaptureDataWriter_narrow(writer);
    if (NetworkCapture_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    instance = NetworkCaptureTypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
    if (instance == NULL) {
        fprintf(stderr, "NetworkCaptureTypeSupport_create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("Writing NetworkCapture, count %d\n", count);

        RTIOsapiUtility_snprintf(instance->msg, 128, "Hello World (%d)", count);

        /*
         * Here we are going to pause capturing for some samples.
         * The resulting pcap file will not contain them.
         */
        if (count == 4 && !NDDS_Utility_pause_network_capture()) {
            fprintf(stderr, "Error pausing network capture\n");
        } else if (count == 6 && !NDDS_Utility_resume_network_capture()) {
            fprintf(stderr, "Error resuming network capture\n");
        }

        retcode = NetworkCaptureDataWriter_write(
                NetworkCapture_writer,
                instance,
                &instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDS_Utility_sleep(&send_period);
    }

    retcode = NetworkCaptureTypeSupport_delete_data_ex(
            instance,
            DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,
                "NetworkCaptureTypeSupport_delete_data error %d\n",
                retcode);
    }

    /*
     * Before deleting the participants that are capturing, we must stop
     * network capture for them.
     */
    if (!NDDS_Utility_stop_network_capture()) {
        fprintf(stderr,
                "Error stopping network capture for all participants\n");
    }

    return publisher_shutdown(participant);
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

    return publisher_main(domain_id, sample_count);
}
