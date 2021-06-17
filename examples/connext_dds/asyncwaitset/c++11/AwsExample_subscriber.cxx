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

#include <dds/core/ddscore.hpp>
#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include <rti/config/Logger.hpp>
#include <rti/core/cond/AsyncWaitSet.hpp>
#include <rti/util/util.hpp>  // for sleep()

#include "AwsExample.hpp"
#include "application.hpp"

class AwsSubscriber {
public:
    static const std::string TOPIC_NAME;

    AwsSubscriber(
            DDS_DomainId_t domain_id,
            rti::core::cond::AsyncWaitSet async_waitset);

    void process_received_samples();

    int received_count();

    ~AwsSubscriber();

public:
    // Entities to receive data
    dds::sub::DataReader<AwsExample> receiver_;
    // Reference to the AWS used for processing the events
    rti::core::cond::AsyncWaitSet async_waitset_;
};

class DataAvailableHandler {
public:
    /* Handles the reception of samples */
    void operator()()
    {
        subscriber_.process_received_samples();
    }

    DataAvailableHandler(AwsSubscriber &subscriber) : subscriber_(subscriber)
    {
    }

private:
    AwsSubscriber &subscriber_;
};

// AwsSubscriberPlayer implementation

const std::string AwsSubscriber::TOPIC_NAME = "AwsExample Example";

AwsSubscriber::AwsSubscriber(
        DDS_DomainId_t domain_id,
        rti::core::cond::AsyncWaitSet async_waitset)
        : receiver_(dds::core::null), async_waitset_(async_waitset)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<AwsExample> topic(participant, TOPIC_NAME);

    // Create a DataReader with default Qos (Subscriber created in-line)
    receiver_ = dds::sub::DataReader<AwsExample>(
            dds::sub::Subscriber(participant),
            topic);

    // DataReader status condition: to process the reception of samples
    dds::core::cond::StatusCondition reader_status_condition(receiver_);
    reader_status_condition.enabled_statuses(
            dds::core::status::StatusMask::data_available());
    reader_status_condition->handler(DataAvailableHandler(*this));
    async_waitset_.attach_condition(reader_status_condition);
}

void AwsSubscriber::process_received_samples()
{
    // Take all samples This will reset the StatusCondition
    dds::sub::LoanedSamples<AwsExample> samples = receiver_.take();

    // Release status condition in case other threads can process outstanding
    // samples
    async_waitset_.unlock_condition(
            dds::core::cond::StatusCondition(receiver_));

    // Process sample
    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            std::cout << "Received sample:\n\t" << sample.data() << std::endl;
        }
    }
    // Sleep a random amount of time between 1 and 10 secs. This is
    // intended to cause the handling thread of the AWS to take a long
    // time dispatching
    rti::util::sleep(dds::core::Duration::from_secs(rand() % 10 + 1));
}

int AwsSubscriber::received_count()
{
    return receiver_->datareader_protocol_status()
            .received_sample_count()
            .total();
}

AwsSubscriber::~AwsSubscriber()
{
    async_waitset_.detach_condition(
            dds::core::cond::StatusCondition(receiver_));
}


int main(int argc, char *argv[])
{
    using namespace application;

    srand(time(NULL));

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, ApplicationKind::Subscriber);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        // An AsyncWaitSet (AWS) for multi-threaded events .
        // The AWS will provide the infrastructure to receive samples using
        // multiple threads.
        rti::core::cond::AsyncWaitSet async_waitset(
                rti::core::cond::AsyncWaitSetProperty().thread_pool_size(
                        arguments.thread_pool_size));

        async_waitset.start();

        AwsSubscriber subscriber(arguments.domain_id, async_waitset);

        std::cout << "Wait for samples..." << std::endl;
        while (!application::shutdown_requested
               && subscriber.received_count() < arguments.sample_count) {
            rti::util::sleep(dds::core::Duration(1));
        }

    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in main(): " << ex.what() << std::endl;
        return -1;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
