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
#include "waitsets.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default QoS.
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<waitsets> topic(participant, "Example waitsets");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter QoS programmatically rather
    // than using the XML file, you will need to uncomment these lines.

    // writer_qos << Reliability::Reliable()
    //            << History::KeepAll()
    //            << Liveliness::Automatic().lease_duration(Duration(1));

    // Create a DataWriter.
    DataWriter<waitsets> writer(Publisher(participant), topic, writer_qos);

    // Create a data sample for writing.
    waitsets instance;

    // Main loop.
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        std::cout << "Writing waitsets, count " << count << std::endl;

        // Modify sample and send the sample.
        instance.x(count);
        writer.write(instance);

        // Send a new sample every second.
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
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
