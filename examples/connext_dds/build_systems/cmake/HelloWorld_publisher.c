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
 * A publication example.
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

/* Delete all entities */
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

    /* RTI Data Distribution Service provides finalize_instance() method on
     * domain participant factory for people who want to release memory used
     * by the participant factory. Uncomment the following block of code for
     * clean destruction of the singleton.
     *
     * retcode = DDS_DomainParticipantFactory_finalize_instance();
     * if (retcode != DDS_RETCODE_OK) {
     *     fprintf(stderr, "finalize_instance error %d\n", retcode);
     *     status = -1;
     * }
     */

    return status;
}

int publisher_main(int domainId, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Publisher *publisher = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataWriter *writer = NULL;
    HelloWorldDataWriter *HelloWorld_writer = NULL;
    HelloWorld *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;
    struct DDS_Duration_t send_period = { 4, 0 };

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
    type_name = HelloWorldTypeSupport_get_type_name();
    retcode = HelloWorldTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant);
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

    HelloWorld_writer = HelloWorldDataWriter_narrow(writer);
    if (HelloWorld_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create data sample for writing */
    instance = HelloWorldTypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
    if (instance == NULL) {
        fprintf(stderr, "HelloWorldTypeSupport_create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /*
     * For a data type that has a key, if the same instance is going to be
     * written multiple times, initialize the key here and register the keyed
     * instance prior to writing
     *
     * instance_handle = HelloWorldDataWriter_register_instance(
     *         HelloWorld_writer,
     *         instance);
     */

    /* Main loop */
    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("Writing HelloWorld, count %d\n", count);

        /* Modify the data to be written here */

        /* Write data */
        retcode = HelloWorldDataWriter_write(
                HelloWorld_writer,
                instance,
                &instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDS_Utility_sleep(&send_period);
    }

    /*
     *
     * retcode = HelloWorldDataWriter_unregister_instance(
     *         HelloWorld_writer,
     *         instance,
     *         &instance_handle);
     * if (retcode != DDS_RETCODE_OK) {
     *     fprintf(stderr, "unregister instance error %d\n", retcode);
     * }
     */

    /* Delete data sample */
    retcode = HelloWorldTypeSupport_delete_data_ex(instance, DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr,
                "HelloWorldTypeSupport_delete_data error %d\n",
                retcode);
    }

    /* Cleanup and delete delete all entities */
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

    /*
     * Uncomment this to turn on additional logging
     *
     * NDDS_Config_Logger_set_verbosity_by_category(
     *         NDDS_Config_Logger_get_instance(),
     *         NDDS_CONFIG_LOG_CATEGORY_API,
     *         NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return publisher_main(domain_id, sample_count);
}
