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
#include "listeners.hpp"

class MyDataWriterListener
        : public dds::pub::NoOpDataWriterListener<listeners> {
public:
    virtual void on_offered_deadline_missed(
            dds::pub::DataWriter<listeners> &writer,
            const dds::core::status::OfferedDeadlineMissedStatus &status)
    {
        std::cout << "DataWriterListener: on_offered_deadline_missed()"
                  << std::endl;
    }

    virtual void on_liveliness_lost(
            dds::pub::DataWriter<listeners> &writer,
            const dds::core::status::LivelinessLostStatus &status)
    {
        std::cout << "DataWriterListener: on_liveliness_lost()" << std::endl;
    }

    virtual void on_offered_incompatible_qos(
            dds::pub::DataWriter<listeners> &writer,
            const dds::core::status::OfferedIncompatibleQosStatus &status)
    {
        std::cout << "DataWriterListener: on_offered_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_publication_matched(
            dds::pub::DataWriter<listeners> &writer,
            const dds::core::status::PublicationMatchedStatus &status)
    {
        std::cout << "DataWriterListener: on_publication_matched()"
                  << std::endl;
        if (status.current_count_change() < 0) {
            std::cout << "lost a subscription" << std::endl;
        } else {
            std::cout << "found a subscription" << std::endl;
        }
    }

    virtual void on_reliable_writer_cache_changed(
            dds::pub::DataWriter<listeners> &writer,
            const rti::core::status::ReliableWriterCacheChangedStatus &status)
    {
        std::cout << "DataWriterListener: on_reliable_writer_cache_changed()"
                  << std::endl;
    }

    virtual void on_reliable_reader_activity_changed(
            dds::pub::DataWriter<listeners> &writer,
            const rti::core::status::ReliableReaderActivityChangedStatus
                    &status)
    {
        std::cout << "DataWriterListener: on_reliable_reader_activity_changed()"
                  << std::endl;
    }
};

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create the participant.
    // To customize QoS, use the configuration file USER_QOS_PROFILES.xml
    dds::domain::DomainParticipant participant(domain_id);

    // Create the publisher
    // To customize QoS, use the configuration file USER_QOS_PROFILES.xml
    dds::pub::Publisher publisher(participant);

    // Create ande Delete Inconsistent Topic
    // ---------------------------------------------------------------
    // Here we create an inconsistent topic to trigger the subscriber
    // application's callback.
    // The inconsistent topic is created with the topic name used in
    // the Subscriber application, but with a different data type --
    // the msg data type defined in listener.idl.
    // Once it is created, we sleep to ensure the applications discover
    // each other and delete the Data Writer and Topic.
    std::cout << "Creating Inconsistent Topic..." << std::endl;
    dds::topic::Topic<msg> inconsistent_topic(participant, "Example listeners");

    // We have to associate a writer to the topic, as Topic information is not
    // actually propagated until the creation of an associated writer.
    dds::pub::DataWriter<msg> inconsistent_writer(
            publisher,
            inconsistent_topic);

    // Sleep to leave time for applications to discover each other.
    rti::util::sleep(dds::core::Duration(2));

    inconsistent_writer.close();
    inconsistent_topic.close();
    std::cout << "... Deleted Incosistent Topic" << std::endl << std::endl;

    // Create Consistent Topic
    // -----------------------------------------------------------------
    // Once we have created the inconsistent topic with the wrong type,
    // we create a topic with the right type name -- listeners -- that we
    // will use to publish data.
    dds::topic::Topic<listeners> topic(participant, "Example listeners");

    // Create a shared pointer for the Data Writer Listener defined above
    auto dw_listener = std::make_shared<MyDataWriterListener>();

    // We will use the Data Writer Listener defined above to print
    // a message when some of events are triggered in the DataWriter.
    // By using shared_pointer it will take care of setting the
    // listener to NULL on destruction.
    dds::pub::DataWriter<listeners> writer(publisher, topic);
    writer.set_listener(dw_listener);

    // Create data sample for writing
    listeners instance;

    // Main loop
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        std::cout << "Writing listeners, count " << samples_written
                  << std::endl;

        // Modify data and send it.
        instance.x(samples_written);
        writer.write(instance);

        rti::util::sleep(dds::core::Duration(2));
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
