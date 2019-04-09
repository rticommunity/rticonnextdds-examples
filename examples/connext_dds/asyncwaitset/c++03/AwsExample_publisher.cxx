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

#include "AwsExample.hpp"

class AwsPublisher {
    
public:        
    
    static const std::string TOPIC_NAME;
    
    AwsPublisher(
            DDS_DomainId_t domain_id,
            int publisher_id,
            rti::core::cond::AsyncWaitSet async_waitset);
    
    void generate_send_event();

    void send_sample();
    
    ~AwsPublisher();
    
private:    
        
    // A writer to send data
    dds::pub::DataWriter<AwsExample> sender_;
    // Sample buffer
    AwsExample sample_;
    // A GuardCondition to generate app-driven sends
    dds::core::cond::GuardCondition send_condition_;
    // Reference to the AWS used for writing the samples
    rti::core::cond::AsyncWaitSet async_waitset_;
};


class SendRequestHandler {
    
public:

    // Handles the send sample condition
    void operator()(dds::core::cond::Condition condition)
    {
        // condition is the same than sendCondition_ so it is safe to
        // perform the downcast. It is important to reset the condition trigger
        // to avoid continuous wakeup and dispatch
        dds::core::cond::GuardCondition guard_condition =
                dds::core::polymorphic_cast<dds::core::cond::GuardCondition>(condition);
        guard_condition.trigger_value(false); 
        publisher_.send_sample();
    }
    
    SendRequestHandler(AwsPublisher& publisher) : publisher_(publisher) 
    {
    }
    
private:
    
    AwsPublisher& publisher_;
    
};

// AwsPublisher implementation

const std::string AwsPublisher::TOPIC_NAME = "AwsExample Example";

AwsPublisher::AwsPublisher(
        DDS_DomainId_t domain_id,
        int publisher_id,
        rti::core::cond::AsyncWaitSet async_waitset)
    : sender_(dds::core::null),
      async_waitset_(async_waitset)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<AwsExample> topic(participant, TOPIC_NAME);

    // Create a DataWriter with default Qos (Publisher created in-line)
    sender_ = dds::pub::DataWriter<AwsExample>(
            dds::pub::Publisher(participant),
            topic);
    // set sample key value:
    sample_.key(publisher_id);
    

    // Send condition: to generate application-driven events to send samples
    send_condition_.handler(SendRequestHandler(*this));
    async_waitset_.attach_condition(send_condition_);
  
}

void AwsPublisher::generate_send_event()
{
    send_condition_.trigger_value(true);
}

void AwsPublisher::send_sample()
{
    std::cout << "Send Sample: " << sample_.number() << std::endl;
    sample_.number(sample_.number() + 1);
    sender_.write(sample_);    
}

AwsPublisher::~AwsPublisher()
{
    async_waitset_.detach_condition(send_condition_);
}



int main(int argc, char *argv[])
{
    int domain_id = 0;
    int publisher_id = 0;
    int sample_count = 0; // infinite loop
    const std::string USAGE(
            "Aws_publisher [options]\n"
            "Options:\n"
            "\t-d, -domainId: Domain ID\n"
            "\t-p, -publisherId: Key value for the Aws samples\n"
            "\t-s, -samples: Total number of published samples\n");
    
    srand(time(NULL));
    publisher_id = rand() % RAND_MAX;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0
                || strcmp(argv[i], "-domainId") == 0) {
            if (i == argc - 1) {
                std::cerr << "missing domain ID parameter value " << std::endl;
                return -1;
            } else {
                domain_id = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-p") == 0
                || strcmp(argv[i], "-publisherId") == 0) {
            if (i == argc - 1) {
                std::cerr << "missing publisher ID parameter value " << std::endl;
                return -1;
            } else {
                publisher_id = atoi(argv[++i]);
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

        // An AsyncWaitSet (AWS) to send samples in a separate thread       
        rti::core::cond::AsyncWaitSet async_waitset;                        
        async_waitset.start();
        
        AwsPublisher publisher(domain_id, publisher_id, async_waitset);
        
        // Generate periodic send events    
        for (int count = 0; count < sample_count || sample_count == 0; count++) {            
            publisher.generate_send_event();
            rti::util::sleep(dds::core::Duration(1));            
        }
        
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

