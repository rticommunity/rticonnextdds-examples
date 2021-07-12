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

#include "cft.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(dds::sub::DataReader<cft> reader)
{
    int count = 0;
    // Take the available data
    dds::sub::LoanedSamples<cft> samples = reader.take();

    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            count++;
            std::cout << sample.data() << std::endl;
        }
    }

    return count;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count,
        bool is_cft)
{
    // To customize any of the entities QoS, use
    // the configuration file USER_QOS_PROFILES.xml

    dds::domain::DomainParticipant participant(domain_id);

    dds::topic::Topic<cft> topic(participant, "Example cft");

    if (is_cft) {
        std::cout << std::endl
                  << "==========================" << std::endl
                  << "Using CFT" << std::endl
                  << "Filter: 1 <= x <= 4" << std::endl
                  << "==========================" << std::endl;
    }

    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to set the reliability and history QoS settings
    // programmatically rather than using the XML, you will need to comment out
    // the following lines of code.
    // reader_qos << Reliability::Reliable()
    //            << Durability::TransientLocal()
    //            << History::KeepLast(20);

    // Create the content filtered topic in the case sel_cft is true
    // The Content Filter Expresion has two parameters:
    // - %0 -- x must be greater or equal than %0.
    // - %1 -- x must be less or equal than %1.
    dds::topic::ContentFilteredTopic<cft> cft_topic = dds::core::null;
    dds::sub::DataReader<cft> reader = dds::core::null;
    dds::sub::Subscriber subscriber(participant);

    if (is_cft) {
        std::cout << "Using ContentFiltered Topic" << std::endl;
        cft_topic = dds::topic::ContentFilteredTopic<cft>(
                topic,
                "ContentFilteredTopic",
                dds::topic::Filter("x >= %0 AND x <= %1", { "1", "4" }));
        reader = dds::sub::DataReader<cft>(subscriber, cft_topic, reader_qos);
    } else {
        std::cout << "Using Normal Topic" << std::endl;
        reader = dds::sub::DataReader<cft>(subscriber, topic, reader_qos);
    }

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

    bool filter_changed1 = false;
    bool filter_changed2 = false;

    // Main loop
    while (!application::shutdown_requested && samples_read < sample_count) {
        // Wait for data and report if it does not arrive in 1 second
        waitset.dispatch(dds::core::Duration(1));

        // If we are not using CFT, we do not need to change the filter
        // parameters
        if (!is_cft) {
            continue;
        }

        if (samples_read == 10 && !filter_changed1) {
            std::cout << std::endl
                      << "==========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Filter: 5 <= x <= 9" << std::endl
                      << "==========================" << std::endl;

            std::vector<std::string> parameters { "5", "9" };
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
            filter_changed1 = true;
        } else if (samples_read == 20 && !filter_changed2) {
            std::cout << std::endl
                      << "==========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Filter: 3 <= x <= 9" << std::endl
                      << "==========================" << std::endl;

            std::vector<std::string> parameters { "3", "9" };
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
            filter_changed2 = true;
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
        run_subscriber_application(
                arguments.domain_id,
                arguments.sample_count,
                arguments.use_cft);
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
