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
#include "ordered_group.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::pub;
using namespace dds::pub::qos;
using namespace dds::topic;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Retrieve the default Publisher QoS, from USER_QOS_PROFILES.xml
    PublisherQos publisher_qos = QosProvider::Default().publisher_qos();

    // If you want to change the Publisher's QoS programmatically rather than
    // using the XML file, uncomment the following line.

    // publisher_qos << Presentation::GroupAccessScope(false, true);

    // Create a single Publisher for all DataWriters.
    Publisher publisher(participant, publisher_qos);

    // Create three Topic, once for each DataWriter.
    Topic<ordered_group> topic1(participant, "Topic1");
    Topic<ordered_group> topic2(participant, "Topic2");
    Topic<ordered_group> topic3(participant, "Topic3");

    // Create three DataWriter. Since the QoS policy is set to 'GROUP',
    // and the three writers belong to the same Publisher, the instance change
    // will be available for the Subscriber in the same order.
    DataWriter<ordered_group> writer1(publisher, topic1);
    DataWriter<ordered_group> writer2(publisher, topic2);
    DataWriter<ordered_group> writer3(publisher, topic3);

    // Create one instance for each DataWriter.
    ordered_group instance1;
    ordered_group instance2;
    ordered_group instance3;

    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        std::cout << "Writing ordered_group, count " << count << std::endl;

        // Modify twice the first instance and send with the first writer.
        instance1.message("First sample, Topic 1 sent by DataWriter number 1");
        writer1.write(instance1);
        instance1.message("Second sample, Topic 1 sent by DataWriter number 1");
        writer1.write(instance1);

        // Modify twice the second instance and send with the second writer.
        instance2.message("First sample, Topic 2 sent by DataWriter number 2");
        writer2.write(instance2);
        instance2.message("Second sample, Topic 2 sent by DataWriter number 2");
        writer2.write(instance2);

        // Modify twice the third instance and send with the third writer.
        instance3.message("First sample, Topic 3 sent by DataWriter number 3");
        writer3.write(instance3);
        instance3.message("Second sample, Topic 3 sent by DataWriter number 3");
        writer3.write(instance3);

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
        publisher_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
