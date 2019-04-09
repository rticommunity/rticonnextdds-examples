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
#include "profiles.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Retrieve the Participant QoS from USER_QOS_PROFILES.xml
    DomainParticipantQos participant_qos = QosProvider::Default()
        .participant_qos();

    // This example uses a built-in QoS profile to enable monitoring on the
    // DomainParticipant. This profile is specified in USER_QOS_PROFILES.xml.
    // To enable it programmatically uncomment these lines.

    // participant_qos = QosProvider::Default().participant_qos(
    //     "BuiltinQosLib::Generic.Monitoring.Common");

    // Create a DomainParticipant.
    DomainParticipant participant(domain_id, participant_qos);

    // Create a Topic -- and automatically register the type.
    Topic<HelloWorld> topic(participant, "Example profiles");

    // Retrieve the DataWriter QoS from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // This example uses a built-in QoS profile to tune QoS for reliable
    // streaming data. To enable it programmatically uncomment these lines.

    // writer_qos = QosProvider::Default().datawriter_qos(
    //     "BuiltinQosLibExp::Pattern.ReliableStreaming");

    // Create a Datawriter.
    DataWriter<HelloWorld> writer(Publisher(participant), topic, writer_qos);

    // Create a data sample for writing.
    HelloWorld instance;

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        std::cout << "Writing profiles, count " << count << std::endl;

        instance.msg("Hello World!");
        writer.write(instance);
        rti::util::sleep(Duration(4));
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
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
