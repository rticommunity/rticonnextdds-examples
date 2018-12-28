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

#include <algorithm>
#include <iostream>

#include <dds/dds.hpp>
#include "poll.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<poll> topic(participant, "Example poll");

    // Create a DataReader with default Qos (Subscriber created in-line)
    DataReader<poll> reader(Subscriber(participant), topic);

    std::cout << std::fixed;
    for (int count = 0; sample_count == 0 || count < sample_count; ++count) {
        // Poll for new data every 5 seconds.
        rti::util::sleep(Duration(5));

        // Check for new data calling the DataReader's take() method.
        LoanedSamples<poll> samples = reader.take();

        // Iterate through the samples read using the 'take()' method and
        // adding the value of x on each of them to calculate the average
        // afterwards.
        double sum = 0;
        for (LoanedSamples<poll>::iterator sample_it = samples.begin();
            sample_it != samples.end();
            sample_it++) {

            if (sample_it->info().valid()){
                sum += sample_it->data().x();
            }
        }

        if (samples.length() > 0) {
            std::cout << "Got " << samples.length() << " samples.  "
                      << "Avg = " << sum / samples.length() << std::endl;
        }
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
