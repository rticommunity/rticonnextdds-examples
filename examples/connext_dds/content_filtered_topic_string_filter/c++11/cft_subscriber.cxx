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
    // Take all samples
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
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<cft> topic(participant, "Example cft");

    // Define the default parameter of the filter.
    std::vector<std::string> parameters = { "SOME_STRING" };

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // reader_qos << Reliability::Reliable()
    //            << Durability::TransientLocal()
    //            << History::KeepLast(20);

    // Create the ContentFilteredTopic and DataReader.
    dds::topic::ContentFilteredTopic<cft> cft_topic = dds::core::null;
    dds::sub::DataReader<cft> reader = dds::core::null;
    dds::sub::Subscriber subscriber(participant);

    if (is_cft) {
        std::cout << "Using ContentFiltered Topic" << std::endl;
        dds::topic::Filter filter("name MATCH %0", parameters);

        // If there is no filter name, the regular SQL filter will be used.
        filter->name(rti::topic::stringmatch_filter_name());

        cft_topic = dds::topic::ContentFilteredTopic<cft>(
                topic,
                "ContentFilteredTopic",
                filter);

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
            dds::sub::status::DataState::new_data(),
            [reader, &samples_read]() {
                samples_read += process_data(reader);
            });

    waitset += read_condition;

    // Change the filter
    if (is_cft) {
        std::cout << "Now setting a new filter: 'name MATCH \"EVEN\"'"
                  << std::endl;
        cft_topic->append_to_expression_parameter(0, "EVEN");
    }

    bool filter_changed1 = false;
    bool filter_changed2 = false;

    // Main loop
    while (!application::shutdown_requested && samples_read < sample_count) {
        // Wait for data and report if it does not arrive in 1 second
        waitset.dispatch(dds::core::Duration(1));

        if (!is_cft) {
            continue;
        }

        if (samples_read == 10 && !filter_changed1) {
            std::cout << std::endl
                      << "===========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Append 'ODD' filter" << std::endl
                      << "===========================" << std::endl;
            cft_topic->append_to_expression_parameter(0, "ODD");
            filter_changed1 = true;
        } else if (samples_read == 20 && !filter_changed2) {
            std::cout << std::endl
                      << "===========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Removing 'EVEN' filter" << std::endl
                      << "===========================" << std::endl;
            cft_topic->remove_from_expression_parameter(0, "EVEN");
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
