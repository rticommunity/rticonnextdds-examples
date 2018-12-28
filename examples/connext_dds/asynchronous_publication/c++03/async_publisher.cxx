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

#include "async.hpp"
#include <dds/dds.hpp>
#include <rti/pub/FlowController.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;
using namespace rti::pub;


void publisher_main(int domain_id, int sample_count)
{
    // To customize participant QoS, use file USER_QOS_PROFILES.xml
    DomainParticipant participant (domain_id);

    // To customize topic QoS, use file USER_QOS_PROFILES.xml
    Topic<async> topic (participant, "Example async");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

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

    // Create the DataWriter with a QoS.
    DataWriter<async> writer (Publisher(participant), topic, writer_qos);

    // Create data sample for writing
    async instance;

    // For a data type that has a key, if the same instance is going to be
    // written multiple times, initialize the key here
    // and register the keyed instance prior to writing.
    // InstanceHandle instance_handle = writer.register_instance(instance);

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        std::cout << "Writing async, count " << count << std::endl;

        // Send count as data
        instance.x(count);

        // Send it, if using instance_handle:
        // writer.write(instance, instance_handle);
        writer.write(instance);

        rti::util::sleep(Duration(0,100000000));
    }

    // If using instance_handle, unregister it.
    //writer.unregister_instance(instance_handle);
}

int main(int argc, char* argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

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
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
