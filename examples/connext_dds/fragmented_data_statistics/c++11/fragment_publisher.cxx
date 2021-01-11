/*
* (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

#include <iostream>

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging
// alternatively, to include all the standard APIs:
//  <dds/dds.hpp>
// or to include both the standard APIs and extensions:
//  <rti/rti.hpp>
//
// For more information about the headers and namespaces, see:
//    https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/api/connext_dds/api_cpp2/group__DDSNamespaceModule.html
// For information on how to use extensions, see:
//    https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/api/connext_dds/api_cpp2/group__DDSCpp2Conventions.html

#include "application.hpp"  // for command line parsing and ctrl-c
#include "fragment.hpp"

void run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    // DDS objects behave like shared pointers or value types
    // (see https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/api/connext_dds/api_cpp2/group__DDSCpp2Conventions.html)

    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic with a name and a datatype
    dds::topic::Topic<fragment> topic(participant, "Example fragment");

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter with default QoS
    dds::pub::DataWriter<fragment> writer(publisher, topic);

    fragment data;
    // Create the data to be written, ensuring it is larger than message_size_max */
    data.data().resize(8000);

    rti::core::status::DataWriterProtocolStatus status;

    for (unsigned int samples_written = 0;
    !application::shutdown_requested && samples_written < sample_count;
    samples_written++) {
        // Modify the data to be written here
        data.x(static_cast<int32_t>(samples_written));

        std::cout << "Writing fragment, count " << samples_written << std::endl;

        writer.write(data);

        // Send once every second
        rti::util::sleep(dds::core::Duration(1));

        status = writer->datawriter_protocol_status();
        std::cout << "Fragmented Data Statistics:"
                  << "\n\t pushed_fragment_count "
                  << status.pushed_fragment_count()
                  << "\n\t pushed_fragment_bytes "
                  << status.pushed_fragment_bytes()
                  << "\n\t pulled_fragment_count "
                  << status.pulled_fragment_count()
                  << "\n\t pulled_fragment_bytes "
                  <<  status.pulled_fragment_bytes()
                  << "\n\t received_nack_fragment_count "
                  << status.received_nack_fragment_count()
                  << "\n\t received_nack_fragment_bytes "
                  << status.received_nack_fragment_bytes() << std::endl;
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
    } catch (const std::exception& ex) {
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
