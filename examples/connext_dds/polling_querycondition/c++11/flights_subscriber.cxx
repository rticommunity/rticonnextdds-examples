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

#include "flights.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos.
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<Flight> topic(participant, "Example Flight");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    // reader_qos << Reliability::Reliable();

    // Create a subscriber
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader.
    dds::sub::DataReader<Flight> reader(subscriber, topic, reader_qos);

    // Query for company named 'CompanyA' and for flights in cruise
    // (about 30,000ft). The company parameter will be changed in run-time.
    // NOTE: There must be single-quotes in the query parameters around-any
    // strings! The single-quote do NOT go in the query condition itself.
    std::vector<std::string> query_parameters = { "'CompanyA'", "30000" };

    std::cout << "Setting parameters to company: " << query_parameters[0]
              << " and altitude bigger or equals to " << query_parameters[1]
              << std::endl;

    // Create the query condition with an expession to MATCH the id field in
    // the structure and a numeric comparison.
    dds::sub::cond::QueryCondition query_condition(
            dds::sub::Query(
                    reader,
                    "company MATCH %0 AND altitude >= %1",
                    query_parameters),
            dds::sub::status::DataState::any_data());

    // Main loop
    bool update = false;
    int samples_read = 0, count = 0;
    while (!application::shutdown_requested && samples_read < sample_count) {
        // Poll for new samples every second.
        rti::util::sleep(dds::core::Duration(1));

        // Change the filter parameter after 5 seconds.
        if ((count + 1) % 10 == 5) {
            query_parameters[0] = "'CompanyB'";
            update = true;
        } else if ((count + 1) % 10 == 0) {
            query_parameters[0] = "'CompanyA'";
            update = true;
        }

        // Set new parameters.
        if (update) {
            std::cout << "Changing parameter to " << query_parameters[0]
                      << std::endl;
            query_condition.parameters(
                    query_parameters.begin(),
                    query_parameters.end());
            update = false;
        }

        // Get new samples selecting them with a condition.
        dds::sub::LoanedSamples<Flight> samples =
                reader.select().condition(query_condition).read();

        for (const auto &sample : samples) {
            if (sample.info().valid()) {
                samples_read++;
                std::cout << "\t" << sample.data() << std::endl;
            }
        }

        count++;
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
