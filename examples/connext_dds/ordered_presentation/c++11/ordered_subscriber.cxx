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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "ordered.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

// Instead of using listeners, we are polling the readers to be able to see
// the order of a set of received samples.
unsigned int poll_readers(std::vector<dds::sub::DataReader<ordered>> &readers)
{
    unsigned int samples_read = 0;
    for (std::vector<int>::size_type i = 0; i < readers.size(); i++) {
        dds::sub::LoanedSamples<ordered> samples = readers[i].take();
        for (auto sample : samples) {
            if (sample.info().valid()) {
                std::cout << std::string(i, '\t') << "Reader " << i
                          << ": Instance" << sample.data().id()
                          << "->value = " << sample.data().value() << std::endl;
                samples_read++;
            }
        }
    }

    return samples_read;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<ordered> topic(participant, "Example ordered");

    // Create a reader per QoS profile: ordering by topic and instance modes.
    std::vector<std::string> profile_names = {
        "ordered_Library::ordered_Profile_subscriber_instance",
        "ordered_Library::ordered_Profile_subscriber_topic"
    };

    std::vector<dds::sub::DataReader<ordered>> readers;
    for (std::vector<int>::size_type i = 0; i < profile_names.size(); i++) {
        std::cout << "Subscriber " << i << " using " << profile_names[i]
                  << std::endl;

        // Retrieve the subscriber QoS from USER_QOS_PROFILES.xml
        dds::sub::qos::SubscriberQos subscriber_qos =
                dds::core::QosProvider::Default().subscriber_qos(
                        profile_names[i]);

        // If you want to change the Subscriber's QoS programmatically rather
        // than using the XML file, you will need to comment out the previous
        // subscriber_qos assignment and uncomment these files.

        // SubscriberQos subscriber_qos;
        // if (i == 0) {
        //     subscriber_qos << Presentation::InstanceAccessScope(false, true);
        // } else if (i == 1) {
        //     subscriber_qos << Presentation::TopicAccessScope(false, true);
        // }

        // Create a subscriber with the custom QoS.
        dds::sub::Subscriber subscriber(participant, subscriber_qos);

        // Retrieve the DataReader QoS from USER_QOS_PROFILES.xml
        dds::sub::qos::DataReaderQos reader_qos =
                dds::core::QosProvider::Default().datareader_qos(
                        profile_names[i]);

        // If you want to change the DataReader's QoS programmatically rather
        // than using the XML file, you will need to comment out the previous
        // reader_qos assignment and uncomment these files.

        // DataReaderQos reader_qos;
        // reader_qos << History::KeepLast(10)
        //            << Reliability::Reliable();

        // Create a DataReader with the custom QoS.
        dds::sub::DataReader<ordered> reader(subscriber, topic, reader_qos);
        readers.push_back(reader);
    }

    // Main loop
    unsigned int samples_read = 0;
    while (!application::shutdown_requested && samples_read < sample_count) {
        std::cout << "ordered subscriber sleeping for 4 sec.." << std::endl;
        rti::util::sleep(dds::core::Duration(4));
        samples_read += poll_readers(readers);
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
