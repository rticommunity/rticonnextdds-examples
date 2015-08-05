/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"
#include "flights.h"
#include "flightsSupport.h"

/* We remove all the listener code as we won't use any listener */

/* Delete all entities */
static int subscriber_shutdown(
    DDS_DomainParticipant *participant)
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
            DDS_TheParticipantFactory, participant);
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides the finalize_instance() method on
       domain participant factory for users who want to release memory used
       by the participant factory. Uncomment the following block of code for
       clean destruction of the singleton. */
/*
    retcode = DDS_DomainParticipantFactory_finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        printf("finalize_instance error %d\n", retcode);
        status = -1;
    }
*/

    return status;
}

static int subscriber_main(int domainId, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Subscriber *subscriber = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataReader *reader = NULL;
    FlightDataReader *flight_reader = NULL;
    DDS_ReturnCode_t retcode;
    const char *type_name = NULL;
    int count = 0;

    /* Poll for new samples every second. */
    struct DDS_Duration_t poll_period = {1,0};

    /* Query Condition-specific types */
    DDS_QueryCondition *query_condition;
    struct DDS_StringSeq query_parameters;
    const char* param_list[] = { "'CompanyA'", "30000" };

    /* Create a Participant. */
    participant = DDS_DomainParticipantFactory_create_participant(
        DDS_TheParticipantFactory, domainId, &DDS_PARTICIPANT_QOS_DEFAULT,
        NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Createa a Subscriber. */
    subscriber = DDS_DomainParticipant_create_subscriber(
        participant, &DDS_SUBSCRIBER_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        printf("create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Register the type before creating the topic. */
    type_name = FlightTypeSupport_get_type_name();
    retcode = FlightTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    /* Create a Topic. */
    topic = DDS_DomainParticipant_create_topic(
        participant, "Example Flight",
        type_name, &DDS_TOPIC_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Call create_datareader passing NULL in the listener parameter. */
    reader = DDS_Subscriber_create_datareader(
        subscriber, DDS_Topic_as_topicdescription(topic),
        &DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        printf("create_datareader error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    flight_reader = FlightDataReader_narrow(reader);
    if (flight_reader == NULL) {
        printf("DataReader narrow error\n");
        return -1;
    }

    /* Query for company named 'CompanyA' and for flights in cruise
     * (about 30,000ft). The company parameter will be changed in run-time.
     * NOTE: There must be single-quotes in the query parameters around-any
     * strings! The single-quote do NOT go in the query condition itself. */
    DDS_StringSeq_ensure_length(&query_parameters, 2, 2);
    DDS_StringSeq_from_array(&query_parameters, param_list, 2);
    printf("Setting parameter to company %s, altitude bigger or equals to %s\n",
        DDS_StringSeq_get(&query_parameters, 0),
        DDS_StringSeq_get(&query_parameters, 1));

    /* Create the query condition with an expession to MATCH the id field in
     * the structure and a numeric comparison. Note that you should make a copy
     * of the expression string when creating the query condition - beware it
     * going out of scope! */
    query_condition = DDS_DataReader_create_querycondition(
        reader,
        DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE,
        DDS_ALIVE_INSTANCE_STATE,
        DDS_String_dup("company MATCH %0 AND altitude >= %1"),
        &query_parameters);

    /* Main loop */
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
        struct DDS_SampleInfoSeq info_seq;
        struct FlightSeq data_seq;
        DDS_ReturnCode_t retcode;
        int update = 0;
        int i = 0;

        /* Poll for new samples every second. */
        NDDS_Utility_sleep(&poll_period);

        /* Change the filter parameter after 5 seconds. */
        if ((count + 1) % 10 == 5) {
            *DDS_StringSeq_get_reference(&query_parameters, 0) =
                DDS_String_dup("'CompanyB'");
            update = 1;
        } else if ((count + 1) % 10 == 0) {
            *DDS_StringSeq_get_reference(&query_parameters, 0) =
                DDS_String_dup("'CompanyA'");
            update = 1;
        }

        /* Set new parameters. */
        if (update) {
            update = 0;
            printf("Changing parameter to %s\n",
                DDS_StringSeq_get(&query_parameters, 0));
            retcode = DDS_QueryCondition_set_query_parameters(
                query_condition,
                &query_parameters);
            if (retcode != DDS_RETCODE_OK) {
                printf("Error setting new parameters: %d\n", retcode);
            }
        }

        /* Iterate through the samples read using the read_w_condition method */
        retcode = FlightDataReader_read_w_condition(
                    flight_reader,
                    &data_seq, &info_seq,
                    DDS_LENGTH_UNLIMITED,
                    DDS_QueryCondition_as_readcondition(query_condition));
        if (retcode == DDS_RETCODE_NO_DATA) {
            /* Not an error */
            continue;
        } else if (retcode != DDS_RETCODE_OK) {
            /* Is an error */
            printf("take error: %d\n", retcode);
            break;
        }

        for (i = 0; i < FlightSeq_get_length(&data_seq); ++i) {
            if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
                FlightTypeSupport_print_data(
                    FlightSeq_get_reference(&data_seq, i));
            }
        }

        retcode = FlightDataReader_return_loan(
            flight_reader, &data_seq, &info_seq);
        if (retcode != DDS_RETCODE_OK) {
            printf("return loan error %d\n", retcode);
        }
    }

    /* Cleanup and delete all entities */
    return subscriber_shutdown(participant);
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS)
int main(int argc, char *argv[])
{
    int domainId = 0;
    int sample_count = 0; /* Infinite loop */

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
        NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL); */


    return subscriber_main(domainId, sample_count);
}
#endif

#ifdef RTI_VX653
const unsigned char* __ctype = NULL;

void usrAppInit ()
{
#ifdef  USER_APPL_INIT
    USER_APPL_INIT;         /* for backwards compatibility */
#endif

    /* add application specific code here */
    taskSpawn("sub", RTI_OSAPI_THREAD_PRIORITY_NORMAL, 0x8, 0x150000, (FUNCPTR)subscriber_main, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

}
#endif
