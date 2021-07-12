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
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<keys> topic(participant, "Example keys");

    // Create a publisher for both DataWriters.
    dds::pub::Publisher publisher(participant);

    // Retrieve the default DataWriter QoS profile from USER_QOS_PROFILES.xml
    // for the first writer.
    dds::pub::qos::DataWriterQos writer1_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer1_qos <<
    // WriterDataLifecycle::ManuallyDisposeUnregisteredInstances()
    //             << Ownership::Exclusive()
    //             << OwnershipStrength(10)
    //             << Reliability::Reliable(Duration(60))
    //             << History::KeepAll()
    //             << DataWriterProtocol().rtps_reliable_writer(
    //                 RtpsReliableWriterProtocol()
    //                 .min_send_window_size(50)
    //                 .max_send_window_size(50));

    // Create a DataWriter with default Qos.
    dds::pub::DataWriter<keys> writer1(publisher, topic, writer1_qos);

    // Retrieve the keys_Profile_dw2 QoS profile from USER_QOS_PROFILES.xml
    // for the second writer.
    dds::pub::qos::DataWriterQos writer2_qos =
            dds::core::QosProvider::Default().datawriter_qos(
                    "keys_Library::keys_Profile_dw2");

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer2_qos <<
    // WriterDataLifecycle::ManuallyDisposeUnregisteredInstances()
    //             << Ownership::Exclusive()
    //             << OwnershipStrength(5);

    dds::pub::DataWriter<keys> writer2(publisher, topic, writer2_qos);

    std::vector<keys> samples1;
    std::vector<dds::core::InstanceHandle> instance_handles1;
    std::vector<bool> samples1_active;

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
            std::cout << "----DW1 registering instance " << k.code()
                      << std::endl;
            instance_handles1.push_back(writer1.register_instance(k));
            samples1_active.push_back(true);
        } else {
            instance_handles1.push_back(dds::core::InstanceHandle::nil());
            samples1_active.push_back(false);
        }

        // Finally, we modify the data to be sent.
        k.x(1);
        samples1.push_back(k);
    }

    keys sample2(samples1[1].code(), 2, 0);
    std::cout << "----DW2 registering instance " << sample2.code() << std::endl;
    dds::core::InstanceHandle instance_handle2 =
            writer2.register_instance(sample2);
    bool sample2_active = true;

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        rti::util::sleep(dds::core::Duration(2));

        // Control first DataWriter.
        if (samples_written == 4) {
            // Start sending the second and third instances.
            std::cout << "----DW1 registering instance " << samples1[1].code()
                      << std::endl
                      << "----DW1 registering instance " << samples1[2].code()
                      << std::endl;
            instance_handles1[1] = writer1.register_instance(samples1[1]);
            instance_handles1[2] = writer1.register_instance(samples1[2]);
            samples1_active[1] = true;
            samples1_active[2] = true;

        } else if (samples_written == 8) {
            // Dispose the second instance.
            std::cout << "----DW1 disposing instance " << samples1[1].code()
                      << std::endl;
            writer1.dispose_instance(instance_handles1[1]);
            samples1_active[1] = false;

        } else if (samples_written == 10) {
            // Unregister the second instance.
            std::cout << "----DW1 unregistering instance " << samples1[1].code()
                      << std::endl;
            writer1.unregister_instance(instance_handles1[1]);
            samples1_active[1] = false;

        } else if (samples_written == 12) {
            // Unregister the third instance.
            std::cout << "----DW1 unregistering instance " << samples1[2].code()
                      << std::endl;
            writer1.unregister_instance(instance_handles1[2]);
            samples1_active[2] = false;

            std::cout << "----DW1 re-registering instance "
                      << samples1[1].code() << std::endl;
            instance_handles1[1] = writer1.register_instance(samples1[1]);
            samples1_active[1] = true;

        } else if (samples_written == 16) {
            std::cout << "----DW1 re-registering instance "
                      << samples1[2].code() << std::endl;
            instance_handles1[2] = writer1.register_instance(samples1[2]);
            samples1_active[2] = true;
        }

        // Send samples for writer 1
        for (int i = 0; i < num_samples; i++) {
            if (samples1_active[i]) {
                samples1[i].y(samples_written + i * 1000);
                std::cout << "DW1 write; code: " << samples1[i].code()
                          << ", x: " << samples1[i].x()
                          << ", y: " << samples1[i].y() << std::endl;
                writer1.write(samples1[i], instance_handles1[i]);
            }
        }

        // Control second DataWriter
        if (samples_written == 16) {
            // Dispose the instance.
            // Since it has lower ownership strength, this does nothing.
            std::cout << "----DW2 disposing instance " << sample2.code()
                      << std::endl;
            writer2.dispose_instance(instance_handle2);
            sample2_active = false;
        }

        // Send sample for writer 2
        sample2.y(-samples_written - 1000);
        if (sample2_active) {
            std::cout << "DW2 write; code: " << sample2.code()
                      << ", x: " << sample2.x() << ", y: " << sample2.y()
                      << std::endl;
            writer2.write(sample2, instance_handle2);
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
