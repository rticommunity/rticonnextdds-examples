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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "listeners.h"
#include "listenersSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

class ParticipantListener : public DDSDomainParticipantListener {
public:
    virtual void on_requested_deadline_missed(
            DDSDataReader * /*reader*/,
            const DDS_RequestedDeadlineMissedStatus & /*status*/)
    {
        std::cout << "ParticipantListener: on_requested_deadline_missed()\n";
    }

    virtual void on_requested_incompatible_qos(
            DDSDataReader * /*reader*/,
            const DDS_RequestedIncompatibleQosStatus & /*status*/)
    {
        std::cout << "ParticipantListener: on_requested_incompatible_qos()\n";
    }

    virtual void on_sample_rejected(
            DDSDataReader * /*reader*/,
            const DDS_SampleRejectedStatus & /*status*/)
    {
        std::cout << "ParticipantListener: on_sample_rejected()\n";
    }

    virtual void on_liveliness_changed(
            DDSDataReader * /*reader*/,
            const DDS_LivelinessChangedStatus & /*status*/)
    {
        std::cout << "ParticipantListener: on_liveliness_changed()\n";
    }

    virtual void on_sample_lost(
            DDSDataReader * /*reader*/,
            const DDS_SampleLostStatus & /*status*/)
    {
        std::cout << "ParticipantListener: on_sample_lost()\n";
    }

    virtual void on_subscription_matched(
            DDSDataReader * /*reader*/,
            const DDS_SubscriptionMatchedStatus & /*status*/)
    {
        std::cout << "ParticipantListener: on_subscription_matched()\n";
    }

    virtual void on_data_available(DDSDataReader *reader)
    {
        std::cout << "ParticipantListener: on_data_available()\n";
    }

    virtual void on_data_on_readers(DDSSubscriber *subscriber /*subscriber*/)
    {
        std::cout << "ParticipantListener: on_data_on_readers()\n";

        /* notify_datareaders() only calls on_data_available for
         * DataReaders with unread samples
         */
        DDS_ReturnCode_t retcode = subscriber->notify_datareaders();
        if (retcode != DDS_RETCODE_OK) {
            std::cout << "notify_datareaders() error: " << retcode << std::endl;
        }
    }

    virtual void on_inconsistent_topic(
            DDSTopic *topic,
            const DDS_InconsistentTopicStatus &status)
    {
        std::cout << "ParticipantListener: on_inconsistent_topic()\n";
    }
};


class SubscriberListener : public DDSSubscriberListener {
public:
    virtual void on_requested_deadline_missed(
            DDSDataReader * /*reader*/,
            const DDS_RequestedDeadlineMissedStatus & /*status*/)
    {
        std::cout << "SubscriberListener: on_requested_deadline_missed()\n";
    }

    virtual void on_requested_incompatible_qos(
            DDSDataReader * /*reader*/,
            const DDS_RequestedIncompatibleQosStatus & /*status*/)
    {
        std::cout << "SubscriberListener: on_requested_incompatible_qos()\n";
    }

    virtual void on_sample_rejected(
            DDSDataReader * /*reader*/,
            const DDS_SampleRejectedStatus & /*status*/)
    {
        std::cout << "SubscriberListener: on_sample_rejected()\n";
    }

    virtual void on_liveliness_changed(
            DDSDataReader * /*reader*/,
            const DDS_LivelinessChangedStatus & /*status*/)
    {
        std::cout << "SubscriberListener: on_liveliness_changed()\n";
    }

    virtual void on_sample_lost(
            DDSDataReader * /*reader*/,
            const DDS_SampleLostStatus & /*status*/)
    {
        std::cout << "SubscriberListener: on_sample_lost()\n";
    }

    virtual void on_subscription_matched(
            DDSDataReader * /*reader*/,
            const DDS_SubscriptionMatchedStatus & /*status*/)
    {
        std::cout << "SubscriberListener: on_subscription_matched()\n";
    }

    virtual void on_data_available(DDSDataReader *reader)
    {
        std::cout << "SubscriberListener: on_data_available()\n";
    }

    virtual void on_data_on_readers(DDSSubscriber *sub /*subscriber*/)
    {
        static int count = 0;
        std::cout << "SubscriberListener: on_data_on_readers()\n";

        // notify_datareaders() only calls on_data_available for
        // DataReaders with unread samples
        DDS_ReturnCode_t retcode = sub->notify_datareaders();
        if (retcode != DDS_RETCODE_OK) {
            std::cout << "notify_datareaders() error: " << retcode << std::endl;
        }

        if (++count > 3) {
            DDS_StatusMask newmask = DDS_STATUS_MASK_ALL;
            // 'Unmask' DATA_ON_READERS status for listener
            newmask &= ~DDS_DATA_ON_READERS_STATUS;
            sub->set_listener(this, newmask);
            std::cout << "Unregistering "
                         "SubscriberListener::on_data_on_readers()\n";
        }
    }
};


class ReaderListener : public DDSDataReaderListener {
public:
    virtual void on_requested_deadline_missed(
            DDSDataReader * /*reader*/,
            const DDS_RequestedDeadlineMissedStatus & /*status*/)
    {
        std::cout << "ReaderListener: on_requested_deadline_missed()\n";
    }

    virtual void on_requested_incompatible_qos(
            DDSDataReader * /*reader*/,
            const DDS_RequestedIncompatibleQosStatus & /*status*/)
    {
        std::cout << "ReaderListener: on_requested_incompatible_qos()\n";
    }

    virtual void on_sample_rejected(
            DDSDataReader * /*reader*/,
            const DDS_SampleRejectedStatus & /*status*/)
    {
        std::cout << "ReaderListener: on_sample_rejected()\n";
    }

    virtual void on_liveliness_changed(
            DDSDataReader * /*reader*/,
            const DDS_LivelinessChangedStatus &status)
    {
        std::cout << "ReaderListener: on_liveliness_changed()\n";
        std::cout << "  Alive writers: " << status.alive_count << std::endl;
    }

    virtual void on_sample_lost(
            DDSDataReader * /*reader*/,
            const DDS_SampleLostStatus & /*status*/)
    {
        std::cout << "ReaderListener: on_sample_lost()\n";
    }

    virtual void on_subscription_matched(
            DDSDataReader * /*reader*/,
            const DDS_SubscriptionMatchedStatus & /*status*/)
    {
        std::cout << "ReaderListener: on_subscription_matched()\n";
    }

    virtual void on_data_available(DDSDataReader *reader);
};

void ReaderListener::on_data_available(DDSDataReader *reader)
{
    listenersDataReader *listeners_reader = NULL;
    listenersSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;
    int i;

    listeners_reader = listenersDataReader::narrow(reader);
    if (listeners_reader == NULL) {
        std::cout << "DataReader narrow error\n";
        return;
    }

    retcode = listeners_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        std::cout << "take error %d\n", retcode;
        return;
    }

    for (i = 0; i < data_seq.length(); ++i) {
        /* If the reference we get is valid data, it means we have actual
         * data available, otherwise we got metadata */
        if (info_seq[i].valid_data) {
            listenersTypeSupport::print_data(&data_seq[i]);
        } else {
            std::cout << "   Got metadata\n";
        }
    }

    retcode = listeners_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        std::cout << "return loan error " << retcode << std::endl;
    }
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    DDS_Duration_t wait_timeout = { 4, 0 };

    // Create Particiant Listener
    ParticipantListener *participant_listener = new ParticipantListener();
    if (participant_listener == NULL) {
        std::cerr << "participant listener instantiation error" << std::endl;
        return EXIT_FAILURE;
    }

    // Start communicating in a domain, usually one participant per application
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    participant_listener /* listener */,
                    DDS_STATUS_MASK_ALL /* get all statuses */);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    /* Create Subscriber Listener */
    SubscriberListener *subscriber_listener = new SubscriberListener();
    if (subscriber_listener == NULL) {
        return shutdown_participant(
                participant,
                "subscriber listener instantiation error",
                EXIT_FAILURE);
    }

    // A Subscriber allows an application to create one or more DataReaders
    DDSSubscriber *subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            subscriber_listener /* listener */,
            DDS_STATUS_MASK_ALL /* get all statuses */);
    if (subscriber == NULL) {
        return shutdown_participant(
                participant,
                "create_subscriber error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = listenersTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            listenersTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example listeners",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    // Create a data reader listener
    ReaderListener *reader_listener = new ReaderListener();
    if (reader_listener == NULL) {
        return shutdown_participant(
                participant,
                "reader listener instantiation error",
                EXIT_FAILURE);
    }

    /* Here we associate the data reader listener to the reader.
     * We just listen for liveliness changed and data available,
     * since most specific listeners will get called */
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            reader_listener /* listener */,
            DDS_LIVELINESS_CHANGED_STATUS
                    | DDS_DATA_AVAILABLE_STATUS /* statuses */);
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    /* Main loop */
    for (int count = 0; !shutdown_requested && (count < sample_count);
         ++count) {
        NDDSUtility::sleep(wait_timeout);
    }

    // Cleanup
    return shutdown_participant(participant, "Shutting down", 0);
}

// Delete all entities
static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status)
{
    DDS_ReturnCode_t retcode;

    std::cout << shutdown_message << std::endl;

    if (participant != NULL) {
        // Cleanup everything created by this Participant
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_contained_entities error" << retcode
                      << std::endl;
            status = EXIT_FAILURE;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_participant error" << retcode << std::endl;
            status = EXIT_FAILURE;
        }
    }
    return status;
}

int main(int argc, char *argv[])
{
    // Parse arguments and handle control-C
    ApplicationArguments arguments;
    parse_arguments(arguments, argc, argv, subscriber);
    if (arguments.parse_result == PARSE_RETURN_EXIT) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == PARSE_RETURN_FAILURE) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    NDDSConfigLogger::get_instance()->set_verbosity(arguments.verbosity);

    int status = run_subscriber_application(
            arguments.domain_id,
            arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error" << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
