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
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos.
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<keys> topic (participant, "Example keys");

    // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather
    // than using the XML file, you will need to comment out the previous
    // writer_qos assignment and uncomment this line.

    //writer_qos << WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

    // Create a DataWriter with default Qos (Publisher created in-line).
    DataWriter<keys> writer(Publisher(participant), topic, writer_qos);

    std::vector<keys> samples;
    std::vector<InstanceHandle> instance_handles;

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
            instance_handles.push_back(InstanceHandle::nil());
        }

        // Finally, we modify the data to be sent.
        k.x((i + 1) * 1000);
        samples.push_back(k);
    }

    // Main loop
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        rti::util::sleep(Duration(1));

        if (count == 5) {
            // Start sending the second and third instances.
            std::cout << "----Registering instance " << samples[1].code()
                      << std::endl;
            instance_handles[1] = writer.register_instance(samples[1]);

            std::cout << "----Registering instance " << samples[2].code()
                      << std::endl;
            instance_handles[2] = writer.register_instance(samples[2]);
        } else if (count == 10) {
            // Unregister the second instance.
            std::cout << "----Unregistering instance " << samples[1].code()
                      << std::endl;
            writer.unregister_instance(instance_handles[1]);
            instance_handles[1] = InstanceHandle::nil();
        } else if (count == 15) {
            // Dispose the third instance.
            std::cout << "----Disposing instance " << samples[2].code()
                      << std::endl;
            writer.dispose_instance(instance_handles[2]);
            instance_handles[2] = InstanceHandle::nil();
        }

        // Update sample data field and send if handle is not nil.
        for (int i = 0; i < num_samples; i++) {
            if (!instance_handles[i].is_nil()) {
                samples[i].y(count);
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
