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
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant.
    DomainParticipant participant(domain_id);

    // Retrieve the custom ordered publisher QoS, from USER_QOS_PROFILES.xml
    PublisherQos publisher_qos = QosProvider::Default().publisher_qos(
        "ordered_Library::ordered_Profile_subscriber_instance");

    // If you want to change the Publisher's QoS programmatically rather
    // than using the XML file, you will need to comment out the previous
    // publisher_qos assignment and uncomment these files.

    // PublisherQos publisher_qos;
    // publisher_qos << Presentation::TopicAccessScope(false, true);

    // Create a publisher with the ordered QoS.
    Publisher publisher(participant, publisher_qos);

    // Create a Topic -- and automatically register the type.
    Topic<ordered> topic(participant, "Example ordered");

    // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos(
        "ordered_Library::ordered_Profile_subscriber_instance");

    // If you want to change the Publisher's QoS programmatically rather
    // than using the XML file, you will need to comment out the previous
    // writer_qos assignment and uncomment these files.

    // DataWriterQos writer_qos;
    // writer_qos << Reliability::Reliable();

    // Create a DataWriter.
    DataWriter<ordered> writer(publisher, topic, writer_qos);

    // Create two samples with different ID and register them
    ordered instance0(0, 0);
    InstanceHandle handle0 = writer.register_instance(instance0);

    ordered instance1(1, 0);
    InstanceHandle handle1 = writer.register_instance(instance1);

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        // Update content.
        instance0.value(count);
        instance1.value(count);

        // Write the two samples.
        std::cout << "writing instance0, value->" << instance0.value()
                  << std::endl;
        writer.write(instance0, handle0);

        std::cout << "writing instance1, value->" << instance1.value()
                  << std::endl;
        writer.write(instance1, handle1);

        rti::util::sleep(Duration(1));
    }

    // Unregister the samples.
    writer.unregister_instance(handle0);
    writer.unregister_instance(handle1);
}

int main(int argc, char *argv[])
{
    int domain_id    = 0;
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
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
