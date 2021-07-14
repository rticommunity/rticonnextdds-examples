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

#include "market_data.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(dds::sub::DataReader<market_data> reader)
{
    int count = 0;
    // Take all samples
    dds::sub::LoanedSamples<market_data> samples = reader.take();

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
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<market_data> topic(participant, "Example market_data");

    // Create a ContentFiltered Topic and specify the STRINGMATCH filter name
    // to use a built-in filter for matching multiple strings.
    // More information can be found in the example
    // 'content_filtered_topic_string_filter'.
    dds::topic::Filter filter("Symbol MATCH 'A'", std::vector<std::string>());
    filter->name(rti::topic::stringmatch_filter_name());
    std::cout << "filter is " << filter.expression() << std::endl;

    dds::topic::ContentFilteredTopic<market_data> cft_topic(
            topic,
            "ContentFilteredTopic",
            filter);

    // Create a subscriber with default QoS
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader with default Qos (Subscriber created in-line)
    dds::sub::DataReader<market_data> reader(subscriber, cft_topic);

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

    // Main loop.
    while (!application::shutdown_requested && samples_read < sample_count) {
        if (samples_read == 3) {
            // On t=3 we add the symbol 'D' to the filter parameter
            // to match 'A' and 'D'.
            cft_topic.extensions().append_to_expression_parameter(0, "D");
            std::cout << "changed filter to Symbol MATCH 'A,D'" << std::endl;
        } else if (samples_read == 6) {
            // On t=6 we remove the symbol 'A' to the filter parameter
            // to match only 'D'.
            cft_topic.extensions().remove_from_expression_parameter(0, "A");
            std::cout << "changed filter to Symbol MATCH 'D'" << std::endl;
        }

        waitset.dispatch(dds::core::Duration(4));
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
