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
#include <rti/core/ListenerBinder.hpp>
#include "coherent.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;
using namespace dds::sub::status;
using namespace rti::core;
using namespace rti::sub;

// Transform a DDS sample into a pair value for a dictionary.
std::pair<char, int> sample2map(const coherent& data) {
    return std::make_pair(data.field(), data.value());
}

class CoherentListener : public NoOpDataReaderListener<coherent> {
public:
    void on_data_available(DataReader<coherent>& reader)
    {
        // Take all samples
        LoanedSamples<coherent> samples = reader.take();

        // Process samples and add to a dictionary
        std::map<char, int> values;
        std::transform(
            rti::sub::valid_samples(samples.begin()),
            rti::sub::valid_samples(samples.end()),
            std::inserter(values, values.begin()),
            sample2map);
        std::cout << std::endl;

        // Print result
        std::cout << "Received updates:" << std::endl;
        typedef std::map<char, int>::const_iterator MapIterator;
        for (MapIterator iter = values.begin(); iter != values.end(); iter++) {
            std::cout << " " << iter->first << " = " << iter->second << ";";
        }
        std::cout << std::endl;
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a participant with default QoS.
    DomainParticipant participant(domain_id);

    // Retrieve the default Subscriber QoS, from USER_QOS_PROFILES.xml
    SubscriberQos subscriber_qos = QosProvider::Default().subscriber_qos();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // subscriber_qos << Presentation::TopicAccessScope(true, false);

    // Create a subscriber.
    Subscriber subscriber(participant, subscriber_qos);

    // Create a Topic -- and automatically register the type.
    Topic<coherent> topic(participant, "Example coherent");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // reader_qos << Reliability::Reliable()
    //            << History::KeepLast(10);

    // Create a DataReader.
    DataReader<coherent> reader(subscriber, topic, reader_qos);

    // Create a DataReader listener using ListenerBinder, a RAII utility that
    // will take care of reseting it from the reader and deleting it.
    ListenerBinder< DataReader<coherent> > scoped_listener =
        bind_and_manage_listener(
            reader,
            new CoherentListener,
            StatusMask::data_available());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        rti::util::sleep(Duration(1));
    }
}

int main(int argc, char *argv[])
{
    int domain_id    = 0;
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
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
