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

#include <string>
#include <iostream>

#include "msg.hpp"
#include <dds/dds.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

class MsgListener :
    public NoOpDataReaderListener<msg> {
public:
    void on_data_available(DataReader<msg>& reader)
    {
        LoanedSamples<msg> samples = reader.take();
        for (LoanedSamples<msg>::iterator sampleIt = samples.begin();
            sampleIt != samples.end();
            ++sampleIt) {

            if (sampleIt->info().valid()) {
                std::cout << sampleIt->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count,
    std::string participant_auth, std::string reader_auth)
{
    // Set user_data qos field for participant
    DomainParticipantQos participant_qos;
    participant_qos << Discovery().multicast_receive_addresses(StringSeq(0));

    unsigned int max_user_data =
        participant_qos.policy<DomainParticipantResourceLimits>()
        .participant_user_data_max_length();
    if (participant_auth.size() > max_user_data) {
        std::cout << "error, participant user_data exceeds resource limits"
                  << std::endl;
    } else {
        participant_qos << UserData(
            ByteSeq(participant_auth.begin(), participant_auth.end()));
    }

    // To create participant with default QoS use the one-argument constructor
    DomainParticipant participant (domain_id, participant_qos);

    // Participant is disabled by default. We enable it now
    participant.enable();

    // To customize publisher QoS, use participant.default_subscriber_qos()
    Subscriber subscriber (participant);

    // To customize topic QoS, use participant.default_topic_qos()
    Topic<msg> topic (participant, "Example msg");

    // Create data reader listener
    MsgListener* reader_listener = new MsgListener();

    // Set user_data for field for datareader
    DataReaderQos reader_qos;

    if (reader_auth.size() > max_user_data) {
        std::cout << "error, datareader user_data exceeds resource limits"
                  << std::endl;
    } else {
        reader_qos << UserData(ByteSeq(reader_auth.begin(), reader_auth.end()));
    }

    // To create datareader with default QoS use the one-argument constructor
    DataReader<msg> reader (subscriber, topic, reader_qos, reader_listener);

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        // Each "sample_count" is one second.
        rti::util::sleep(Duration(1));
    }

    delete reader_listener;
}

int main(int argc, char* argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

    // Changes for Builtin_Topics
    // Get arguments for auth strings and pass to subscriber_main()
    std::string participant_auth = "password";
    std::string reader_auth = "Reader_Auth";

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    if (argc >= 4) {
        participant_auth = argv[3];
    }

    if (argc >= 5) {
        reader_auth = argv[4];
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count, participant_auth, reader_auth);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
