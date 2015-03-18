/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/
#include <iostream>
#include <dds/pub/ddspub.hpp>

using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::core;

// By default StringTopicType can manage strings up to 1K
const int MAX_STRING_SIZE = 1024;

// Domain id
const int DOMAIN_ID = 0;

int main() {
    // --- Set Up --------------------------------------------------------- //

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    // Create a DomainParticipant with the default QoS.
    DomainParticipant participant (DOMAIN_ID);

    // Create a Topic and automatically register the type.
    Topic<StringTopicType> topic (participant, "HelloSimple");

    // Create a DataWriter with default QoS.
    // The publisher is created in-line.
    DataWriter<StringTopicType> writer (Publisher(participant), topic);

    // --- Write Data ----------------------------------------------------- //

    std::cout << "Ready to write data." << std::endl;
    std::cout << "When the subscriber is ready, you can start writing." << std::endl;
    std::cout << "Press CTRL+C to terminate or enter an empty line to do a clean shutdown."
              << std::endl
              << std::endl;

    // RTI provides APIs for detecting when data readers and data writers
    // join the network. You can also configure data durability so that data
    // readers can receive data that were written before they started.
    // However, for the sake of keeping this example as simple as possible,
    // it asks you to wait for both sides to start before continuing.
    char sample[MAX_STRING_SIZE];
    while (true) {
        std::cout << "Please type a message> ";
        if (!cin.getline(sample, MAX_STRING_SIZE-1)) {
            break;
        }

        writer.write(sample);

        if (strlen(sample) == 0) {
            break;
        }
    }

    std::cout << "Shutting down" << std::endl;
    return 0;
}
