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

#include <cstdlib>
#include <iostream>

#include <dds/dds.hpp>
#include "keys.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    Topic<keys> topic (participant, "Example keys");

    // Create a publisher for both DataWriters.
    Publisher publisher(participant);

    // Retrieve the default DataWriter QoS profile from USER_QOS_PROFILES.xml
    // for the first writer.
    DataWriterQos writer1_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer1_qos << WriterDataLifecycle::ManuallyDisposeUnregisteredInstances()
    //             << Ownership::Exclusive()
    //             << OwnershipStrength(10)
    //             << Reliability::Reliable(Duration(60))
    //             << History::KeepAll()
    //             << DataWriterProtocol().rtps_reliable_writer(
    //                 RtpsReliableWriterProtocol()
    //                 .min_send_window_size(50)
    //                 .max_send_window_size(50));

    // Create a DataWriter with default Qos.
    DataWriter<keys> writer1(publisher, topic, writer1_qos);

    // Retrieve the keys_Profile_dw2 QoS profile from USER_QOS_PROFILES.xml
    // for the second writer.
    DataWriterQos writer2_qos = QosProvider::Default().datawriter_qos(
        "keys_Library::keys_Profile_dw2");

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer2_qos << WriterDataLifecycle::ManuallyDisposeUnregisteredInstances()
    //             << Ownership::Exclusive()
    //             << OwnershipStrength(5);

    DataWriter<keys> writer2(publisher, topic, writer2_qos);

    std::vector<keys> samples1;
    std::vector<InstanceHandle> instance_handles1;
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
            instance_handles1.push_back(InstanceHandle::nil());
            samples1_active.push_back(false);
        }

        // Finally, we modify the data to be sent.
        k.x(1);
        samples1.push_back(k);
    }

    keys sample2(samples1[1].code(), 2, 0);
    std::cout << "----DW2 registering instance " << sample2.code() << std::endl;
    InstanceHandle instance_handle2 = writer2.register_instance(sample2);
    bool sample2_active = true;

    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        rti::util::sleep(Duration(2));

        // Control first DataWriter.
        if (count == 4) {
            // Start sending the second and third instances.
            std::cout << "----DW1 registering instance " << samples1[1].code()
                      << std::endl
                      << "----DW1 registering instance " << samples1[2].code()
                      << std::endl;
            instance_handles1[1] = writer1.register_instance(samples1[1]);
            instance_handles1[2] = writer1.register_instance(samples1[2]);
            samples1_active[1] = true;
            samples1_active[2] = true;

        } else if (count == 8) {
            // Dispose the second instance.
            std::cout << "----DW1 disposing instance " << samples1[1].code()
                      << std::endl;
            writer1.dispose_instance(instance_handles1[1]);
            samples1_active[1] = false;

        } else if (count == 10) {
            // Unregister the second instance.
            std::cout << "----DW1 unregistering instance " << samples1[1].code()
                      << std::endl;
            writer1.unregister_instance(instance_handles1[1]);
            samples1_active[1] = false;

        } else if (count == 12) {
            // Unregister the third instance.
            std::cout << "----DW1 unregistering instance " << samples1[2].code()
                      << std::endl;
            writer1.unregister_instance(instance_handles1[2]);
            samples1_active[2] = false;

            std::cout << "----DW1 re-registering instance "
                      << samples1[1].code() << std::endl;
            instance_handles1[1] = writer1.register_instance(samples1[1]);
            samples1_active[1] = true;

        } else if (count == 16) {
            std::cout << "----DW1 re-registering instance "
                      << samples1[2].code() << std::endl;
            instance_handles1[2] = writer1.register_instance(samples1[2]);
            samples1_active[2] = true;
        }

        // Send samples for writer 1
        for (int i = 0; i < num_samples; i++) {
            if (samples1_active[i]) {
                samples1[i].y(count + i * 1000);
                std::cout << "DW1 write; code: " << samples1[i].code()
                          << ", x: " << samples1[i].x() << ", y: "
                          << samples1[i].y() << std::endl;
                writer1.write(samples1[i], instance_handles1[i]);
            }
        }

        // Control second DataWriter
        if (count == 16) {
            // Dispose the instance.
            // Since it has lower ownership strength, this does nothing.
            std::cout << "----DW2 disposing instance " << sample2.code()
                      << std::endl;
            writer2.dispose_instance(instance_handle2);
            sample2_active = false;
        }

        // Send sample for writer 2
        sample2.y(-count - 1000);
        if (sample2_active) {
            std::cout << "DW2 write; code: " << sample2.code() << ", x: "
                      << sample2.x() << ", y: " << sample2.y() << std::endl;
            writer2.write(sample2, instance_handle2);
        }
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        publisher_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
