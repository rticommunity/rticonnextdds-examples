/*
 * (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

#include <algorithm>
#include <iostream>
#include <vector>

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging
#include "ShapeType.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(
        dds::sub::DataReader<ShapeType> reader,
        const std::string &reader_name)
{
    // Take all samples
    int count = 0;
    dds::sub::LoanedSamples<ShapeType> samples = reader.take();
    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            count++;
            std::cout << reader_name << ": " << sample.data() << std::endl;
        }
    }

    return count;
}  // The LoanedSamples destructor returns the loan

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    rti::domain::register_type<ShapeType>("ShapeType");

    // IMPORTANT: If you plan to use a domain_id different than 0, remember
    // to change the value of the domain_id attribute of the domain tag in
    // the USER_QOS_PROFILES.xml
    rti::domain::DomainParticipantConfigParams params(domain_id);

    auto default_provider = dds::core::QosProvider::Default();

    // Create the participant and all its entities from the XML file
    // (USER_QOS_PROFILES.xml)
    dds::domain::DomainParticipant participant =
            default_provider.extensions().create_participant_from_config(
                    "ShapeType_DomainParticipantLibrary::"
                    "ShapeTypeSubscriberParticipant",
                    params);

    const unsigned int reader_count = 4;
    const std::string reader_names[] = { "Sub::RED_DR",
                                         "Sub::RED_DR#1",
                                         "Sub::BLUE_DR",
                                         "Sub::BLUE_DR#1" };
    std::vector<dds::sub::DataReader<ShapeType>> readers;
    std::vector<dds::core::cond::WaitSet> waitsets(reader_count);
    std::vector<int> samples_read(reader_count);

    // Retrieve the four DataReaders and configure their
    // ReadConditions and Waitsets
    for (unsigned int i = 0; i < reader_count; i++) {
        auto reader = rti::sub::find_datareader_by_name<
                dds::sub::DataReader<ShapeType>>(participant, reader_names[i]);
        if (reader == dds::core::null) {
            throw std::runtime_error(
                    "Failed to find DataReader with name: " + reader_names[i]);
        }
        readers.push_back(reader);
        waitsets[i] += rti::sub::cond::create_read_condition_ex(
                readers[i],
                dds::sub::status::DataState::any(),
                [&samples_read, readers, i, reader_names]() {
                    samples_read[i] +=
                            process_data(readers[i], reader_names[i]);
                });
    }

    auto below_sample_count = [sample_count](int count) {
        return count < sample_count;
    };
    while (!application::shutdown_requested
           && std::all_of(
                   samples_read.begin(),
                   samples_read.end(),
                   below_sample_count)) {
        // Run the handlers of the active conditions. Wait for up to 1 second.
        for (auto &waitset : waitsets) {
            waitset.dispatch(dds::core::Duration(1));
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
