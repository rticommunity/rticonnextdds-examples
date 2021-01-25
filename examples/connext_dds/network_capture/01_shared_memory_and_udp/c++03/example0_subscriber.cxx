/*******************************************************************************
 (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <string>

#include "example0.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace rti::util;

class Example0ReaderListener : public NoOpDataReaderListener<example0> {
public:
    void on_data_available(DataReader<example0> &reader)
    {
        LoanedSamples<example0> samples = reader.take();
        for (LoanedSamples<example0>::iterator sample_it = samples.begin();
             sample_it != samples.end();
             sample_it++) {
            if (sample_it->info().valid()) {
                std::cout << sample_it->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Start capturing traffic for all participants.
    //
    // All participants: those already created and those yet to be created.
    // Default parameters: all transports and some other sane defaults.
    //
    // A capture file will be created for each participant. The capture file
    // will start with the prefix "publisher" and continue with a suffix
    // dependent on the participant's GUID.
    if (!network_capture::start("subscriber")) {
        std::cerr << "Error starting network capture" << std::endl;
    }

    DomainParticipant participant(domain_id);

    Topic<example0> topic(participant, "Example0 using network capture C++03 API");

    Subscriber subscriber(
            participant,
            QosProvider::Default().subscriber_qos());

    DataReader<example0> reader(
            subscriber,
            topic,
            QosProvider::Default().datareader_qos());

    rti::core::ListenerBinder<DataReader<example0>> scoped_listener =
            rti::core::bind_and_manage_listener(
                    reader,
                    new Example0ReaderListener,
                    StatusMask::data_available());

    for (int count = 0; count < sample_count || sample_count == 0; ++count) {
        std::cout << "example0 subscriber sleeping for 4 sec...\n";
        rti::util::sleep(Duration(1));
    }

    // Before deleting the participants that are capturing, we must stop
    // network capture for them.
    if (!network_capture::stop()) {
        std::cerr << "Error stopping network capture" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0;  // Infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // Enable network capture.
    //
    // This must be called before:
    //   - Any other network capture function is called.
    //   - Creating the participants for which we want to capture traffic.
    if (!network_capture::enable()) {
        std::cerr << "Error enabling network capture" << std::endl;
    }

    try {
        subscriber_main(domain_id, sample_count);
    } catch (const std::exception &ex) {
        std::cerr << "Exception in subscriber_main: " << ex.what() << "\n";
        return -1;
    }

    // Disable network capture.
    //
    // This must be:
    //   - The last network capture function that is called.
    if (!network_capture::disable()) {
        std::cerr << "Error disabling network capture" << std::endl;
    }

    return 0;
}
