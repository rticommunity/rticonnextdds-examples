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
#include "tbf.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

class tbfReaderListener : public NoOpDataReaderListener<tbf> {
  public:
    void on_data_available(dds::sub::DataReader<tbf>& reader)
    {
        // Take all samples
        LoanedSamples<tbf> samples = reader.take();

        for (LoanedSamples<tbf>::iterator sample_it = samples.begin();
        sample_it != samples.end(); sample_it++) {

            if (sample_it->info().valid()){
                // Here we get source timestamp of the sample using the sample
                // info. 'info.source_timestamp()' returns dds::core::Time.
                double source_timestamp = sample_it->info()
                    .source_timestamp().to_secs();

                const tbf& data = sample_it->data();
                std::cout << source_timestamp << "\t" << data.code()
                          << "\t\t" << data.x() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<tbf> topic(participant, "Example tbf");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather
    // than using the XML file, you will need uncomment this line.

    // reader_qos << TimeBasedFilter(Duration::from_secs(2));

    // Create a DataReader with default Qos (Subscriber created in-line)
    DataReader<tbf> reader(Subscriber(participant), topic, reader_qos);

    // Associate a listener to the DataReader using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    ListenerBinder< DataReader<tbf> > reader_listener =
        rti::core::bind_and_manage_listener(
                reader,
                new tbfReaderListener,
                dds::core::status::StatusMask::all());

    std::cout << "================================================" << std::endl
              << "Source Timestamp\tInstance\tX"                    << std::endl
              << "================================================" << std::endl
              << std::fixed;
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        rti::util::sleep(Duration(1));
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
        std::cerr << "Exception in subscriber_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
