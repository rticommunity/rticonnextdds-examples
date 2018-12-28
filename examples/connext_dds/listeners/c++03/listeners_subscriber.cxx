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

#include <iostream>

#include "listeners.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace rti::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::sub;
using namespace dds::topic;

class MyParticipantListener : public NoOpDomainParticipantListener {
public:
    virtual void on_requested_deadline_missed(
        dds::pub::AnyDataWriter& writer,
        const dds::core::status::OfferedDeadlineMissedStatus& status)
    {
        std::cout << "ParticipantListener: on_requested_deadline_missed()"
                  << std::endl;
    }

    virtual void on_offered_incompatible_qos(
        dds::pub::AnyDataWriter& writer,
        const ::dds::core::status::OfferedIncompatibleQosStatus& status)
    {
        std::cout << "ParticipantListener: on_offered_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_sample_rejected(
        dds::sub::AnyDataReader& the_reader,
        const dds::core::status::SampleRejectedStatus& status)
    {
        std::cout << "ParticipantListener: on_sample_rejected()"
                  << std::endl;
    }

    virtual void on_liveliness_changed(
        dds::sub::AnyDataReader& the_reader,
        const dds::core::status::LivelinessChangedStatus& status)
    {
        std::cout << "ParticipantListener: on_liveliness_changed()"
                  << std::endl;
    }


    virtual void on_sample_lost(
        dds::sub::AnyDataReader& the_reader,
        const dds::core::status::SampleLostStatus& status)
    {
        std::cout << "ParticipantListener: on_sample_lost()"
                  << std::endl;
    }

    virtual void on_subscription_matched(
        dds::sub::AnyDataReader& the_reader,
        const dds::core::status::SubscriptionMatchedStatus& status)
    {
        std::cout << "ParticipantListener: on_subscription_matched()"
                  << std::endl;
    }

    virtual void on_data_available(dds::sub::AnyDataReader& the_reader)
    {
        std::cout << "ParticipantListener: on_data_available()"
                  << std::endl;
    }


    virtual void on_data_on_readers(dds::sub::Subscriber& sub)
    {
        // Notify DataReaders only calls on_data_available for
        // DataReaders with unread samples.
        sub.notify_datareaders();
        std::cout << "ParticipantListener: on_data_on_readers()"
                  << std::endl;
    }

    virtual void on_inconsistent_topic(
        dds::topic::AnyTopic& topic,
        const dds::core::status::InconsistentTopicStatus& status)
    {
        std::cout << "ParticipantListener: on_inconsistent_topic()"
                  << std::endl;
    }
};

class MySubscriberListener : public NoOpSubscriberListener {
public:
    virtual void on_requested_deadline_missed(
        AnyDataReader& the_reader,
        const dds::core::status::RequestedDeadlineMissedStatus& status)
    {
        std::cout << "SubscriberListener: on_requested_deadline_missed()"
                  << std::endl;
    }

    virtual void on_requested_incompatible_qos(
        AnyDataReader& the_reader,
        const dds::core::status::RequestedIncompatibleQosStatus& status)
    {
        std::cout << "SubscriberListener: on_requested_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_sample_rejected(
        AnyDataReader& the_reader,
        const dds::core::status::SampleRejectedStatus& status)
    {
        std::cout << "SubscriberListener: on_sample_rejected()"
                  << std::endl;
    }

    virtual void on_liveliness_changed(
        AnyDataReader& the_reader,
        const dds::core::status::LivelinessChangedStatus& status)
    {
        std::cout << "SubscriberListener: on_liveliness_changed()"
                  << std::endl;
    }

    virtual void on_sample_lost(
        AnyDataReader& the_reader,
        const dds::core::status::SampleLostStatus& status)
    {
        std::cout << "SubscriberListener: on_sample_lost()"
                  << std::endl;
    }

    virtual void on_subscription_matched(
        AnyDataReader& the_reader,
        const dds::core::status::SubscriptionMatchedStatus& status)
    {
        std::cout << "SubscriberListener: on_subscription_matched()"
                  << std::endl;
    }

    virtual void on_data_available(AnyDataReader& the_reader)
    {
        std::cout << "SubscriberListener: on_data_available()"
                  << std::endl;
    }

    virtual void on_data_on_readers(Subscriber& sub)
    {
        static int count = 0;
        std::cout << "SubscriberListener: on_data_on_readers()"
                  << std::endl;

        sub->notify_datareaders();
        if (++count > 3) {
            StatusMask new_mask = StatusMask::all();
            new_mask &= ~StatusMask::data_on_readers();
            sub.listener(this, new_mask);
        }
    }
};

class MyDataReaderListener : public NoOpDataReaderListener<listeners> {
    virtual void on_requested_deadline_missed(
        DataReader<listeners>& reader,
        const dds::core::status::RequestedDeadlineMissedStatus& status)
    {
        std::cout << "ReaderListener: on_requested_deadline_missed()"
                  << std::endl;
    }

    virtual void on_requested_incompatible_qos(
        DataReader<listeners>& reader,
        const dds::core::status::RequestedIncompatibleQosStatus& status)
    {
        std::cout << "ReaderListener: on_requested_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_sample_rejected(
        DataReader<listeners>& reader,
        const dds::core::status::SampleRejectedStatus& status)
    {
        std::cout << "ReaderListener: on_sample_rejected()"
                  << std::endl;
    }

    virtual void on_liveliness_changed(
        DataReader<listeners>& reader,
        const dds::core::status::LivelinessChangedStatus& status)
    {
        std::cout << "ReaderListener: on_liveliness_changed()"   << std::endl
                  << "  Alive writers: " << status.alive_count() << std::endl;
    }

    virtual void on_sample_lost(
        DataReader<listeners>& reader,
        const dds::core::status::SampleLostStatus& status)
    {
        std::cout << "ReaderListener: on_sample_lost()"
                  << std::endl;
    }

    virtual void on_subscription_matched(
        DataReader<listeners>& reader,
        const dds::core::status::SubscriptionMatchedStatus& status)
    {
        std::cout << "ReaderListener: on_subscription_matched()"
                  << std::endl;
    }

    virtual void on_data_available(DataReader<listeners>& reader)
    {
        LoanedSamples<listeners> samples = reader.take();
        for (LoanedSamples<listeners>::iterator sampleIt = samples.begin();
                sampleIt != samples.end();
                ++sampleIt) {

            // If the reference we get is valid data, it means we have actual
            // data available, otherwise we got metadata.
            if (sampleIt->info().valid()) {
                std::cout << sampleIt->data() << std::endl;
            } else {
                std::cout << "  Got metadata" << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create the participant
    DomainParticipant participant (domain_id);

    // Associate a listener to the participant using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    ListenerBinder<DomainParticipant> participant_listener =
        rti::core::bind_and_manage_listener(
            participant,
            new MyParticipantListener,
            dds::core::status::StatusMask::all());

    // To customize topic QoS, use the configuration file USER_QOS_PROFILES.xml
    Topic<listeners> topic (participant, "Example listeners");

    // Create the subscriber and associate a listener
    Subscriber subscriber (participant);
    ListenerBinder<Subscriber> subscriber_listener =
        rti::core::bind_and_manage_listener(
            subscriber,
            new MySubscriberListener,
            dds::core::status::StatusMask::all());

    // Create the DataReader and associate a listener
    DataReader<listeners> reader (subscriber, topic);
    ListenerBinder< DataReader<listeners> > datareader_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new MyDataReaderListener,
            dds::core::status::StatusMask::all());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        // Each "sample_count" is four seconds.
        rti::util::sleep(Duration(4));
    }
}

int main(int argc, char* argv[])
{
    int domain_id = 0;
    int sample_count = 0;   // Infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
