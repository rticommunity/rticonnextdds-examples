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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "poll.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<poll> topic(participant, "Example poll");

    // Create a Subscriber with default QoS
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader with default Qos
    dds::sub::DataReader<poll> reader(subscriber, topic);

    std::cout << std::fixed;
    int samples_read = 0;
    while (!application::shutdown_requested && samples_read < sample_count) {
        // Poll for new data every 5 seconds.
        rti::util::sleep(dds::core::Duration(5));

        // Check for new data calling the DataReader's take() method.
        dds::sub::LoanedSamples<poll> samples = reader.take();

        // Iterate through the samples read using the 'take()' method and
        // adding the value of x on each of them to calculate the average
        // afterwards.
        double sum = 0;
        for (const auto &sample : samples) {
            if (sample.info().valid()) {
                samples_read++;
                sum += sample.data().x();
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
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
