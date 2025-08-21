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
//    https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_cpp2/group__DDSNamespaceModule.html
// For information on how to use extensions, see:
//    https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_cpp2/group__DDSCpp2Conventions.html

#include "application.hpp"  // for command line parsing and ctrl-c
#include "instance.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // DDS objects behave like shared pointers or value types
    // (see
    // https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_cpp2/group__DDSCpp2Conventions.html)

    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic with a name and a datatype
    dds::topic::Topic<instance> topic(participant, "Example instance");

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter with default QoS
    dds::pub::DataWriter<instance> writer(publisher, topic);

    instance data;
    dds::core::InstanceHandle instance_handle =
            dds::core::InstanceHandle::nil();
    rti::core::status::DataWriterCacheStatus status;
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify the data to be written here
        data.x = static_cast<int16_t>(samples_written);

        std::cout << "Writing instance, count " << samples_written << std::endl;
        instance_handle = writer.register_instance(data);
        writer.write(data, instance_handle);

        // Send once every second
        rti::util::sleep(dds::core::Duration(1));

        if (samples_written % 2 == 0) {
            /* Unregister the instance */
            writer.unregister_instance(instance_handle);
        } else if (samples_written % 3 == 0) {
            /* Dispose the instance */
            writer.dispose_instance(instance_handle);
        }
        status = writer.extensions().datawriter_cache_status();
        std::cout << "Instance statistics:"
                  << "\n\t alive_instance_count "
                  << status.alive_instance_count()
                  << "\n\t unregistered_instance_count "
                  << status.unregistered_instance_count()
                  << "\n\t disposed_instance_count "
                  << status.disposed_instance_count() << std::endl;
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
