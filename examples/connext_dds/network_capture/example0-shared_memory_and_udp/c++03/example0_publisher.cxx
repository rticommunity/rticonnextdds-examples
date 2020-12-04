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

#include "example0.hpp"
#include <dds/dds.hpp>

using namespace dds::core;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::pub;
using namespace rti::util;

void publisher_main(int domain_id, int sample_count)
{
    // Start capturing traffic for all participants.
    //
    // All participants: those already created and those yet to be created.
    // Default parameters: all transports and some other sane defaults.
    //
    // A capture file will be created for each participant. The capture file
    // will start with the prefix "publisher" and continue with a suffix
    // dependent on the participant's GUID.
    if (!network_capture::start("publisher")) {
        std::cout << "Error starting network capture" << std::endl;
    }

    DomainParticipant participant(domain_id);

    Topic<example0> topic(participant, "Example0 using network capture C++03 API");

    Publisher publisher(
            participant,
            QosProvider::Default().publisher_qos());

    DataWriter<example0> writer(
            publisher,
            topic,
            QosProvider::Default().datawriter_qos());

    example0 sample;
    InstanceHandle instance_handle = InstanceHandle::nil();
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        sample.msg("Hello World " + std::to_string(count));
        std::cout << "Writing sample, count " << count << std::endl;

        // Here we are going to pause capturing for some samples.
        // The resulting pcap file will not contain them.
        if (count == 4 && !network_capture::pause()) {
            std::cout << "Error pausing network capture";
        } else if (count == 6 && !network_capture::resume()) {
            std::cout << "Error resuming network capture";
        }
        writer.write(sample);

        rti::util::sleep(Duration(1));
    }

    // Before deleting the participants that are capturing, we must stop
    // network capture for them.
    if (!network_capture::stop()) {
        std::cout << "Error stopping network capture" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0;  // infinite loop

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
        std::cout << "Error enabling network capture" << std::endl;
    }

    try {
        publisher_main(domain_id, sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    // Disable network capture.
    //
    // This must be:
    //   - The last network capture function that is called.
    if (!network_capture::disable()) {
        std::cout << "Error disabling network capture" << std::endl;
    }

    return 0;
}
