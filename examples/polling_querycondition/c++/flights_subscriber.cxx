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
#ifdef RTI_VX653
#include <vThreadsData.h>
#endif
#include "flights.h"
#include "flightsSupport.h"
#include "ndds/ndds_cpp.h"

/* We remove all the listener code as we won't use any listener */

/* Delete all entities */
static int subscriber_shutdown(
    DDSDomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            printf("delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides the finalize_instance() method on
       domain participant factory for people who want to release memory used
       by the participant factory. Uncomment the following block of code for
       clean destruction of the singleton. */
/*
    retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        printf("finalize_instance error %d\n", retcode);
        status = -1;
    }
*/
    return status;
}

extern "C" int subscriber_main(int domainId, int sample_count)
{
    DDSDomainParticipant *participant = NULL;
    DDSSubscriber *subscriber = NULL;
    DDSTopic *topic = NULL;
    DDSDataReader *reader = NULL;
    DDS_ReturnCode_t retcode;
    const char *type_name = NULL;
    int count = 0;

    /* Poll for new samples every second. */
    DDS_Duration_t receive_period = {1,0};
    int status = 0;

    /* Create the Participant. */
    participant = DDSTheParticipantFactory->create_participant(
        domainId, DDS_PARTICIPANT_QOS_DEFAULT,
        NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Create a Subscriber. */
    subscriber = participant->create_subscriber(
        DDS_SUBSCRIBER_QOS_DEFAULT, NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        printf("create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Register the type before creating the topic. */
    type_name = FlightTypeSupport::get_type_name();
    retcode = FlightTypeSupport::register_type(
        participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    /* Create a Topic. */
    topic = participant->create_topic(
        "Example Flight",
        type_name, DDS_TOPIC_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Call create_datareader passing NULL in the listener parameter. */
    reader = subscriber->create_datareader(
        topic, DDS_DATAREADER_QOS_DEFAULT, NULL,
        DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        printf("create_datareader error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    FlightDataReader *flight_reader =
        FlightDataReader::narrow(reader);
    if (reader == NULL) {
        printf("DataReader narrow error\n");
        return -1;
    }

    /* Query for company named 'CompanyA' and for flights in cruise
     * (about 30,000ft). The company parameter will be changed in run-time.
     * NOTE: There must be single-quotes in the query parameters around-any
     * strings! The single-quote do NOT go in the query condition itself. */
    DDS_StringSeq query_parameters;
    query_parameters.ensure_length(2, 2);
    query_parameters[0] = "'CompanyA'";
    query_parameters[1] = "30000";
    printf("Setting parameter to company %s, altitude bigger or equals to %s\n",
        query_parameters[0], query_parameters[1]);

    /* Create the query condition with an expession to MATCH the id field in
     * the structure and a numeric comparison. Note that you should make a copy
     * of the expression string when creating the query condition - beware it
     * going out of scope! */
    DDSQueryCondition *query_condition = flight_reader->create_querycondition(
        DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE,
        DDS_ALIVE_INSTANCE_STATE,
        DDS_String_dup("company MATCH %0 AND altitude >= %1"),
        query_parameters);

    /* Main loop */
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
        /* Poll for a new samples every second. */
        NDDSUtility::sleep(receive_period);

        /* Change the filter parameter after 5 seconds. */
        bool update = false;
        if ((count + 1) % 10 == 5) {
            query_parameters[0] = "'CompanyB'";
            update = true;
        } else if ((count + 1) % 10 == 0) {
            query_parameters[0] = "'CompanyA'";
            update = true;
        }

        /* Set new parameters. */
        if (update) {
            printf("Changing parameter to %s\n", query_parameters[0]);
            query_condition->set_query_parameters(query_parameters);
            update = false;
        }

        DDS_SampleInfoSeq info_seq;
        FlightSeq data_seq;

        /* Iterate through the samples read using the read_w_condition method */
        retcode = flight_reader->read_w_condition(data_seq, info_seq,
                        DDS_LENGTH_UNLIMITED, query_condition);
        if (retcode == DDS_RETCODE_NO_DATA) {
            // Not an error
            continue;
        } else if (retcode != DDS_RETCODE_OK) {
            // Is an error
            printf("take error: %d\n", retcode);
            break;
        }

        for (int i = 0; i < data_seq.length(); ++i) {
            if (info_seq[i].valid_data) {
                printf("\t[trackId: %d, company: %s, altitude: %d]\n",
                    data_seq[i].trackId, data_seq[i].company,
                    data_seq[i].altitude);
            }
        }

        retcode = flight_reader->return_loan(data_seq, info_seq);
        if (retcode != DDS_RETCODE_OK) {
            printf("return loan error %d\n", retcode);
        }
    }

    /* Delete all entities */
    status = subscriber_shutdown(participant);

    return status;
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS)
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
    NDDSConfigLogger::get_instance()->
        set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
                                  NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    return subscriber_main(domainId, sample_count);
}
#endif

#ifdef RTI_VX653
const unsigned char* __ctype = *(__ctypePtrGet());

extern "C" void usrAppInit ()
{
#ifdef  USER_APPL_INIT
    USER_APPL_INIT;         /* for backwards compatibility */
#endif

    /* add application specific code here */
    taskSpawn("sub", RTI_OSAPI_THREAD_PRIORITY_NORMAL, 0x8, 0x150000, (FUNCPTR)subscriber_main, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

}
#endif
