/* listeners_publisher.c

   A publication of data of type listeners

   This file is derived from code automatically generated by the rtiddsgen 
   command:

   rtiddsgen -language C -example <arch> listeners.idl

   Example publication of type listeners automatically generated by 
   'rtiddsgen'. To test them follow these steps:

   (1) Compile this file and the example subscription.

   (2) Start the subscription with the command
   objs/<arch>/listeners_subscriber <domain_id> <sample_count>

   (3) Start the publication with the command
   objs/<arch>/listeners_publisher <domain_id> <sample_count>

   (4) [Optional] Specify the list of discovery initial peers and 
   multicast receive addresses via an environment variable or a file 
   (in the current working directory) called NDDS_DISCOVERY_PEERS. 
       
   You can run any number of publishers and subscribers programs, and can 
   add and remove them dynamically from the domain.
              
                                   
   Example:
        
   To run the example application on domain <domain_id>:
                                  
   On Unix: 
       
   objs/<arch>/listeners_publisher <domain_id> 
   objs/<arch>/listeners_subscriber <domain_id> 
                            
   On Windows:
       
   objs\<arch>\listeners_publisher <domain_id>  
   objs\<arch>\listeners_subscriber <domain_id>   
       
       
    modification history
   ------------ -------       
*/

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"
#include "listeners.h"
#include "listenersSupport.h"

void DataWriterListener_on_offered_deadline_missed(
    void *listener_data,
    DDS_DataWriter *writer,
    const struct DDS_OfferedDeadlineMissedStatus *status)
{
    printf("DataWriterListener: on_offered_deadline_missed()\n");
}

void DataWriterListener_on_liveliness_lost(
    void *listener_data, 
    DDS_DataWriter *writer, 
    const struct DDS_LivelinessLostStatus *status)
{
    printf("DataWriterListener: on_liveliness_lost()\n");
}

void DataWriterListener_on_offered_incompatible_qos(
    void *listener_data, 
    DDS_DataWriter *writer, 
    const struct DDS_OfferedIncompatibleQosStatus *status)
{
    printf("DataWriterListener: on_offered_incompatible_qos()\n");  
}

void DataWriterListener_on_publication_matched(
    void *listener_data, 
    DDS_DataWriter *writer, 
    const struct DDS_PublicationMatchedStatus *status)
{
    printf("DataWriterListener: on_publication_matched()\n");
    if (status->current_count_change < 0) {
	printf("lost a subscription\n");
    } 
    else {
	printf("found a subscription\n");
    }
}

void DataWriterListener_on_reliable_writer_cache_changed(
    void *listener_data, 
    DDS_DataWriter *writer, 
    const struct DDS_ReliableWriterCacheChangedStatus *status)
{
    printf("DataWriterListener: on_reliable_writer_cache_changed()\n");    
}

void DataWriterListener_on_reliable_reader_activity_changed(
    void *listener_data, 
    DDS_DataWriter *writer, 
    const struct DDS_ReliableReaderActivityChangedStatus *status)
{
    printf("DataWriterListener: on_reliable_reader_activity_changed()\n");
}

/* Delete all entities */
static int publisher_shutdown(DDS_DomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
	retcode = DDS_DomainParticipant_delete_contained_entities(participant);
	if (retcode != DDS_RETCODE_OK) {
	    printf("delete_contained_entities error %d\n", retcode);
	    status = -1;
	}

	retcode = DDS_DomainParticipantFactory_delete_participant(DDS_TheParticipantFactory, 
								  participant);
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

static int publisher_main(int domainId, int sample_count)
{
    DDS_DomainParticipant *participant = NULL;
    DDS_Publisher *publisher = NULL;
    DDS_Topic *topic = NULL;
    DDS_DataWriter *writer = NULL;
    listenersDataWriter *listeners_writer = NULL;
    listeners *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    
    struct DDS_DataWriterListener writer_listener = 
	DDS_DataWriterListener_INITIALIZER;

    DDS_Topic *inconsistent_topic = NULL;
    DDS_DataWriter *inconsistent_topic_writer = NULL;
    const char *inconsistent_topic_type_name = NULL;
    msgDataWriter *msg_writer = NULL;

    int count = 0;  
    struct DDS_Duration_t send_period = {1,0};
    struct DDS_Duration_t sleep_period = {2,0};
    
    /* To customize participant QoS, use 
       the configuration file USER_QOS_PROFILES.xml */
    participant = DDS_DomainParticipantFactory_create_participant(DDS_TheParticipantFactory, 
								  domainId, 
								  &DDS_PARTICIPANT_QOS_DEFAULT,
								  NULL /* listener */, 
								  DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
	printf("create_participant error\n");
	publisher_shutdown(participant);
	return -1;
    }

    /* To customize publisher QoS, use 
       the configuration file USER_QOS_PROFILES.xml */
    publisher = DDS_DomainParticipant_create_publisher(participant, 
						       &DDS_PUBLISHER_QOS_DEFAULT, 
						       NULL /* listener */,
						       DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
	printf("create_publisher error\n");
	publisher_shutdown(participant);
	return -1;
    }

    /* Create ande Delete Inconsistent Topic 
     * ---------------------------------------------------------------
     * Here we create an inconsistent topic to trigger the subscriber 
     * application's callback. 
     * The inconsistent topic is created with the topic name used in 
     * the Subscriber application, but with a different data type -- 
     * the msg data type defined in partitions.idl.
     * Once it is created, we sleep to ensure the applications discover
     * each other and delete the Data Writer and Topic.
     */
    
    /* First we register the msg type -- we name it
     * inconsistent_topic_type_name to avoid confusion. 
     */
    printf("Creating Inconsistent Topic...  \n");

    inconsistent_topic_type_name = msgTypeSupport_get_type_name();
    retcode = msgTypeSupport_register_type(participant, 
					   inconsistent_topic_type_name);
    if (retcode != DDS_RETCODE_OK) {
	printf("register_type error %d\n", retcode);
	publisher_shutdown(participant);
	return -1;
    }

    inconsistent_topic = 
	DDS_DomainParticipant_create_topic(participant,
					   "Example listeners",
					   inconsistent_topic_type_name, 
					   &DDS_TOPIC_QOS_DEFAULT, 
					   NULL,
					   DDS_STATUS_MASK_NONE);
    if (inconsistent_topic == NULL) {
	printf("create_topic error (inconsistent topic)\n");
	publisher_shutdown(participant);
	return -1;
    }
    
    /* We have to associate a writer to the topic, as Topic information is not
     * actually propagated until the creation of an associated writer.
     */
    inconsistent_topic_writer = DDS_Publisher_create_datawriter(publisher,
								inconsistent_topic, 
								&DDS_DATAWRITER_QOS_DEFAULT, 
								NULL /* listener */,
								DDS_STATUS_MASK_NONE);
    if (inconsistent_topic_writer == NULL) {
	printf("create_datawriter error\n");
	publisher_shutdown(participant);
	return -1;
    }

    msg_writer = msgDataWriter_narrow(inconsistent_topic_writer);
    if (msg_writer == NULL) {
	printf("DataWriter narrow error\n");
	publisher_shutdown(participant);
	return -1;
    }
    
    /* Sleep to leave time for applications to discover each other */
    NDDS_Utility_sleep(&sleep_period);

    /* Before creating the "consistent" topic, we delete the Data Writer and the
     * inconsistent topic.
     */
    retcode = DDS_Publisher_delete_datawriter(publisher, inconsistent_topic_writer);
    if (retcode != DDS_RETCODE_OK) {
        printf("delete_datawriter error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    retcode = DDS_DomainParticipant_delete_topic(participant, inconsistent_topic);
    if (retcode != DDS_RETCODE_OK) {
        printf("delete_topic error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    printf("... Deleted Inconsistent Topic\n\n");

    /* Create Consistent Topic 
     * -----------------------------------------------------------------
     * Once we have created the inconsistent topic with the wrong type, 
     * we create a topic with the right type name -- listeners -- that we 
     * will use to publish data. 
     */

    /* Register type before creating topic */
    type_name = listenersTypeSupport_get_type_name();
    retcode = listenersTypeSupport_register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
	printf("register_type error %d\n", retcode);
	publisher_shutdown(participant);
	return -1;
    }

    /* To customize topic QoS, use 
       the configuration file USER_QOS_PROFILES.xml */
    topic = DDS_DomainParticipant_create_topic(participant, 
					       "Example listeners",
					       type_name, 
					       &DDS_TOPIC_QOS_DEFAULT, 
					       NULL /* listener */,
					       DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
	printf("create_topic error\n");
	publisher_shutdown(participant);
	return -1;
    }

       

    /* We will use the Data Writer Listener defined above to print
     * a message when some of events are triggered in the DataWriter. 
     * To do that, first we have to pass the writer_listener and then
     * we have to enable all status in the status mask.
     */

    /* Set up participant listener */
    writer_listener.on_offered_deadline_missed =
	DataWriterListener_on_offered_deadline_missed;
    writer_listener.on_liveliness_lost =
	DataWriterListener_on_liveliness_lost;
    writer_listener.on_offered_incompatible_qos =
	DataWriterListener_on_offered_incompatible_qos;
    writer_listener.on_publication_matched =
	DataWriterListener_on_publication_matched;
    writer_listener.on_reliable_writer_cache_changed = 
	DataWriterListener_on_reliable_writer_cache_changed;
    writer_listener.on_reliable_reader_activity_changed = 
	DataWriterListener_on_reliable_reader_activity_changed;

    writer = DDS_Publisher_create_datawriter(publisher,
					     topic,
					     &DDS_DATAWRITER_QOS_DEFAULT, 
					     &writer_listener  /* listener */, 
					     DDS_STATUS_MASK_ALL /* enable all statuses */);
    if (writer == NULL) {
	printf("create_datawriter error\n");
	publisher_shutdown(participant);
	return -1;
    }
    listeners_writer = listenersDataWriter_narrow(writer);
    if (listeners_writer == NULL) {
	printf("DataWriter narrow error\n");
	publisher_shutdown(participant);
	return -1;
    }

    /* Create data sample for writing */
    instance = listenersTypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
    

    if (instance == NULL) {
	printf("listenersTypeSupport_create_data error\n");
	publisher_shutdown(participant);
	return -1;
    }

    /* For a data type that has a key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    /*
      instance_handle = listenersDataWriter_register_instance(
      listeners_writer, instance);
    */

    printf("Publishing data using Consinstent Topic... \n");
    /* Main loop */
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {

	printf("Writing listeners, count %d\n", count);

	/* Modify the data to be written here */
        instance->x = count;

	/* Write data */
	retcode = listenersDataWriter_write(
					    listeners_writer, instance, &instance_handle);
	if (retcode != DDS_RETCODE_OK) {
	    printf("write error %d\n", retcode);
	}

	NDDS_Utility_sleep(&send_period);
    }

    /*
      retcode = listenersDataWriter_unregister_instance(
      listeners_writer, instance, &instance_handle);
      if (retcode != DDS_RETCODE_OK) {
      printf("unregister instance error %d\n", retcode);
      }
    */

    /* Delete data sample */
    retcode = listenersTypeSupport_delete_data_ex(instance, DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
	printf("listenersTypeSupport_delete_data error %d\n", retcode);
    }

    /* Cleanup and delete delete all entities */         
    return publisher_shutdown(participant);
}

#if defined(RTI_WINCE)
int wmain(int argc, wchar_t** argv)
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
        
    return publisher_main(domainId, sample_count);
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
        
    return publisher_main(domainId, sample_count);
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
