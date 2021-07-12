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
#include "ordered.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id);

    // Retrieve the custom ordered publisher QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::PublisherQos publisher_qos =
            dds::core::QosProvider::Default().publisher_qos(
                    "ordered_Library::ordered_Profile_subscriber_instance");

    // If you want to change the Publisher's QoS programmatically rather
    // than using the XML file, you will need to comment out the previous
    // publisher_qos assignment and uncomment these files.

    // PublisherQos publisher_qos;
    // publisher_qos << Presentation::TopicAccessScope(false, true);

    // Create a publisher with the ordered QoS.
    dds::pub::Publisher publisher(participant, publisher_qos);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<ordered> topic(participant, "Example ordered");

    // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos(
                    "ordered_Library::ordered_Profile_subscriber_instance");

    // If you want to change the Publisher's QoS programmatically rather
    // than using the XML file, you will need to comment out the previous
    // writer_qos assignment and uncomment these files.

    // DataWriterQos writer_qos;
    // writer_qos << Reliability::Reliable();

    // Create a DataWriter.
    dds::pub::DataWriter<ordered> writer(publisher, topic, writer_qos);

    // Create two samples with different ID and register them
    ordered instance0(0, 0);
    dds::core::InstanceHandle handle0 = writer.register_instance(instance0);

    ordered instance1(1, 0);
    dds::core::InstanceHandle handle1 = writer.register_instance(instance1);

    // Main loop
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Update content.
        instance0.value(samples_written);
        instance1.value(samples_written);

        // Write the two samples.
        std::cout << "writing instance0, value->" << instance0.value()
                  << std::endl;
        writer.write(instance0, handle0);

        std::cout << "writing instance1, value->" << instance1.value()
                  << std::endl;
        writer.write(instance1, handle1);

        rti::util::sleep(dds::core::Duration(1));
    }

    // Unregister the samples.
    writer.unregister_instance(handle0);
    writer.unregister_instance(handle1);
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
