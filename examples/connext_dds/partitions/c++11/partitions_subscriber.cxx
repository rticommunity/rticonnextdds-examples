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

#include "partitions.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(dds::sub::DataReader<partitions> reader)
{
    int count = 0;
    // Take all samples
    dds::sub::LoanedSamples<partitions> samples = reader.take();

    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            count++;
            // After partitions match, the instance is considered new
            dds::sub::status::ViewState view_state =
                    sample.info().state().view_state();
            if (view_state == dds::sub::status::ViewState::new_view()) {
                std::cout << "Found new instance" << std::endl;
            }

            std::cout << sample.data() << std::endl;
        }
    }

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

    dds::core::policy::Partition partition =
            subscriber_qos.policy<dds::core::policy::Partition>();
    std::vector<std::string> partition_names = partition.name();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // partition_names[0] = "ABC";
    // partition_names[1] = "X*Z";
    // partition.name(partition_names);
    // subscriber_qos << partition;

    std::cout << "Setting partition to "
              << "'" << partition_names[0] << "', "
              << "'" << partition_names[1] << "'..." << std::endl;

    // Create a subscriber.
    dds::sub::Subscriber subscriber(participant, subscriber_qos);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<partitions> topic(participant, "Example partitions");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // reader_qos << Reliability::Reliable()
    //            << History::KeepAll()
    //            << Durability::TransientLocal();

    // Create a DataReader.
    dds::sub::DataReader<partitions> reader(subscriber, topic, reader_qos);

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

    // Main loop
    while (!application::shutdown_requested && samples_read < sample_count) {
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
