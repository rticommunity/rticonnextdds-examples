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
#include <ctime>

#include "async.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

// For timekeeping
clock_t InitTime;

class AsyncListener : public NoOpDataReaderListener<async> {
public:
    void on_data_available(DataReader<async>& reader)
    {
        LoanedSamples<async> samples = reader.take();
        for (LoanedSamples<async>::iterator sampleIt = samples.begin();
                sampleIt != samples.end();
                ++sampleIt) {

            // Print the time we get each sample.
            if (sampleIt->info().valid()) {
                double elapsed_ticks = clock() - InitTime;
                double elapsed_secs  = elapsed_ticks / CLOCKS_PER_SEC;
                std::cout << "@ t=" << elapsed_secs << "s"
                          << ", got x = " << sampleIt->data().x() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // For timekeeping
    InitTime = clock();

    // To customize the paritcipant QoS, use the file USER_QOS_PROFILES.xml
    DomainParticipant participant (domain_id);

    // To customize the topic QoS, use the file USER_QOS_PROFILES.xml
    Topic<async> topic (participant, "Example async");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    // reader_qos << Reliability::Reliable(Duration(60));
    // reader_qos << History::KeepAll();

    // Create a DataReader with a QoS
    DataReader<async> reader (Subscriber(participant), topic, reader_qos);

    // Create a data reader listener using ListenerBinder, a RAII utility that
    // will take care of reseting it from the reader and deleting it.
    rti::core::ListenerBinder< DataReader<async> > scoped_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new AsyncListener,
            dds::core::status::StatusMask::data_available());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        std::cout << "async subscriber sleeping for 4 sec..." << std::endl;
        rti::util::sleep(Duration(4));
    }
}

int main(int argc, char* argv[])
{
    int domain_id = 0;
    int sample_count = 0;   // Infinite loop

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
