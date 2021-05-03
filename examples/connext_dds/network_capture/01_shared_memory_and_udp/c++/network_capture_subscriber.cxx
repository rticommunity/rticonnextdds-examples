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
 * A simple NetworkCapture using network capture to save DomainParticipant
 * traffic.
 *
 * This example is a simple hello world running network capture for both a
 * publisher participant (network_capture_publisher.cxx).and a subscriber
 * participant (network_capture_subscriber.cxx). It shows the basic flow when
 * working with network capture:
 *   - Enabling before anything else.
 *   - Start capturing traffic (for one or all participants).
 *   - Pause/resume capturing traffic (for one or all participants).
 *   - Stop capturing trafffic (for one or all participants).
 *   - Disable after everything else.
 */
#include <stdio.h>
#include <stdlib.h>

#include "ndds/ndds_cpp.h"
#include "network_capture.h"
#include "network_captureSupport.h"

class NetworkCaptureListener : public DDSDataReaderListener {
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

void NetworkCaptureListener::on_data_available(DDSDataReader *reader)
{
    NetworkCaptureDataReader *NetworkCapture_reader = NULL;
    NetworkCaptureSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;
    int i;

    NetworkCapture_reader = NetworkCaptureDataReader::narrow(reader);
    if (NetworkCapture_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = NetworkCapture_reader->take(
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
            NetworkCaptureTypeSupport::print_data(&data_seq[i]);
        }
    }

    retcode = NetworkCapture_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

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

    retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "finalize_instance error %d\n", retcode);
        status = -1;
    }

    if (!NDDSUtilityNetworkCapture::disable()) {
        fprintf(stderr, "Error disabling network capture\n");
        status = -1;
    }
    return status;
}

extern "C" int subscriber_main(int domainId, int sample_count)
{
    DDSDomainParticipant *participant = NULL;
    DDSSubscriber *subscriber = NULL;
    DDSTopic *topic = NULL;
    NetworkCaptureListener *reader_listener = NULL;
    DDSDataReader *reader = NULL;
    DDS_ReturnCode_t retcode;
    const char *type_name = NULL;
    int count = 0;
    DDS_Duration_t receive_period = { 1, 0 };
    int status = 0;
    DDS_Boolean success = DDS_BOOLEAN_FALSE;

    /*
     * Enable network capture.
     *
     * This must be called before:
     *   - Any other network capture function is called.
     *   - Creating the participants for which we want to capture traffic.
     */
    success = NDDSUtilityNetworkCapture::enable();
    if (!success) {
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
     * will start with the prefix "subscriber" and continue with a suffix
     * dependent on the participant's GUID.
     */
    success = NDDSUtilityNetworkCapture::start("subscriber");
    if (!success) {
        fprintf(stderr, "Error starting network capture\n");
        return -1;
    }

    participant = DDSTheParticipantFactory->create_participant(
            domainId,
            DDS_PARTICIPANT_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    type_name = NetworkCaptureTypeSupport::get_type_name();
    retcode = NetworkCaptureTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    topic = participant->create_topic(
            "Network capture shared memory example",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL, /* listener */
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    reader_listener = new NetworkCaptureListener();
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

    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("NetworkCapture subscriber sleeping for %d sec...\n",
               receive_period.sec);

        NDDSUtility::sleep(receive_period);
    }

    /*
     * But before deleting the participants that are capturing, we must stop
     * network capture for them.
     */
    success = NDDSUtilityNetworkCapture::stop();
    if (!success) {
        fprintf(stderr, "Error stopping network capture\n");
        subscriber_shutdown(participant);
        return -1;
    }

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

    return subscriber_main(domain_id, sample_count);
}
