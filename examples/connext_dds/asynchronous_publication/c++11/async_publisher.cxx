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

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"  // for command line parsing and ctrl-c
#include "async.hpp"


void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // To customize participant QoS, use file USER_QOS_PROFILES.xml
    dds::domain::DomainParticipant participant(domain_id);

    // To customize topic QoS, use file USER_QOS_PROFILES.xml
    dds::topic::Topic<async> topic(participant, "Example async");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    //
    // In this case, we set the publish mode QoS to asynchronous publish mode,
    // which enables asynchronous publishing. We also set the flow controller
    // to be the fixed rate flow controller, and we increase the history depth.
    // writer_qos << Reliability::Reliable(Duration(60));

    // DataWriterProtocol writer_protocol_qos;
    // RtpsReliableWriterProtocol rtps_writer_protocol;
    // rtps_writer_protocol.min_send_window_size(50);
    // rtps_writer_protocol.max_send_window_size(50);
    // writer_protocol_qos.rtps_reliable_writer(rtps_writer_protocol);
    // writer_qos << writer_protocol_qos;

    // Since samples are only being sent once per second, DataWriter will need
    // to keep them on queue.  History defaults to only keeping the last
    // sample enqueued, so we increase that here.
    // writer_qos << History::KeepLast(12);

    // Set flowcontroller for DataWriter
    // writer_qos << PublishMode::Asynchronous(FlowController::FIXED_RATE_NAME);

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create the DataWriter with a QoS.
    dds::pub::DataWriter<async> writer(publisher, topic, writer_qos);

    // Create data sample for writing
    async instance;

    // For a data type that has a key, if the same instance is going to be
    // written multiple times, initialize the key here
    // and register the keyed instance prior to writing.
    // InstanceHandle instance_handle = writer.register_instance(instance);

    // Main loop
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        std::cout << "Writing async, count " << samples_written << std::endl;

        // Send count as data
        instance.x(samples_written);

        // Send it, if using instance_handle:
        // writer.write(instance, instance_handle);
        writer.write(instance);

        rti::util::sleep(dds::core::Duration::from_millisecs(100));
    }

    // If using instance_handle, unregister it.
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
