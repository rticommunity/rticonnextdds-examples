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
/* coherent_subscriber.c

   A subscription example

   This file is derived from code automatically generated by the rtiddsgen
   command:

   rtiddsgen -language C -example <arch> coherent.idl

   Example subscription of type coherent automatically generated by
   'rtiddsgen'. To test them, follow these steps:

   (1) Compile this file and the example publication.

   (2) Start the subscription with the command
       objs/<arch>/coherent_subscriber <domain_id> <sample_count>

   (3) Start the publication with the command
       objs/<arch>/coherent_publisher <domain_id> <sample_count>

   (4) [Optional] Specify the list of discovery initial peers and
       multicast receive addresses via an environment variable or a file
       (in the current working directory) called NDDS_DISCOVERY_PEERS.

   You can run any number of publishers and subscribers programs, and can
   add and remove them dynamically from the domain.


   Example:

       To run the example application on domain <domain_id>:

       On UNIX systems:

       objs/<arch>/coherent_publisher <domain_id>
       objs/<arch>/coherent_subscriber <domain_id>

       On Windows systems:

       objs\<arch>\coherent_publisher <domain_id>
       objs\<arch>\coherent_subscriber <domain_id>


modification history
------------ -------
* Modify listener callback to update Object State
* Change presentation scope in subscriber QoS
* Use reliable communication (currently required for coherency QoS)
* All QoS are in the xml file
*/

#include "coherent.h"
#include "coherentSupport.h"
#include "ndds/ndds_c.h"
#include <stdio.h>
#include <stdlib.h>

/* Start changes for coherent_presentation */
#define STATENVALS 6
int statevals[STATENVALS] = { 0 };

void print_state()
{
    unsigned int i;
    char c = 'a';
    for (i = 0; i < STATENVALS; ++i) {
        printf(" %c = %d;", c++, statevals[i]);
    }
}

void set_state(char c, int value)
{
    unsigned int idx = c - 'a';
    if (idx < 0 || idx >= STATENVALS) {
        printf("error: invalid field '%c'\n", c);
        return;
    }
    statevals[idx] = value;
}

/* End changes for coherent_presentation */

void coherentListener_on_requested_deadline_missed(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_RequestedDeadlineMissedStatus *status)
{
}

void coherentListener_on_requested_incompatible_qos(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_RequestedIncompatibleQosStatus *status)
{
}

void coherentListener_on_sample_rejected(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SampleRejectedStatus *status)
{
}

void coherentListener_on_liveliness_changed(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_LivelinessChangedStatus *status)
{
}

void coherentListener_on_sample_lost(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SampleLostStatus *status)
{
}

void coherentListener_on_subscription_matched(
        void *listener_data,
        DDS_DataReader *reader,
        const struct DDS_SubscriptionMatchedStatus *status)
{
}

void coherentListener_on_data_available(
        void *listener_data,
        DDS_DataReader *reader)
{
    coherentDataReader *coherent_reader = NULL;
    struct coherentSeq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i;
    int len = 0;
    coherent *data;

    coherent_reader = coherentDataReader_narrow(reader);
    if (coherent_reader == NULL) {
        printf("DataReader narrow error\n");
        return;
    }

    retcode = coherentDataReader_take(
            coherent_reader,
            &data_seq,
            &info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);
    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        printf("take error %d\n", retcode);
        return;
    }

    /* Start changes for coherent_presentation */

    /* Firstly, we process all samples */
    len = 0;
    for (i = 0; i < DDS_SampleInfoSeq_get_length(&info_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
            data = coherentSeq_get_reference(&data_seq, i);
            len++;
            set_state(data->field, data->value);
        }
    }

    /* Then, we print the results */
    if (len > 0) {
        printf("Received %d updates\n", len);
        print_state();
        printf("\n");
    }

    /* End changes for coherent_presentation */

    retcode = coherentDataReader_return_loan(
            coherent_reader,
            &data_seq,
            &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        printf("return loan error %d\n", retcode);
    }
}

/* Delete all entities */
static int subscriber_shutdown(
        DDS_DomainParticipant *participant,
        struct DDS_SubscriberQos *subscriber_qos,
        struct DDS_DataReaderQos *datareader_qos)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = DDS_DomainParticipant_delete_contained_entities(participant);
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDS_DomainParticipantFactory_delete_participant(
                DDS_TheParticipantFactory,
                participant);
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    retcode = DDS_SubscriberQos_finalize(subscriber_qos);
    if (retcode != DDS_RETCODE_OK) {
        printf("DDS_SubscriberQos_finalize error %d\n", retcode);
        status = -1;
    }

    retcode = DDS_DataReaderQos_finalize(datareader_qos);
    if (retcode != DDS_RETCODE_OK) {
        printf("DDS_DataReaderQos_finalize error %d\n", retcode);
        status = -1;
    }

    retcode = DDS_DomainParticipantFactory_finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        printf("finalize_instance error %d\n", retcode);
        status = -1;
    }

    return status;
}

static int subscriber_main(int domainId, int sample_count)
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
    struct DDS_SubscriberQos subscriber_qos = DDS_SubscriberQos_INITIALIZER;
    struct DDS_DataReaderQos datareader_qos = DDS_DataReaderQos_INITIALIZER;

    /* To customize participant QoS, use
     the configuration file USER_QOS_PROFILES.xml */
    participant = DDS_DomainParticipantFactory_create_participant(
            DDS_TheParticipantFactory,
            domainId,
            &DDS_PARTICIPANT_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
        return -1;
    }

    /* To customize subscriber QoS, use
     the configuration file USER_QOS_PROFILES.xml */
    subscriber = DDS_DomainParticipant_create_subscriber(
            participant,
            &DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        printf("create_subscriber error\n");
        subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
        return -1;
    }

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_subscriber call above.
     */

    /* Start changes for coherent_presentation */

    /* Get default subscriber QoS to customize */
    /*    retcode =
       DDS_DomainParticipant_get_default_subscriber_qos(participant,
                &subscriber_qos);
        if (retcode != DDS_RETCODE_OK) {
            printf("get_default_subscriber_qos error\n");
            return -1;
        }

        subscriber_qos.presentation.access_scope = DDS_TOPIC_PRESENTATION_QOS;
        subscriber_qos.presentation.coherent_access = DDS_BOOLEAN_TRUE;

        subscriber = DDS_DomainParticipant_create_subscriber(participant,
                &subscriber_qos, NULL, DDS_STATUS_MASK_NONE);
        if (subscriber == NULL) {
            printf("create_subscriber error\n");
            subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
            return -1;
        }

    */    /* End changes for coherent_presentation */

    /* Register the type before creating the topic */
    type_name = coherentTypeSupport_get_type_name();
    retcode = coherentTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
        return -1;
    }

    /* To customize topic QoS, use
     the configuration file USER_QOS_PROFILES.xml */
    topic = DDS_DomainParticipant_create_topic(
            participant,
            "Example coherent",
            type_name,
            &DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
        return -1;
    }

    /* Set up a data reader listener */
    reader_listener.on_requested_deadline_missed =
            coherentListener_on_requested_deadline_missed;
    reader_listener.on_requested_incompatible_qos =
            coherentListener_on_requested_incompatible_qos;
    reader_listener.on_sample_rejected = coherentListener_on_sample_rejected;
    reader_listener.on_liveliness_changed =
            coherentListener_on_liveliness_changed;
    reader_listener.on_sample_lost = coherentListener_on_sample_lost;
    reader_listener.on_subscription_matched =
            coherentListener_on_subscription_matched;
    reader_listener.on_data_available = coherentListener_on_data_available;

    /* To customize data reader QoS, use
     the configuration file USER_QOS_PROFILES.xml */
    reader = DDS_Subscriber_create_datareader(
            subscriber,
            DDS_Topic_as_topicdescription(topic),
            &DDS_DATAREADER_QOS_DEFAULT,
            &reader_listener,
            DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        printf("create_datareader error\n");
        subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
        return -1;
    }

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_datareader call above.
     */
    /* Start changes for coherent_presentation */

    /* Get default datareader QoS to customize */
    /*    retcode = DDS_Subscriber_get_default_datareader_qos(subscriber,
                &datareader_qos);
        if (retcode != DDS_RETCODE_OK) {
            printf("get_default_datareader_qos error\n");
            return -1;
        }

        datareader_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
        datareader_qos.history.depth = 10;

        reader = DDS_Subscriber_create_datareader(subscriber,
                DDS_Topic_as_topicdescription(topic), &datareader_qos,
                &reader_listener, DDS_STATUS_MASK_ALL);
        if (reader == NULL) {
            printf("create_datareader error\n");
            subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
            return -1;
        }

    */    /* End changes for coherent_presentation */

    /* Main loop */
    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        NDDS_Utility_sleep(&poll_period);
    }

    /* Cleanup and delete all entities */
    return subscriber_shutdown(participant, &subscriber_qos, &datareader_qos);
}

#if defined(RTI_WINCE)
int wmain(int argc, wchar_t **argv)
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = _wtoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = _wtoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
     NDDS_Config_Logger_set_verbosity_by_category(
     NDDS_Config_Logger_get_instance(),
     NDDS_CONFIG_LOG_CATEGORY_API,
     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return subscriber_main(domainId, sample_count);
}
#elif !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS)
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

    /* Uncomment this to turn on additional logging
     NDDS_Config_Logger_set_verbosity_by_category(
     NDDS_Config_Logger_get_instance(),
     NDDS_CONFIG_LOG_CATEGORY_API,
     NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
     */

    return subscriber_main(domainId, sample_count);
}
#endif

#ifdef RTI_VX653
const unsigned char *__ctype = NULL;

void usrAppInit()
{
    #ifdef USER_APPL_INIT
    USER_APPL_INIT; /* for backwards compatibility */
    #endif

    /* add application specific code here */
    taskSpawn(
            "sub",
            RTI_OSAPI_THREAD_PRIORITY_NORMAL,
            0x8,
            0x150000,
            (FUNCPTR) subscriber_main,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0);
}
#endif
