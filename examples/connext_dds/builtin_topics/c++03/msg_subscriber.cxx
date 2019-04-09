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
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core;
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
    std::string participant_auth)
{
    // Retrieve the default participant QoS, from USER_QOS_PROFILES.xml
    DomainParticipantQos participant_qos = QosProvider::Default()
        .participant_qos();
    DomainParticipantResourceLimits resource_limits_qos =
        participant_qos.policy<DomainParticipantResourceLimits>();

    // If you want to change the Participant's QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // resource_limits_qos.participant_user_data_max_length(1024);
    // participant_qos << resource_limits_qos;

    unsigned int max_participant_user_data = resource_limits_qos
        .participant_user_data_max_length();
    if (participant_auth.size() > max_participant_user_data) {
        std::cout << "error, participant user_data exceeds resource limits"
                  << std::endl;
    } else {
        participant_qos << UserData(
            ByteSeq(participant_auth.begin(), participant_auth.end()));
    }

    // Create a DomainParticipant.
    DomainParticipant participant(domain_id, participant_qos);

    // Participant is disabled by default in USER_QOS_PROFILES. We enable it now
    participant.enable();

    // Create a Topic -- and automatically register the type.
    Topic<msg> topic(participant, "Example msg");

    // Create a DataReader
    DataReader<msg> reader(Subscriber(participant), topic);

    // Create a data reader listener using ListenerBinder, a RAII utility that
    // will take care of reseting it from the reader and deleting it.
    ListenerBinder<DataReader<msg> > scoped_listener = bind_and_manage_listener(
        reader,
        new MsgListener,
        dds::core::status::StatusMask::data_available());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        // Each "sample_count" is one second.
        rti::util::sleep(Duration(1));
    }
}

int main(int argc, char* argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop

    // Changes for Builtin_Topics
    // Get arguments for auth strings and pass to subscriber_main()
    std::string participant_auth = "password";

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    if (argc >= 4) {
        participant_auth = argv[3];
    }


    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count, participant_auth);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
