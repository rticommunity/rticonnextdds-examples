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
#include "keys.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos.
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<keys> topic(participant, "Example keys");

    // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather
    // than using the XML file, you will need to comment out the previous
    // writer_qos assignment and uncomment this line.

    // writer_qos <<
    // WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter with default Qos.
    dds::pub::DataWriter<keys> writer(publisher, topic, writer_qos);

    std::vector<keys> samples;
    std::vector<dds::core::InstanceHandle> instance_handles;

    // RTI Connext could examine the key fields each time it needs to determine
    // which data-instance is being modified. However, for performance and
    // semantic reasons, it is better for your application to declare all the
    // data-instances it intends to modify prior to actually writing any
    // samples. This is known as registration.

    int num_samples = 3;
    for (int i = 0; i < num_samples; i++) {
        // In order to register the instances, we must set their associated
        // keys first.
        keys k;
        k.code(i);

        // Initially, we register only the first sample.
        if (i == 0) {
            // The keys must have been set before making this call.
            std::cout << "Registering instance " << k.code() << std::endl;
            instance_handles.push_back(writer.register_instance(k));
        } else {
            instance_handles.push_back(dds::core::InstanceHandle::nil());
        }

        // Finally, we modify the data to be sent.
        k.x((i + 1) * 1000);
        samples.push_back(k);
    }

    // Main loop
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        rti::util::sleep(dds::core::Duration(1));

        if (samples_written == 5) {
            // Start sending the second and third instances.
            std::cout << "----Registering instance " << samples[1].code()
                      << std::endl;
            instance_handles[1] = writer.register_instance(samples[1]);

            std::cout << "----Registering instance " << samples[2].code()
                      << std::endl;
            instance_handles[2] = writer.register_instance(samples[2]);
        } else if (samples_written == 10) {
            // Unregister the second instance.
            std::cout << "----Unregistering instance " << samples[1].code()
                      << std::endl;
            writer.unregister_instance(instance_handles[1]);
            instance_handles[1] = dds::core::InstanceHandle::nil();
        } else if (samples_written == 15) {
            // Dispose the third instance.
            std::cout << "----Disposing instance " << samples[2].code()
                      << std::endl;
            writer.dispose_instance(instance_handles[2]);
            instance_handles[2] = dds::core::InstanceHandle::nil();
        }

        // Update sample data field and send if handle is not nil.
        for (int i = 0; i < num_samples; i++) {
            if (!instance_handles[i].is_nil()) {
                samples[i].y(samples_written);
                std::cout << "Writing instance " << samples[i].code()
                          << ", x: " << samples[i].x()
                          << ", y: " << samples[i].y() << std::endl;
                writer.write(samples[i], instance_handles[i]);
            }
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
