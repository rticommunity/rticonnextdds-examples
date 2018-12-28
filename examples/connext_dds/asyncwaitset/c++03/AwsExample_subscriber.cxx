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
#include <rti/core/cond/AsyncWaitSet.hpp>
#include <rti/util/util.hpp> // for sleep()
#include <rti/config/Logger.hpp>

#include "AwsExample.hpp"

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
    
    DataAvailableHandler(AwsSubscriber& subscriber) : subscriber_(subscriber)
    {
    }
    
private:
    
    AwsSubscriber& subscriber_;
    
};

// AwsSubscriberPlayer implementation

const std::string AwsSubscriber::TOPIC_NAME = "AwsExample Example";

AwsSubscriber::AwsSubscriber(
        DDS_DomainId_t domain_id,
        rti::core::cond::AsyncWaitSet async_waitset)
    : receiver_(dds::core::null),
      async_waitset_(async_waitset)
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
    async_waitset_.unlock_condition(dds::core::cond::StatusCondition(receiver_));

    // Process sample 
    for (dds::sub::LoanedSamples<AwsExample>::iterator sample_it = samples.begin();
         sample_it != samples.end();
         sample_it++) {
        if (sample_it->info().valid()) {
            std::cout 
                    << "Received sample:\n\t" 
                    << sample_it->data() 
                    << std::endl;
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
            .received_sample_count().total();
}

AwsSubscriber::~AwsSubscriber()
{
    async_waitset_.detach_condition(dds::core::cond::StatusCondition(receiver_));
}



int main(int argc, char *argv[])
{
    int domain_id = 0;
    int thread_pool_size = 4;
    int sample_count = 0; // infinite loop
    const std::string USAGE(
            "AwsSubscriber_subscriber [options]\n"
            "Options:\n"
            "\t-d, -domainId: Domain ID\n"
            "\t-t, -threads: Number of threads used to process sample reception\n"
            "\t-s, -samples: Total number of received samples before exiting\n");
    
    srand(time(NULL));

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0
                || strcmp(argv[i], "-domainId") == 0) {
            if (i == argc - 1) {
                std::cerr << "missing domain ID parameter value " << std::endl;
                return -1;
            } else {
                domain_id = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-t") == 0
                || strcmp(argv[i], "-threads") == 0) {
            if (i == argc - 1) {
                std::cerr << "missing threads parameter value " << std::endl;
                return -1;
            } else {
                thread_pool_size = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-s") == 0
                || strcmp(argv[i], "-samples") == 0) {
            if (i == argc - 1) {
                std::cerr << "missing samples parameter value " << std::endl;
                return -1;
            } else {
                sample_count = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-h") == 0
                || strcmp(argv[i], "-help") == 0) {
            std::cout << USAGE << std::endl;
            return 0;
        }
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {

        // An AsyncWaitSet (AWS) for multi-threaded events .
        // The AWS will provide the infrastructure to receive samples using
        // multiple threads.
        rti::core::cond::AsyncWaitSet async_waitset(
                rti::core::cond::AsyncWaitSetProperty()
                    .thread_pool_size(thread_pool_size));   
            
        async_waitset.start();
        
        AwsSubscriber subscriber(
                domain_id, 
                async_waitset); 
          
        std::cout << "Wait for samples..." << std::endl;
        while (subscriber.received_count() < sample_count || sample_count == 0) {   
            rti::util::sleep(dds::core::Duration(1));
        }
        
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

