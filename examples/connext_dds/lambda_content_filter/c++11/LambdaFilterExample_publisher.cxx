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

#include <iostream>
#include <random>

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "LambdaFilterExample.hpp"
#include "LambdaFilter.hpp"
#include "application.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Register the same filter we use in the subscriber.
    //
    // This is *optional*, but recommended for better network usage. By
    // registering the filter on this participant, we allow the DataWriter to
    // filter the samples it writes before sending them.
    //
    // Note that the filter name and its definition need to be identical to the
    // one in the subscribing application.
    //
    create_lambda_filter<Stock>(
            "stock_cft",
            participant,
            [](const Stock &stock) {
                return stock.symbol() == "GOOG" || stock.symbol() == "IBM";
            });

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<Stock> topic(participant, "Example Stock");

    // Create a DataWriter with default Qos (Publisher created in-line)
    dds::pub::DataWriter<Stock> writer(dds::pub::Publisher(participant), topic);

    Stock sample;

    std::vector<std::string> symbols { "GOOG", "IBM", "RTI", "MSFT" };
    std::random_device random_device;
    std::uniform_int_distribution<int> distribution(0, symbols.size() - 1);

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify the data to be written here
        sample.symbol(symbols[distribution(random_device)]);
        sample.value(distribution(random_device) * 1.1);

        // Print the sample we're writting
        std::cout << "Writing " << sample << std::endl;
        writer.write(sample);

        // Print whether the sample was pushed on the network (to the
        // DataReader) or not (for example, because it was locally filtered out)
        auto pushed_samples = writer->datawriter_protocol_status()
                                      .pushed_sample_count()
                                      .change();
        std::cout << "Sample was "
                  << (pushed_samples != 0 ? "pushed" : "not pushed")
                  << std::endl;

        rti::util::sleep(dds::core::Duration(4));
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
        run_publisher_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
