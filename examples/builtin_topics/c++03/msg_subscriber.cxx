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

#include <dds/sub/ddssub.hpp>
#include "msg.hpp"

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
        for (auto sampleIt = samples.begin();
            sampleIt != samples.end();
            ++sampleIt) {

            if (sampleIt->info().valid()) {
                std::cout << sampleIt->data() << std::endl;
            }
        }
    }
};

void subscriberMain(int domainId, int sampleCount,
    std::string participantAuth, std::string readerAuth)
{
    // Set user_data qos field for participant
    DomainParticipantQos participantQos;
    participantQos << Discovery().multicast_receive_addresses(StringSeq(0));

    unsigned int maxUserData =
        participantQos.policy<DomainParticipantResourceLimits>()
        .participant_user_data_max_length();
    if (participantAuth.size() > maxUserData) {
        std::cout << "error, participant user_data exceeds resource limits"
                  << std::endl;
    } else {
        participantQos << UserData(
            ByteSeq(participantAuth.begin(), participantAuth.end()));
    }

    // To create participant with default QoS use the one-argument constructor
    DomainParticipant participant (domainId, participantQos);

    // Participant is disabled by default. We enable it now
    participant.enable();

    // To customize publisher QoS, use participant.default_subscriber_qos()
    Subscriber subscriber (participant);

    // To customize topic QoS, use participant.default_topic_qos()
    Topic<msg> topic (participant, "Example msg");

    // Create data reader listener
    MsgListener* readerListener = new MsgListener();

    // Set user_data for field for datareader
    DataReaderQos readerQos;

    if (readerAuth.size() > maxUserData) {
        std::cout << "error, datareader user_data exceeds resource limits"
                  << std::endl;
    } else {
        readerQos << UserData(ByteSeq(readerAuth.begin(), readerAuth.end()));
    }

    // To create datareader with default QoS use the one-argument constructor
    DataReader<msg> reader (subscriber, topic, readerQos, readerListener);

    // Main loop
    for (int count = 0; (sampleCount == 0) || (count < sampleCount); ++count) {
        // Each "sampleCount" is one second.
        rti::util::sleep(Duration(1));
    }

    delete readerListener;
}

void main(int argc, char* argv[])
{
    int domainId = 0;
    int sampleCount = 0; /* infinite loop */

    // Changes for Builtin_Topics
    // Get arguments for auth strings and pass to subscriberMain()
    std::string participantAuth = "password";
    std::string readerAuth = "Reader_Auth";

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }

    if (argc >= 3) {
        sampleCount = atoi(argv[2]);
    }

    if (argc >= 4) {
        participantAuth = argv[3];
    }

    if (argc >= 5) {
        readerAuth = argv[4];
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriberMain(domainId, sampleCount, participantAuth, readerAuth);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
    }
}
