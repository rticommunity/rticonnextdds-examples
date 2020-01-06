/*******************************************************************************
 (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

#include "hpf.h"
#include "hpfSupport.h"
#include "ndds/ndds_cpp.h"

class hpfListener : public DDSDataReaderListener {
public:
    virtual void on_requested_deadline_missed(
            DDSDataReader * /*reader*/,
            const DDS_RequestedDeadlineMissedStatus & /*status*/)
    {
    }

    virtual void on_requested_incompatible_qos(
            DDSDataReader * /*reader*/,
            const DDS_RequestedIncompatibleQosStatus & /*status*/)
    {
    }

    virtual void on_sample_rejected(
            DDSDataReader * /*reader*/,
            const DDS_SampleRejectedStatus & /*status*/)
    {
    }

    virtual void on_liveliness_changed(
            DDSDataReader * /*reader*/,
            const DDS_LivelinessChangedStatus & /*status*/)
    {
    }

    virtual void on_sample_lost(
            DDSDataReader * /*reader*/,
            const DDS_SampleLostStatus & /*status*/)
    {
    }

    virtual void on_subscription_matched(
            DDSDataReader * /*reader*/,
            const DDS_SubscriptionMatchedStatus & /*status*/)
    {
    }

    virtual void on_data_available(DDSDataReader *reader);
};

void hpfListener::on_data_available(DDSDataReader *reader)
{
    hpfDataReader *hpf_reader = NULL;
    hpfSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;
    int i;

    hpf_reader = hpfDataReader::narrow(reader);
    if (hpf_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = hpf_reader->take(
            data_seq,
            info_seq,
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

    for (i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            printf("Received data\n");
            hpfTypeSupport::print_data(&data_seq[i]);
        }
    }

    retcode = hpf_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

/* Delete all entities */
static int subscriber_shutdown(DDSDomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
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
        fprintf(stderr, "finalize_instance error %d\n", retcode);
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
    hpfListener *reader_listener = NULL;
    DDSDataReader *reader = NULL;
    DDS_ReturnCode_t retcode;
    const char *type_name = NULL;
    int count = 0;
    DDS_Duration_t receive_period = { 4, 0 };
    int status = 0;

    /* To customize the participant QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    participant = DDSTheParticipantFactory->create_participant(
            domainId,
            DDS_PARTICIPANT_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* To customize the subscriber QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Register the type before creating the topic */
    type_name = hpfTypeSupport::get_type_name();
    retcode = hpfTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    /* To customize the topic QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic(
            "Example hpf",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Create a data reader listener */
    reader_listener = new hpfListener();

    /* To customize the data reader QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            reader_listener,
            DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        fprintf(stderr, "create_datareader error\n");
        subscriber_shutdown(participant);
        delete reader_listener;
        return -1;
    }

    /* Main loop */
    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("hpf subscriber sleeping for %d sec...\n", receive_period.sec);

        NDDSUtility::sleep(receive_period);
    }

    /* Delete all entities */
    status = subscriber_shutdown(participant);
    delete reader_listener;

    return status;
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

    /* Uncomment this to turn on additional logging
    NDDSConfigLogger::get_instance()->
    set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
    NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    return subscriber_main(domain_id, sample_count);
}
