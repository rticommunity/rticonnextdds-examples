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
#include "tbf.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    Topic<tbf> topic (participant, "Example tbf");

    // Create a DataWriter with default Qos (Publisher created in-line)
    DataWriter<tbf> writer(Publisher(participant), topic);

    // Create a sample to write
    tbf sample;

    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        std::cout << "Writing tbf, count " << count << std::endl;

        // Update instance1 (code = 1) every 0.5 seconds => when count is even.
        if ((count + 1) % 2 == 0) {
            std::cout << "Publishing instance 1" << std::endl;
            sample.code(1);
            sample.x(count);
            writer.write(sample);
        }

        // Update instance 0 (code = 0) every 0.25 seconds.
        std::cout << "Publishing instance 0" << std::endl;
        sample.code(0);
        sample.x(count);
        writer.write(sample);

        // The loop to write new samples will sleep for 0.25 second.
        rti::util::sleep(Duration::from_millisecs(250));
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
