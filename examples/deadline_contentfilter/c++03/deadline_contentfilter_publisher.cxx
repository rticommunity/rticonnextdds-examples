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

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "deadline_contentfilter.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

class DeadlineWriterListener :
    public NoOpDataWriterListener<deadline_contentfilter> {
public:
    void on_offered_deadline_missed(
        DataWriter<deadline_contentfilter> &writer,
        const OfferedDeadlineMissedStatus &status)
    {
        // Creates a temporary object in order to find out which instance
        // missed its deadline period. The 'key_value' call only fills in the
        // values of the key fields inside the object.
        deadline_contentfilter affected_sample;
        writer.key_value(affected_sample, status.last_instance_handle());

        // Print out which instance missed its deadline.
        std::cout << "Offered deadline missed on instance code = "
                  << affected_sample.code() << std::endl;
    }
};

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    Topic<deadline_contentfilter> topic(
        participant, "Example deadline_contentfilter");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer_qos << Deadline(Duration::from_millisecs(1500));

    // Create a DataWriter (Publisher created in-line)
    DataWriter<deadline_contentfilter> writer(Publisher(participant), topic);

    // Associate a listener to the DataWriter using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    rti::core::ListenerBinder<DataWriter<deadline_contentfilter> > listener =
        rti::core::bind_and_manage_listener(
            writer,
            new DeadlineWriterListener,
            StatusMask::all());

    // Create two instances for writing and register them in order to be able
    // to recover them from the instance handle in the listener.
    deadline_contentfilter sample0(0, 0, 0);
    InstanceHandle handle0 = writer.register_instance(sample0);

    deadline_contentfilter sample1(1, 0, 0);
    InstanceHandle handle1 = writer.register_instance(sample1);

    // Main loop.
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        rti::util::sleep(Duration(1));

        // Update non-key fields.
        sample0.x(count);
        sample0.y(count);
        sample1.x(count);
        sample1.y(count);

        std::cout << "Writing instance0, x = " << sample0.x() << ", y = "
                  << sample0.y() << std::endl;
        writer.write(sample0, handle0);

        if (count < 15) {
            std::cout << "Writing instance1, x = " << sample1.x() << ", y = "
                      << sample1.y() << std::endl;
            writer.write(sample1, handle1);
        } else if (count == 15) {
            std::cout << "Stopping writes to instance1" << std::endl;
        }
    }

    // Unregister the instances.
    writer.unregister_instance(handle0);
    writer.unregister_instance(handle1);
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
