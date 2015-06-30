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

#include "ordered.hpp"
#include <dds/dds.hpp>

using namespace dds::core;
using namespace dds::core::cond;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::cond;
using namespace dds::sub::qos;
using namespace dds::sub::status;

void poll_readers(std::vector< DataReader<ordered> >& readers)
{
    for (std::vector<int>::size_type i = 0; i < readers.size(); i++) {
        LoanedSamples<ordered> samples = readers[i].take();
        for (auto sample : samples) {
            if (sample.info().valid()) {
                std::cout << std::string(i, '\t') << "Reader " << i
                          << ": Instance" << sample.data().id()
                          << "->value = " << sample.data().value()
                          << std::endl;
            }
        }
    }
}

void subscriber_main(int domain_id, int sample_count)
{
    DomainParticipant participant(domain_id);

    Topic<ordered> topic(participant, "Example ordered");

    std::vector<std::string> profile_names = {
        "ordered_Library::ordered_Profile_subscriber_instance",
        "ordered_Library::ordered_Profile_subscriber_topic" };

    std::vector< DataReader<ordered> > readers;
    for (std::vector<int>::size_type i = 0; i < profile_names.size(); i++) {
        std::cout << "Subscriber " << i << " using " << profile_names[i]
                  << std::endl;

        SubscriberQos subscriber_qos = QosProvider::Default().subscriber_qos(
            profile_names[i]);

        // SubscriberQos subscriber_qos;
        // if (i == 0) {
        //     subscriber_qos << Presentation::InstanceAccessScope(false, true);
        // } else if (i == 1) {
        //     subscriber_qos << Presentation::TopicAccessScope(false, true);
        // }

        Subscriber subscriber(participant, subscriber_qos);

        DataReaderQos reader_qos = QosProvider::Default().datareader_qos(
            profile_names[i]);

        // DataReaderQos reader_qos;
        // reader_qos << History::KeepLast(10)
        //            << Reliability::Reliable();

        DataReader<ordered> reader(subscriber, topic, reader_qos);
        readers.push_back(reader);
    }

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        std::cout << "ordered subscriber sleeping for 4 sec.." << std::endl;
        rti::util::sleep(Duration(4));
        poll_readers(readers);
    }
}

int main(int argc, char *argv[])
{
    int domain_id    = 0;
    int sample_count = 0; // infinite loop

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
