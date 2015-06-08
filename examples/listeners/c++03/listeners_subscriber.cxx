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

#include "listeners.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace rti::core;
using namespace dds::domain;
using namespace dds::sub;
using namespace dds::topic;

class MyParticipantListener : public NoOpDomainParticipantListener {

};

class MySubscriberListener : public NoOpSubscriberListener {

};

class MyDataReaderListener : public NoOpDataReaderListener<listeners> {

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
