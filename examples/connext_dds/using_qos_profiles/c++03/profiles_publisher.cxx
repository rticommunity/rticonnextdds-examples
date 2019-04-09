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
#include "profiles.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;

void publisher_main(int domain_id, int sample_count)
{
    // Retrieve QoS from custom profile XML and USER_QOS_PROFILES.xml
    QosProvider qos_provider("my_custom_qos_profiles.xml");

    // Create a DomainParticipant with the default QoS of the provider.
    DomainParticipant participant(domain_id, qos_provider.participant_qos());

    // Create a Publisher with default QoS.
    Publisher publisher(participant, qos_provider.publisher_qos());

    // Create a Topic with default QoS.
    Topic<profiles> topic(
        participant,
        "Example profiles",
        qos_provider.topic_qos());

    // Create a DataWriter with the QoS profile "transient_local_profile",
    // from QoS library "profiles_Library".
    DataWriter<profiles> writer_transient_local(publisher, topic,
        qos_provider.datawriter_qos(
            "profiles_Library::transient_local_profile"));

    // Create a DataReader with the QoS profile "volatile_profile",
    // from the QoS library "profiles_Library".
    DataWriter<profiles> writer_volatile(publisher, topic,
        qos_provider.datawriter_qos(
            "profiles_Library::volatile_profile"));

    // Create a data sample for writing.
    profiles instance;

    // Main loop.
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        // Update the counter value of the sample.
        instance.x(count);

        // Send the sample using the DataWriter with "volatile" durability.
        std::cout << "Writing profile_name = volatile_profile,\t x = " << count
                  << std::endl;
        instance.profile_name("volatile_profile");
        writer_volatile.write(instance);

        // Send the sample using the DataWriter with "transient_local"
        // durability.
        std::cout << "Writing profile_name = transient_local_profile,\t x = "
                  << count << std::endl << std::endl;
        instance.profile_name("transient_local_profile");
        writer_transient_local.write(instance);

        // Send the sample every second.
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
        publisher_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
