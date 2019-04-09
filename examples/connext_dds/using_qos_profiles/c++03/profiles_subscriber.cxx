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
#include <cstdlib>

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "profiles.hpp"

using namespace dds::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;

class ProfilesListener : public NoOpDataReaderListener<profiles> {
public:
    ProfilesListener(std::string name) : listener_name_(name)
    {
    }

    void on_data_available(DataReader<profiles>& reader)
    {
        // Take all samples
        LoanedSamples<profiles> samples = reader.take();

        std::cout << "============================================="
                  << std::endl << listener_name() << " listener received"
                  << std::endl;

        for (LoanedSamples<profiles>::iterator sample_it = samples.begin();
            sample_it != samples.end(); sample_it++) {

            if (sample_it->info().valid()) {
                std::cout << sample_it->data() << std::endl;
            }
        }

        std::cout << "============================================="
                  << std::endl << std::endl;
    }

    std::string listener_name() const
    {
        return listener_name_;
    }

private:
    const std::string listener_name_;
};

void subscriber_main(int domain_id, int sample_count)
{
    // Retrieve QoS from custom profile XML and USER_QOS_PROFILES.xml
    QosProvider qos_provider("my_custom_qos_profiles.xml");

    // Create a DomainParticipant with the default QoS of the provider.
    DomainParticipant participant(domain_id, qos_provider.participant_qos());

    // Create a Subscriber with default QoS.
    Subscriber subscriber(participant, qos_provider.subscriber_qos());

    // Create a Topic with default QoS.
    Topic<profiles> topic(
        participant,
        "Example profiles",
        qos_provider.topic_qos());

    // Create a DataWriter with the QoS profile "transient_local_profile" that
    // it is inside the QoS library "profiles_Library".
    DataReader<profiles> reader_transient_local(subscriber, topic,
        qos_provider.datareader_qos(
            "profiles_Library::transient_local_profile"));

    // Use a ListeberBinder to take care of resetting and deleting the listener.
    rti::core::ListenerBinder<DataReader<profiles> > scoped_transient_listener =
        rti::core::bind_and_manage_listener(
            reader_transient_local,
            new ProfilesListener("transient_local_profile"),
            StatusMask::data_available());

    // Create a DataReader with the QoS profile "volatile_profile" that it is
    // inside the QoS library "profiles_Library".
    DataReader<profiles> reader_volatile(subscriber, topic,
        qos_provider.datareader_qos(
            "profiles_Library::volatile_profile"));

    // Use a ListeberBinder to take care of resetting and deleting the listener.
    rti::core::ListenerBinder<DataReader<profiles> > scoped_volatile_listener =
        rti::core::bind_and_manage_listener(
            reader_volatile,
            new ProfilesListener("volatile_profile"),
            StatusMask::data_available());

    // Main loop.
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        rti::util::sleep(Duration(1));
    }
}

int main(int argc, char* argv[])
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
