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
#include <dds/sub/ddssub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"  // for command line parsing and ctrl-c
#include "deadline_contentfilter.hpp"

class DeadlineWriterListener
        : public dds::pub::NoOpDataWriterListener<deadline_contentfilter> {
public:
    void on_offered_deadline_missed(
            dds::pub::DataWriter<deadline_contentfilter> &writer,
            const dds::core::status::OfferedDeadlineMissedStatus &status)
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

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<deadline_contentfilter> topic(
            participant,
            "Example deadline_contentfilter");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // writer_qos << Deadline(Duration::from_millisecs(1500));

    // Create a publisher
    dds::pub::Publisher publisher(participant);

    // Create a shared pointer for DeadlineWriterListener class
    auto deadline_listener = std::make_shared<DeadlineWriterListener>();

    // Create a DataWriter with the listener
    dds::pub::DataWriter<deadline_contentfilter> writer(
            publisher,
            topic,
            writer_qos,
            deadline_listener);

    // Create two instances for writing and register them in order to be able
    // to recover them from the instance handle in the listener.
    deadline_contentfilter sample0(0, 0, 0);
    dds::core::InstanceHandle handle0 = writer.register_instance(sample0);

    deadline_contentfilter sample1(1, 0, 0);
    dds::core::InstanceHandle handle1 = writer.register_instance(sample1);

    // Main loop.
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        rti::util::sleep(dds::core::Duration(1));

        // Update non-key fields.
        sample0.x(samples_written);
        sample0.y(samples_written);
        sample1.x(samples_written);
        sample1.y(samples_written);

        std::cout << "Writing instance0, x = " << sample0.x()
                  << ", y = " << sample0.y() << std::endl;
        writer.write(sample0, handle0);

        if (samples_written < 15) {
            std::cout << "Writing instance1, x = " << sample1.x()
                      << ", y = " << sample1.y() << std::endl;
            writer.write(sample1, handle1);
        } else if (samples_written == 15) {
            std::cout << "Stopping writes to instance1" << std::endl;
        }
    }

    // Unregister the instances.
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
