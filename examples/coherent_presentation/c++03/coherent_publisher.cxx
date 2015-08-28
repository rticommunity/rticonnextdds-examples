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

#include <cstdlib>
#include <iostream>

#include <dds/dds.hpp>
#include "coherent.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    Topic<coherent> topic (participant, "Example coherent");

    // Retrieve the Publisher QoS, from USER_QOS_PROFILES.xml.
    PublisherQos publisher_qos = QosProvider::Default().publisher_qos();

    // If you want to change the Publisher's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following lines.

    // publisher_qos << Presentation::TopicAccessScope(true, false);

    Publisher publisher(participant, publisher_qos);

    // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml.
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following lines.

    // writer_qos << Reliability::Reliable()
    //            << History::KeepLast(10)
    //            << DataWriterProtocol().rtps_reliable_writer(
    //                 RtpsReliableWriterProtocol().heartbeats_per_max_samples(0));

    // Create a DataWriter with default Qos
    DataWriter<coherent> writer(publisher, topic, writer_qos);

    coherent sample;
    sample.id(0);
    InstanceHandle handle = writer.register_instance(sample);

    int num_samples = 7;
    for (int count = 0; count < sample_count || sample_count == 0; ) {

        CoherentSet coherent_set(publisher);
        std::cout << "Begin Coherent Changes" << std::endl;

        for (int i = 0; i < num_samples; i++, count++) {
            rti::util::sleep(Duration(1));

            sample.field('a' + i);
            sample.value((int)(rand() / (RAND_MAX / 10.0)));
            std::cout << "\tUpdating instance, " << sample.field() << "->"
                      << sample.value() << std::endl;
            writer.write(sample, handle);
        }

        // `coherent_set.end()` will be called by the destructor.
        std::cout << "End Coherent Changes" << std::endl << std::endl;
    }

    writer.unregister_instance(handle);
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
        std::cerr << "Exception in publisher_main(): " << ex.what()
                  << std::endl;
        return -1;
    }

    return 0;
}
