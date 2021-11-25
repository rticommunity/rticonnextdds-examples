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
#include "cft.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // To customize any of the entities QoS, use
    // the configuration file USER_QOS_PROFILES.xml

    dds::domain::DomainParticipant participant(domain_id);

    dds::topic::Topic<cft> topic(participant, "Example cft");

    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to set the reliability and history QoS settings
    // programmatically rather than using the XML, you will need to comment out
    // the following lines of code.
    // writer_qos << Reliability::Reliable()
    //            << Durability::TransientLocal()
    //            << History::KeepLast(20);

    dds::pub::Publisher publisher(participant);
    dds::pub::DataWriter<cft> writer(publisher, topic, writer_qos);

    // Create data sample for writing
    cft instance;

    // For a data type that has a key, if the same instance is going to be
    // written multiple times, initialize the key here
    // and register the keyed instance prior to writing.

    dds::core::InstanceHandle instance_handle =
            dds::core::InstanceHandle::nil();
    // instance_handle = writer.register_instance(instance);

    // Main loop
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify the data to be sent here

        // Our purpose is to increment x every time we send a sample and to
        // reset the x counter to 0 every time we send 10 samples (x=0,1,..,9).
        // Using the value of count, we can get set x to the appropriate value
        // applying % 10 operation to it.
        instance.count(samples_written);
        instance.x(samples_written % 10);

        std::cout << "Writing cft, count " << instance.count() << "\t"
                  << "x=" << instance.x() << std::endl;

        writer.write(instance, instance_handle);

        // Send a new sample every second
        rti::util::sleep(dds::core::Duration(1));
    }

    // writer.unregister_instance(instance_handle);
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
