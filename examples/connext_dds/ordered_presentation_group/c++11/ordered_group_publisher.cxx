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
#include "ordered_group.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Retrieve the default Publisher QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::PublisherQos publisher_qos =
            dds::core::QosProvider::Default().publisher_qos();

    // If you want to change the Publisher's QoS programmatically rather than
    // using the XML file, uncomment the following line.

    // publisher_qos << Presentation::GroupAccessScope(false, true);

    // Create a single Publisher for all DataWriters.
    dds::pub::Publisher publisher(participant, publisher_qos);

    // Create three Topic, once for each DataWriter.
    dds::topic::Topic<ordered_group> topic1(participant, "Topic1");
    dds::topic::Topic<ordered_group> topic2(participant, "Topic2");
    dds::topic::Topic<ordered_group> topic3(participant, "Topic3");

    // Create three DataWriter. Since the QoS policy is set to 'GROUP',
    // and the three writers belong to the same Publisher, the instance change
    // will be available for the Subscriber in the same order.
    dds::pub::DataWriter<ordered_group> writer1(publisher, topic1);
    dds::pub::DataWriter<ordered_group> writer2(publisher, topic2);
    dds::pub::DataWriter<ordered_group> writer3(publisher, topic3);

    // Create one instance for each DataWriter.
    ordered_group instance1;
    ordered_group instance2;
    ordered_group instance3;

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        std::cout << "Writing ordered_group, count " << samples_written
                  << std::endl;

        // Modify twice the first instance and send with the first writer.
        instance1.message("First sample, Topic 1 sent by DataWriter number 1");
        writer1.write(instance1);
        instance1.message("Second sample, Topic 1 sent by DataWriter number 1");
        writer1.write(instance1);

        // Modify twice the second instance and send with the second writer.
        instance2.message("First sample, Topic 2 sent by DataWriter number 2");
        writer2.write(instance2);
        instance2.message("Second sample, Topic 2 sent by DataWriter number 2");
        writer2.write(instance2);

        // Modify twice the third instance and send with the third writer.
        instance3.message("First sample, Topic 3 sent by DataWriter number 3");
        writer3.write(instance3);
        instance3.message("Second sample, Topic 3 sent by DataWriter number 3");
        writer3.write(instance3);

        rti::util::sleep(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, ApplicationKind::publisher);
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
