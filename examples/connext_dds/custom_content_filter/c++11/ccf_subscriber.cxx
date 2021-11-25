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

#include "ccf.hpp"
#include "filter.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(dds::sub::DataReader<Foo> reader)
{
    int count = 0;
    // Take the available data
    dds::sub::LoanedSamples<Foo> samples = reader.take();

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
        unsigned int sample_count)
{
    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<Foo> topic(participant, "Example ccf");

    // Register the custom filter type. It must be registered in both sides.
    participant->register_contentfilter(
            rti::topic::CustomFilter<CustomFilterType>(new CustomFilterType()),
            "CustomFilter");

    // Create the filter with the expression and the type registered.
    dds::topic::Filter filter("%0 %1 x", { "2", "divides" });
    filter->name("CustomFilter");

    // Create the content filtered topic.
    dds::topic::ContentFilteredTopic<Foo> cft_topic(
            topic,
            "ContentFilteredTopic",
            filter);

    std::cout << "Filter: 2 divides x" << std::endl;

    // Create a DataReader. Note that we are using the content filtered topic.
    dds::sub::Subscriber subcriber(participant);
    dds::sub::DataReader<Foo> reader(subcriber, cft_topic);

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
        if (samples_read == 4 && !filter_changed1) {
            std::cout << "Changing filter parameters" << std::endl
                      << "Filter: 15 greater-than x" << std::endl;
            std::vector<std::string> parameters = { "15", "greater-than" };
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
            filter_changed1 = true;
        } else if (samples_read == 10 && !filter_changed2) {
            std::cout << "Changing filter parameters" << std::endl
                      << "Filter: 3 divides x" << std::endl;
            std::vector<std::string> parameters = { "3", "divides" };
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
            filter_changed2 = true;
        }

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
