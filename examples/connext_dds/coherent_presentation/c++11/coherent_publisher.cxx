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
#include "coherent.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<coherent> topic(participant, "Example coherent");

    // Retrieve the Publisher QoS, from USER_QOS_PROFILES.xml.
    dds::pub::qos::PublisherQos publisher_qos =
            dds::core::QosProvider::Default().publisher_qos();

    // If you want to change the Publisher's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following lines.

    // publisher_qos << Presentation::TopicAccessScope(true, false);

    dds::pub::Publisher publisher(participant, publisher_qos);

    // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml.
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following lines.

    // writer_qos << Reliability::Reliable()
    //            << History::KeepLast(10)
    //            << DataWriterProtocol().rtps_reliable_writer(
    //                 RtpsReliableWriterProtocol().heartbeats_per_max_samples(0));

    // Create a DataWriter with default Qos
    dds::pub::DataWriter<coherent> writer(publisher, topic, writer_qos);

    coherent sample;
    sample.id(0);
    dds::core::InstanceHandle handle = writer.register_instance(sample);

    int num_samples = 7;
    for (unsigned int count = 0;
         !application::shutdown_requested && count < sample_count;) {
        dds::pub::CoherentSet coherent_set(publisher);
        std::cout << "Begin Coherent Changes" << std::endl;

        for (int i = 0; i < num_samples; i++, count++) {
            rti::util::sleep(dds::core::Duration(1));

            sample.field('a' + i);
            sample.value((int) (rand() / (RAND_MAX / 10.0)));
            std::cout << "\tUpdating instance, " << sample.field() << "->"
                      << sample.value() << std::endl;
            writer.write(sample, handle);
        }

        // `coherent_set.end()` will be called by the destructor.
        std::cout << "End Coherent Changes" << std::endl << std::endl;
    }

    writer.unregister_instance(handle);
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
