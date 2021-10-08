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

#include "coherent.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(dds::sub::DataReader<coherent> reader)
{
    int count = 0;
    // Take all samples
    dds::sub::LoanedSamples<coherent> samples = reader.take();

    // Process samples and add to a dictionary
    std::map<char, int> values;
    std::transform(
            rti::sub::valid_samples(samples.begin()),
            rti::sub::valid_samples(samples.end()),
            std::inserter(values, values.begin()),
            [](const coherent &data) {
                return std::make_pair(data.field(), data.value());
            });
    std::cout << std::endl;

    // Print result
    std::cout << "Received updates:" << std::endl;
    for (const auto &value : values) {
        count++;
        std::cout << " " << value.first << " = " << value.second << ";";
    }
    std::cout << std::endl;

    return count;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a participant with default QoS.
    dds::domain::DomainParticipant participant(domain_id);

    // Retrieve the default Subscriber QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::SubscriberQos subscriber_qos =
            dds::core::QosProvider::Default().subscriber_qos();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // subscriber_qos << Presentation::TopicAccessScope(true, false);

    // Create a subscriber.
    dds::sub::Subscriber subscriber(participant, subscriber_qos);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<coherent> topic(participant, "Example coherent");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // reader_qos << Reliability::Reliable()
    //            << History::KeepLast(10);

    // Create a DataReader.
    dds::sub::DataReader<coherent> reader(subscriber, topic, reader_qos);

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;

    // Create a ReadCondition for any data on this reader, and add to WaitSet
    unsigned int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [reader, &samples_read]() {
                samples_read += process_data(reader);
            });

    waitset += read_condition;

    // Main loop
    while (!application::shutdown_requested && samples_read < sample_count) {
        std::cout << "async subscriber sleeping up to 1 sec..." << std::endl;

        // Wait for data and report if it does not arrive in 1 second
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
