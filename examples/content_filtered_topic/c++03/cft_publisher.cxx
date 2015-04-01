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
#include <string>
#include <dds/pub/ddspub.hpp>
#include "cft.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::pub;
using namespace dds::pub::qos;
using namespace dds::topic;

void publisherMain(int domainId, int sampleCount)
{
    // To customize any of the entities QoS, use
    // the configuration file USER_QOS_PROFILES.xml

    DomainParticipant participant (domainId);

    Topic<cft> topic (participant, "Example cft");

    DataWriter<cft> writer (Publisher(participant), topic);

    // If you want to set the reliability and history QoS settings
    // programmatically rather than using the XML, you will need to add
    // the following lines to your code and comment out the datawriter
    // constructor call above.

    /*
    DataWriterQos datawriter_qos;
    datawriter_qos << Reliability::Reliable();
    datawriter_qos << Durability::TransientLocal();
    datawriter_qos << History::KeepLast(20);
    DataWriter<cft> writer (Publisher(participant), topic, datawriter_qos);
    */

    // Create data sample for writing
    cft instance;

    // For a data type that has a key, if the same instance is going to be
    // written multiple times, initialize the key here
    // and register the keyed instance prior to writing.
    
    InstanceHandle instance_handle = InstanceHandle::nil();
    //instance_handle = writer.register_instance(instance);

    // Main loop
    for (int count = 0; (sampleCount == 0) || (count < sampleCount); ++count) {
        // Modify the data to be sent here

        // Our purpose is to increment x every time we send a sample and to
        // reset the x counter to 0 every time we send 10 samples (x=0,1,..,9).
        // Using the value of count, we can get set x to the appropriate value
        // applying % 10 operation to it.
        instance.count(count);
        instance.x(count % 10);

        std::cout << "Writing cft, count " << instance.count() << "\t"
                  << "x=" << instance.x()  << std::endl;

        writer->write(instance, instance_handle);

        // Send a new sample every second
        rti::util::sleep(Duration(1));
    }

    //writer.unregister_instance(instance_handle);
}

void main(int argc, char *argv[])
{
    int domainId = 0;
    int sampleCount = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }

    if (argc >= 3) {
        sampleCount = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    publisherMain(domainId, sampleCount);
}
