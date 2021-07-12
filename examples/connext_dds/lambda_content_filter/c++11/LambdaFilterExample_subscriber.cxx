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

#include <dds/core/ddscore.hpp>
#include <dds/sub/ddssub.hpp>
// Or simply include <dds/dds.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "LambdaFilterExample.hpp"
#include "LambdaFilter.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    dds::domain::DomainParticipant participant(domain_id);

    dds::topic::Topic<Stock> topic(participant, "Example Stock");

    // Create a ContentFilteredTopic that only selects those Stocks whose
    // symbol is GOOG or IBM.
    auto lambda_cft = create_lambda_cft<Stock>(
            "stock_cft",
            topic,
            [](const Stock &stock) {
                return stock.symbol() == "GOOG" || stock.symbol() == "IBM";
            });

    dds::sub::DataReader<Stock> reader(
            dds::sub::Subscriber(participant),
            lambda_cft);

    // Create a ReadCondition for any data on this reader and associate a
    // handler
    int count = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [&reader, &count]() {
                // Take all samples
                dds::sub::LoanedSamples<Stock> samples = reader.take();
                for (auto sample : samples) {
                    if (sample.info().valid()) {
                        count++;
                        std::cout << sample.data() << std::endl;
                    }
                }
            }  // The LoanedSamples destructor returns the loan
    );

    // Create a WaitSet and attach the ReadCondition
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    while (!application::shutdown_requested && count < sample_count) {
        // Dispatch will call the handlers associated to the WaitSet conditions
        // when they activate
        std::cout << "Stock subscriber sleeping for 4 sec...\n";
        waitset.dispatch(dds::core::Duration(4));  // Wait up to 4s each time
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
