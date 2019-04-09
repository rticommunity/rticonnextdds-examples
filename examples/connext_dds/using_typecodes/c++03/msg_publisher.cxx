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
#include "msg.hpp"

using namespace dds::core;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::pub;

void publisher_main(int domain_id, int sample_count)
{
    // Retrieve the Participant QoS, from USER_QOS_PROFILES.xml
    DomainParticipantQos participant_qos = QosProvider::Default()->
        participant_qos();

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // participant_qos << DomainParticipantResourceLimits().
    //     type_code_max_serialized_length(3070);

    // Create a DomainParticipant.
    DomainParticipant participant(domain_id, participant_qos);

    // Create a Topic -- and automatically register the type
    Topic<msg> topic(participant, "Example msg");

    // Create a DataWriter with default Qos (Publisher created in-line)
    DataWriter<msg> writer(Publisher(participant), topic);

    msg sample;
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        // Update sample and send it.
        std::cout << "Writing msg, count " << count << std::endl;
        sample.count(count);
        writer.write(sample);

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
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
