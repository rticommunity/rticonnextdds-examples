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

#include "cfc.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

clock_t Init_time;

int process_data(dds::sub::DataReader<cfc> reader)
{
    int count = 0;
    // Take all samples
    dds::sub::LoanedSamples<cfc> samples = reader.take();

    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            count++;
            // Print the time we get each sample.
            double elapsed_ticks = clock() - Init_time;
            double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

            std::cout << "@ t=" << elapsed_secs
                      << "s, got x = " << sample.data().x() << std::endl;
        }
    }

    return count;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // For timekeeping.
    Init_time = clock();

    // Retrieve the default Participant QoS, from USER_QOS_PROFILES.xml
    dds::domain::qos::DomainParticipantQos participant_qos =
            dds::core::QosProvider::Default().participant_qos();

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    //
    // By default, data will be sent via shared memory "and" UDPv4.
    // Because the flowcontroller limits writes across all interfaces,
    // this halves the effective send rate. To avoid this, we enable only the
    // UDPv4 transport.

    // participant_qos << TransportBuiltin::UDPv4();

    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id, participant_qos);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<cfc> topic(participant, "Example cfc");

    // Create a subscriber
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader with default Qos (Subscriber created in-line)
    dds::sub::DataReader<cfc> reader(subscriber, topic);

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;

    // Create a ReadCondition for any data on this reader, and add to WaitSet
    unsigned int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::new_data(),
            [reader, &samples_read]() {
                samples_read += process_data(reader);
            });

    waitset += read_condition;

    std::cout << std::fixed;

    // Main loop
    while (!application::shutdown_requested && samples_read < sample_count) {
        waitset.dispatch(dds::core::Duration(1));
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
