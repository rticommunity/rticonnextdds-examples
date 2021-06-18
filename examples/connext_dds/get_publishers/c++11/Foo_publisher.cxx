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

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"  // for command line parsing and ctrl-c
#include "Foo.hpp"

void run_publisher_application(unsigned int domain_id)
{
    using namespace rti::core::policy;

    // Create a DomainParticipant with default QoS.
    dds::domain::DomainParticipant participant(domain_id);

    // Retrieve publisher QoS to set names.
    dds::pub::qos::PublisherQos publisher_qos =
            dds::core::QosProvider::Default().publisher_qos();

    // Create the first publisher.
    publisher_qos << EntityName("Foo");
    dds::pub::Publisher publisher1(participant, publisher_qos);
    std::cout << "The first  publisher name is "
              << publisher1.qos().policy<EntityName>().name().get()
              << std::endl;

    // Create the second publisher.
    publisher_qos << EntityName("Bar");
    dds::pub::Publisher publisher2(participant, publisher_qos);
    std::cout << "The second publisher name is "
              << publisher2.qos().policy<EntityName>().name().get()
              << std::endl;

    // Find the publishers.
    std::cout << std::endl
              << "Calling to find_publishers()..." << std::endl
              << std::endl;
    std::vector<dds::pub::Publisher> publishers;
    rti::pub::find_publishers(participant, std::back_inserter(publishers));

    // Iterate over the publishers and print the name.
    std::cout << "Found " << publishers.size()
              << " publishers on this participant" << std::endl;
    for (std::vector<int>::size_type i = 0; i < publishers.size(); i++) {
        std::cout << "The " << i << " publisher name is "
                  << publishers[i].qos().policy<EntityName>().name().get()
                  << std::endl;
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
        run_publisher_application(arguments.domain_id);
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
