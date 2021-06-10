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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "listeners.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

class MyParticipantListener
        : public dds::domain::NoOpDomainParticipantListener {
public:
    virtual void on_requested_deadline_missed(
            dds::pub::AnyDataWriter &writer,
            const dds::core::status::OfferedDeadlineMissedStatus &status)
    {
        std::cout << "ParticipantListener: on_requested_deadline_missed()"
                  << std::endl;
    }

    virtual void on_offered_incompatible_qos(
            dds::pub::AnyDataWriter &writer,
            const ::dds::core::status::OfferedIncompatibleQosStatus &status)
    {
        std::cout << "ParticipantListener: on_offered_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_sample_rejected(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::SampleRejectedStatus &status)
    {
        std::cout << "ParticipantListener: on_sample_rejected()" << std::endl;
    }

    virtual void on_liveliness_changed(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::LivelinessChangedStatus &status)
    {
        std::cout << "ParticipantListener: on_liveliness_changed()"
                  << std::endl;
    }


    virtual void on_sample_lost(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::SampleLostStatus &status)
    {
        std::cout << "ParticipantListener: on_sample_lost()" << std::endl;
    }

    virtual void on_subscription_matched(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::SubscriptionMatchedStatus &status)
    {
        std::cout << "ParticipantListener: on_subscription_matched()"
                  << std::endl;
    }

    virtual void on_data_available(dds::sub::AnyDataReader &the_reader)
    {
        std::cout << "ParticipantListener: on_data_available()" << std::endl;
    }


    virtual void on_data_on_readers(dds::sub::Subscriber &sub)
    {
        // Notify DataReaders only calls on_data_available for
        // DataReaders with unread samples.
        sub.notify_datareaders();
        std::cout << "ParticipantListener: on_data_on_readers()" << std::endl;
    }

    virtual void on_inconsistent_topic(
            dds::topic::AnyTopic &topic,
            const dds::core::status::InconsistentTopicStatus &status)
    {
        std::cout << "ParticipantListener: on_inconsistent_topic()"
                  << std::endl;
    }
};

class MySubscriberListener
        : public dds::sub::NoOpSubscriberListener,
          public std::enable_shared_from_this<MySubscriberListener> {
public:
    virtual void on_requested_deadline_missed(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::RequestedDeadlineMissedStatus &status)
    {
        std::cout << "SubscriberListener: on_requested_deadline_missed()"
                  << std::endl;
    }

    virtual void on_requested_incompatible_qos(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::RequestedIncompatibleQosStatus &status)
    {
        std::cout << "SubscriberListener: on_requested_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_sample_rejected(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::SampleRejectedStatus &status)
    {
        std::cout << "SubscriberListener: on_sample_rejected()" << std::endl;
    }

    virtual void on_liveliness_changed(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::LivelinessChangedStatus &status)
    {
        std::cout << "SubscriberListener: on_liveliness_changed()" << std::endl;
    }

    virtual void on_sample_lost(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::SampleLostStatus &status)
    {
        std::cout << "SubscriberListener: on_sample_lost()" << std::endl;
    }

    virtual void on_subscription_matched(
            dds::sub::AnyDataReader &the_reader,
            const dds::core::status::SubscriptionMatchedStatus &status)
    {
        std::cout << "SubscriberListener: on_subscription_matched()"
                  << std::endl;
    }

    virtual void on_data_available(dds::sub::AnyDataReader &the_reader)
    {
        std::cout << "SubscriberListener: on_data_available()" << std::endl;
    }

    virtual void on_data_on_readers(dds::sub::Subscriber &sub)
    {
        static int count = 0;
        std::cout << "SubscriberListener: on_data_on_readers()" << std::endl;

        sub->notify_datareaders();
        if (++count > 3) {
            auto subscriber_listener = shared_from_this();
            dds::core::status::StatusMask new_mask =
                    dds::core::status::StatusMask::all();
            new_mask &= ~dds::core::status::StatusMask::data_on_readers();
            sub.set_listener(subscriber_listener, new_mask);
        }
    }
};

class MyDataReaderListener
        : public dds::sub::NoOpDataReaderListener<listeners> {
    virtual void on_requested_deadline_missed(
            dds::sub::DataReader<listeners> &reader,
            const dds::core::status::RequestedDeadlineMissedStatus &status)
    {
        std::cout << "ReaderListener: on_requested_deadline_missed()"
                  << std::endl;
    }

    virtual void on_requested_incompatible_qos(
            dds::sub::DataReader<listeners> &reader,
            const dds::core::status::RequestedIncompatibleQosStatus &status)
    {
        std::cout << "ReaderListener: on_requested_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_sample_rejected(
            dds::sub::DataReader<listeners> &reader,
            const dds::core::status::SampleRejectedStatus &status)
    {
        std::cout << "ReaderListener: on_sample_rejected()" << std::endl;
    }

    virtual void on_liveliness_changed(
            dds::sub::DataReader<listeners> &reader,
            const dds::core::status::LivelinessChangedStatus &status)
    {
        std::cout << "ReaderListener: on_liveliness_changed()" << std::endl
                  << "  Alive writers: " << status.alive_count() << std::endl;
    }

    virtual void on_sample_lost(
            dds::sub::DataReader<listeners> &reader,
            const dds::core::status::SampleLostStatus &status)
    {
        std::cout << "ReaderListener: on_sample_lost()" << std::endl;
    }

    virtual void on_subscription_matched(
            dds::sub::DataReader<listeners> &reader,
            const dds::core::status::SubscriptionMatchedStatus &status)
    {
        std::cout << "ReaderListener: on_subscription_matched()" << std::endl;
    }

    virtual void on_data_available(dds::sub::DataReader<listeners> &reader)
    {
        dds::sub::LoanedSamples<listeners> samples = reader.take();
        for (const auto &sample : samples) {
            // If the reference we get is valid data, it means we have actual
            // data available, otherwise we got metadata.
            if (sample.info().valid()) {
                std::cout << sample.data() << std::endl;
            } else {
                std::cout << "  Got metadata" << std::endl;
            }
        }
    }
};

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a shared pointer for the Participant Listener
    auto participant_listener = std::make_shared<MyParticipantListener>();

    // Create the participant
    dds::domain::DomainParticipant participant(domain_id);

    // Associate a listener to the participant using a shared pointer. It
    // will take care of setting it to NULL on destruction.
    participant.set_listener(participant_listener);

    // To customize topic QoS, use the configuration file USER_QOS_PROFILES.xml
    dds::topic::Topic<listeners> topic(participant, "Example listeners");

    // Create the subscriber and associate a listener
    dds::sub::Subscriber subscriber(participant);
    auto subscriber_listener = std::make_shared<MySubscriberListener>();
    subscriber.set_listener(subscriber_listener);

    // Create the DataReader and associate a listener
    dds::sub::DataReader<listeners> reader(subscriber, topic);
    auto dw_listener = std::make_shared<MyDataReaderListener>();
    reader.set_listener(dw_listener);

    // Main loop
    for (int count = 0;
         !application::shutdown_requested && (count < sample_count);
         ++count) {
        // Each "sample_count" is four seconds.
        rti::util::sleep(dds::core::Duration(4));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
