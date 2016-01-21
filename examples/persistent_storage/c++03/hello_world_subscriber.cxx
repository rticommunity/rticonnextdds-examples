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
#include <algorithm>
#include <iostream>

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "hello_world.hpp"

using namespace dds::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

class hello_worldReaderListener : public NoOpDataReaderListener<hello_world> {
  public:
    void on_data_available(DataReader<hello_world>& reader)
    {
        // Take all samples
        LoanedSamples<hello_world> samples = reader.take();

        for (LoanedSamples<hello_world>::iterator sample_it = samples.begin();
            sample_it != samples.end();
            sample_it++) {

            if (sample_it->info().valid()){
                std::cout << sample_it->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count, bool drs)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<hello_world> topic(participant, "Example hello_world");

    // If you use Durable Reader State, you need to set up several properties.
    // In this example, we have modified them using a QoS XML profile. See
    // further details in USER_QOS_PROFILES.xml.
    std::string qos_libname = "persistence_example_Library";
    DataReaderQos reader_qos = drs ?
        QosProvider::Default().datareader_qos(
            qos_libname + "::durable_reader_state_Profile") :
        QosProvider::Default().datareader_qos(
            qos_libname + "::persistence_service_Profile");

    // Create a DataReader (Subscriber created in-line)
    DataReader<hello_world> reader(Subscriber(participant), topic, reader_qos);

    // Create a data reader listener using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    rti::core::ListenerBinder< DataReader<hello_world> > scoped_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new hello_worldReaderListener,
            StatusMask::data_available());

    for (int count = 0; sample_count == 0 || count < sample_count; ++count) {
        std::cout << "hello_world subscriber sleeping for 4 sec..."
                  << std::endl;
        rti::util::sleep(Duration(4));
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop
    bool drs = false;

    for (int i = 1; i < argc; ) {
        const std::string& param = argv[i++];

        if (param == "-domain_id" && i < argc) {
            domain_id = atoi(argv[i++]);
        } else if (param == "-sample_count" && i < argc) {
            sample_count = atoi(argv[i++]);
        } else if (param == "-drs" && i < argc) {
            drs = (atoi(argv[i++]) == 1);
        } else {
            std::cout << argv[0] << " [options]" << std::endl
                      << "\t-domain_id <domain ID> (default: 0)" << std::endl
                      << "\t-sample_count <number of published samples> "
                      << "(default: 0 (infinite))" << std::endl
                      << "\t-drs <1|0> Enable/Disable durable reader state "
                      << "(default: 0)" << std::endl;
            return -1;
        }
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count, drs);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
