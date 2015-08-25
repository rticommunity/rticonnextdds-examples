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
#include <rti/pub/FlowController.hpp>
#include "cfc.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::pub;
using namespace rti::pub;
using namespace dds::pub::qos;

const std::string flow_controller_name = "custom_flowcontroller";

FlowControllerProperty define_flowcontroller(DomainParticipant& participant)
{
    // Create a custom FlowController based on tocken-bucket mechanism.
    // First specify the token-bucket configuration.
    // In summary, each token can be used to send about one message,
    // and we get 2 tokens every 100 ms, so this limits transmissions to
    // about 20 messages per second.
    FlowControllerTokenBucketProperty flowcontroller_tokenbucket;

    // Don't allow too many tokens to accumulate.
    flowcontroller_tokenbucket.max_tokens(2);
    flowcontroller_tokenbucket.tokens_added_per_period(2);
    flowcontroller_tokenbucket.tokens_leaked_per_period(LENGTH_UNLIMITED);

    // Period of 100 ms.
    flowcontroller_tokenbucket.period(Duration::from_millisecs(100));

    // The sample size is 1000, but the minimum bytes_per_token is 1024.
    // Furthermore, we want to allow some overhead.
    flowcontroller_tokenbucket.bytes_per_token(1024);

    // Create a FlowController Property to set the TokenBucket definition.
    FlowControllerProperty flowcontroller_property(
        FlowControllerSchedulingPolicy::EARLIEST_DEADLINE_FIRST,
        flowcontroller_tokenbucket);

    return flowcontroller_property;
}

void publisher_main(int domain_id, int sample_count)
{
    // Retrieve the default Participant QoS, from USER_QOS_PROFILES.xml
    DomainParticipantQos participant_qos = QosProvider::Default()
        .participant_qos();

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    //
    // By default, data will be sent via shared memory "and" UDPv4.
    // Because the flowcontroller limits writes across all interfaces,
    // this halves the effective send rate. To avoid this, we enable only the
    // UDPv4 transport.

    // participant_qos << TransportBuiltin::UDPv4();

    // Create a DomainParticipant.
    DomainParticipant participant(domain_id, participant_qos);

    // Create the FlowController by code instead of from USER_QOS_PROFILES.xml.
    FlowControllerProperty flow_controller_property =
        define_flowcontroller(participant);
    FlowController flowcontroller(
        participant,
        flow_controller_name,
        flow_controller_property);

    // Create a Topic -- and automatically register the type
    Topic<cfc> topic(participant, "Example cfc");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer_qos << History::KeepAll()
    //            << PublishMode::Asynchronous(flow_controller_name);

    // Create a DataWriter (Publisher created in-line)
    DataWriter<cfc> writer(Publisher(participant), topic, writer_qos);

    // Create a sample to write with a long payload.
    cfc sample;
    sample.str(std::string(999, 'A'));

    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        // Simulate bursty writer.
        rti::util::sleep(Duration(1));

        for (int i = 0; i < 10; i++) {
            sample.x(count * 10 + i);

            std::cout << "Writing cfc, sample " << sample.x() << std::endl;
            writer.write(sample);
        }
    }

    // This new sleep is to give the subscriber time to read all the samples.
    rti::util::sleep(Duration(4));
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
