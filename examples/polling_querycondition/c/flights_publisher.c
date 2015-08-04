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

/* Delete all entities */
static int publisher_shutdown(
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

    /* RTI Connext provides finalize_instance() method on
       domain participant factory for people who want to release memory used
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

static int publisher_main(int domain_id, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Publisher *publisher = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataWriter *writer = NULL;
    FlightDataWriter *flight_writer = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;

    /* Send a new sample every second */
    struct DDS_Duration_t send_period = {1,0};

    /* Create the flight info samples. */
    const int num_flights = 4;
    Flight flights_info[4];
    flights_info[0].trackId = 1111;
    flights_info[0].company = "CompanyA";
    flights_info[0].altitude = 15000;
    flights_info[1].trackId = 2222;
    flights_info[1].company = "CompanyB";
    flights_info[1].altitude = 20000;
    flights_info[2].trackId = 3333;
    flights_info[2].company = "CompanyA";
    flights_info[2].altitude = 30000;
    flights_info[3].trackId = 4444;
    flights_info[3].company = "CompanyB";
    flights_info[3].altitude = 25000;

    /* Create a Participant. */
    participant = DDS_DomainParticipantFactory_create_participant(
        DDS_TheParticipantFactory, domain_id, &DDS_PARTICIPANT_QOS_DEFAULT,
        NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create a Publisher. */
    publisher = DDS_DomainParticipant_create_publisher(
        participant, &DDS_PUBLISHER_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        printf("create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Register type before creating the topic. */
    type_name = FlightTypeSupport_get_type_name();
    retcode = FlightTypeSupport_register_type(
        participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    /* Create a Topic. */
    topic = DDS_DomainParticipant_create_topic(
        participant, "Example Flight",
        type_name, &DDS_TOPIC_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create a DataWriter. */
    writer = DDS_Publisher_create_datawriter(
        publisher, topic,
        &DDS_DATAWRITER_QOS_DEFAULT, NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        printf("create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }
    flight_writer = FlightDataWriter_narrow(writer);
    if (flight_writer == NULL) {
        printf("DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Main loop */
    for (count=0; (sample_count==0)||(count<sample_count); count+=num_flights) {
        int i;

        /* Update flight info latitude and write */
        printf("Updating and sending values\n");

        for (i = 0; i < num_flights; i++) {
            /* Set the plane altitude lineally
             * (usually the max is at 41,000ft).
             */
            flights_info[i].altitude += count * 100;
            if (flights_info[i].altitude >= 41000) {
                flights_info[i].altitude = 41000;
            }

            printf("\t[trackId: %d, company: %s, altitude: %d]\n",
                flights_info[i].trackId, flights_info[i].company,
                flights_info[i].altitude);

            /* Write the instance */
            retcode = FlightDataWriter_write(
                flight_writer, &flights_info[i], &instance_handle);
            if (retcode != DDS_RETCODE_OK) {
                printf("write error %d\n", retcode);
            }
        }

        NDDS_Utility_sleep(&send_period);
    }

    /* Cleanup and delete delete all entities */
    return publisher_shutdown(participant);
}


#if !(defined(RTI_VXWORKS) && !defined(__RTP__)) && !defined(RTI_PSOS)
int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* Infinite loop */

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
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

    return publisher_main(domain_id, sample_count);
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
    taskSpawn("pub", RTI_OSAPI_THREAD_PRIORITY_NORMAL, 0x8, 0x150000, (FUNCPTR)publisher_main, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

}
#endif
