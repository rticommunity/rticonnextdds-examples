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

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "ordered_group.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::sub;
using namespace dds::sub::qos;
using namespace rti::sub;
using namespace dds::topic;

class ordered_groupSubscriberListener: public NoOpSubscriberListener{
  public:
    void on_data_on_readers(Subscriber& subscriber)
    {
        // Create a coherent group access.
        CoherentAccess coherent_access(subscriber);

        // Get the sequence of DataReaders that specifies the order
        // in wich each sample should be read.
        std::vector<AnyDataReader> readers;
        int num_readers = find(subscriber,
            dds::sub::status::DataState::new_data(),
            std::back_inserter(readers));
        std::cout << num_readers << std::endl;

        for (int i = 0; i < num_readers; i++) {
            DataReader<ordered_group> reader =
                readers[i].get<ordered_group>();

            // We need to take only one sample each time, as we want to follow
            // the sequence of DataReaders. This way the samples will be
            // returned in the order in which they were modified.
            LoanedSamples<ordered_group> sample =
                reader.select().max_samples(1).take();

            if (sample.length() > 0 && sample[0].info().valid()) {
                std::cout << sample[0].data() << std::endl;
            }
        }

        // The destructor will ends the coherent group access
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Retrieve the default Subscriber QoS, from USER_QOS_PROFILES.xml
    SubscriberQos subscriber_qos = QosProvider::Default().subscriber_qos();

    // If you want to change the Subscriber's QoS programmatically rather than
    // using the XML file, uncomment the following line.

    // subscriber_qos << Presentation::GroupAccessScope(false, true);

    // Create a Subscriber.
    Subscriber subscriber(participant, subscriber_qos);

    // Associate a listener to the subscriber using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    ListenerBinder<Subscriber> subscriber_listener =
        rti::core::bind_and_manage_listener(
            subscriber,
            new ordered_groupSubscriberListener,
            dds::core::status::StatusMask::data_on_readers());

    // Create three Topic, once for each DataReader.
    Topic<ordered_group> topic1(participant, "Topic1");
    Topic<ordered_group> topic2(participant, "Topic2");
    Topic<ordered_group> topic3(participant, "Topic3");

    // Create one DataReader for each Topic.
    DataReader<ordered_group> reader1(subscriber, topic1);
    DataReader<ordered_group> reader2(subscriber, topic2);
    DataReader<ordered_group> reader3(subscriber, topic3);

    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        std::cout << "ordered_group subscriber sleeping for 1 sec..."
                  << std::endl;
        rti::util::sleep(Duration(1));
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop

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
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
