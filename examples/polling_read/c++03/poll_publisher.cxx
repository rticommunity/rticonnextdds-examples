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
#include "poll.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    Topic<poll> topic (participant, "Example poll");

    // Create a DataWriter with default Qos (Publisher created in-line)
    DataWriter<poll> writer(Publisher(participant), topic);

    poll sample;
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        // Set x to a random number between 0 and 9.
        sample.x((int)(rand() / (RAND_MAX/10.0)));

        std::cout << "Writing poll, count " << count << ", x = " << sample.x()
                  << std::endl;
        writer.write(sample);

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
        publisher_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
