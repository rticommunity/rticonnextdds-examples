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
#include <rti/core/ListenerBinder.hpp>
#include "profiles.hpp"

using namespace dds::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

class ProfilesListener : public NoOpDataReaderListener<HelloWorld> {
public:
    void on_data_available(DataReader<HelloWorld>& reader)
    {
        // Take all samples
        LoanedSamples<HelloWorld> samples = reader.take();

        for (LoanedSamples<HelloWorld>::iterator sample_it = samples.begin();
            sample_it != samples.end(); sample_it++) {

            if (sample_it->info().valid()) {
                std::cout << sample_it->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
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

    // Retrieve the DataReader QoS from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // This example uses a built-in QoS profile to tune QoS for reliable
    // streaming data. To enable it programmatically uncomment these lines.

    // reader_qos = QosProvider::Default().datareader_qos(
    //     "BuiltinQosLibExp::Pattern.ReliableStreaming");

    // Create a DataReader.
    DataReader<HelloWorld> reader(Subscriber(participant), topic, reader_qos);

    // Create a DataReader listener using ListenerBinder, a RAII utility that
    // will take care of reseting it from the reader and deleting it.
    rti::core::ListenerBinder< DataReader<HelloWorld> > scoped_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new ProfilesListener,
            StatusMask::data_available());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        std::cout << "profiles subscriber sleeping for 4 sec..." << std::endl;
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
        subscriber_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
