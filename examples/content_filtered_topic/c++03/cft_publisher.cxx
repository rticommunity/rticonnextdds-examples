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

#include "cft.hpp"
#include <dds/dds.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // To customize any of the entities QoS, use
    // the configuration file USER_QOS_PROFILES.xml

    DomainParticipant participant (domain_id);

    Topic<cft> topic (participant, "Example cft");

    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to set the reliability and history QoS settings
    // programmatically rather than using the XML, you will need to comment out
    // the following lines of code.
    // writer_qos << Reliability::Reliable()
    //            << Durability::TransientLocal()
    //            << History::KeepLast(20);

    DataWriter<cft> writer (Publisher(participant), topic, writer_qos);

    // Create data sample for writing
    cft instance;

    // For a data type that has a key, if the same instance is going to be
    // written multiple times, initialize the key here
    // and register the keyed instance prior to writing.

    InstanceHandle instance_handle = InstanceHandle::nil();
    // instance_handle = writer.register_instance(instance);

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        // Modify the data to be sent here

        // Our purpose is to increment x every time we send a sample and to
        // reset the x counter to 0 every time we send 10 samples (x=0,1,..,9).
        // Using the value of count, we can get set x to the appropriate value
        // applying % 10 operation to it.
        instance.count(count);
        instance.x(count % 10);

        std::cout << "Writing cft, count " << instance.count() << "\t"
                  << "x=" << instance.x()  << std::endl;

        writer.write(instance, instance_handle);

        // Send a new sample every second
        rti::util::sleep(Duration(1));
    }

    // writer.unregister_instance(instance_handle);
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

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
        std::cerr << "Exception in publisher_main(): " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
