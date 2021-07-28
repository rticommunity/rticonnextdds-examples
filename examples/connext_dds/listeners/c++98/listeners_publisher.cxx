/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "listeners.h"
#include "listenersSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

class DataWriterListener : public DDSDataWriterListener {
public:
    virtual void on_offered_deadline_missed(
            DDSDataWriter * /*writer*/,
            const DDS_OfferedDeadlineMissedStatus & /*status*/)
    {
        std::cout << "DataWriterListener: on_offered_deadline_missed()\n";
    }

    virtual void on_liveliness_lost(
            DDSDataWriter * /*writer*/,
            const DDS_LivelinessLostStatus & /*status*/)
    {
        std::cout << "DataWriterListener: on_liveliness_lost()\n";
    }

    virtual void on_offered_incompatible_qos(
            DDSDataWriter * /*writer*/,
            const DDS_OfferedIncompatibleQosStatus & /*status*/)
    {
        std::cout << "DataWriterListener: on_offered_incompatible_qos()\n";
    }

    virtual void on_publication_matched(
            DDSDataWriter *writer,
            const DDS_PublicationMatchedStatus &status)
    {
        std::cout << "DataWriterListener: on_publication_matched()\n";
        if (status.current_count_change < 0) {
            std::cout << "lost a subscription\n";
        } else {
            std::cout << "found a subscription\n";
        }
    }

    virtual void on_reliable_writer_cache_changed(
            DDSDataWriter *writer,
            const DDS_ReliableWriterCacheChangedStatus &status)
    {
        std::cout << "DataWriterListener: on_reliable_writer_cache_changed()\n";
    }

    virtual void on_reliable_reader_activity_changed(
            DDSDataWriter *writer,
            const DDS_ReliableReaderActivityChangedStatus &status)
    {
        std::cout << "DataWriterListener: "
                     "on_reliable_reader_activity_changed()\n";
    }
};

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    DDS_Duration_t send_period = { 1, 0 };
    DDS_Duration_t sleep_period = { 2, 0 };

    // Start communicating in a domain, usually one participant per application
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    // A Publisher allows an application to create one or more DataWriters
    DDSPublisher *publisher = participant->create_publisher(
            DDS_PUBLISHER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        return shutdown_participant(
                participant,
                "create_publisher error",
                EXIT_FAILURE);
    }

    /* Create ande Delete Inconsistent Topic
     * ---------------------------------------------------------------
     * Here we create an inconsistent topic to trigger the subscriber
     * application's callback.
     * The inconsistent topic is created with the topic name used in
     * the Subscriber application, but with a different data type --
     * the msg data type defined in partitions.idl.
     * Once it is created, we sleep to ensure the applications discover
     * each other and delete the Data Writer and Topic.
     */

    /* First we register the msg type -- we name it
     * inconsistent_topic_type_name to avoid confusion.
     */
    std::cout << "Creating Inconsistent Topic...  \n";

    const char *inconsistent_type_name = msgTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            msgTypeSupport::register_type(participant, inconsistent_type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    DDSTopic *inconsistent_topic = participant->create_topic(
            "Example listeners",
            inconsistent_type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (inconsistent_topic == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    /* We have to associate a writer to the topic, as Topic information is not
     * actually propagated until the creation of an associated writer.
     */
    DDSDataWriter *inconsistent_topic_writer = publisher->create_datawriter(
            inconsistent_topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (inconsistent_topic == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    msgDataWriter *msg_writer =
            msgDataWriter::narrow(inconsistent_topic_writer);
    if (msg_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Sleep to leave time for applications to discover each other */
    NDDSUtility::sleep(sleep_period);

    retcode = publisher->delete_datawriter(inconsistent_topic_writer);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "delete_datawriter error",
                EXIT_FAILURE);
    }

    retcode = participant->delete_topic(inconsistent_topic);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "delete_topic error",
                EXIT_FAILURE);
    }

    std::cout << "... Deleted Inconsistent Topic\n\n";

    /* Create Consistent Topic
     * -----------------------------------------------------------------
     * Once we have created the inconsistent topic with the wrong type,
     * we create a topic with the right type name -- listeners -- that we
     * will use to publish data.
     */

    // Register the datatype to use when creating the Topic
    const char *type_name = listenersTypeSupport::get_type_name();
    retcode = listenersTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example listeners",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }


    /* We will use the Data Writer Listener defined above to print
     * a message when some of events are triggered in the DataWriter.
     * To do that, first we have to pass the writer_listener and then
     * we have to enable all status in the status mask.
     */
    DataWriterListener *writer_listener = new DataWriterListener();

    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            writer_listener /* listener */,
            DDS_STATUS_MASK_ALL /* enable all statuses */);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    listenersDataWriter *typed_writer =
            listenersDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Create data sample for writing */

    listeners *data = listenersTypeSupport::create_data();

    if (data == NULL) {
        return shutdown_participant(
                participant,
                "listenersTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* For a data type that has a key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    /*
      instance_handle = typed_writer->register_instance(*data);
    */

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        std::cout << "Writing listeners, count " << samples_written
                  << std::endl;

        // Modify the data to be sent here
        data->x = samples_written;

        retcode = typed_writer->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        NDDSUtility::sleep(send_period);
    }

    /*
        retcode = typed_writer->unregister_instance(
            *data, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr "unregister instance error " << retcode << std::endl);
        }
    */

    // Delete previously allocated data, including all contained elements
    retcode = listenersTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "listenersTypeSupport::delete_data error " << retcode
                  << std::endl;
    }

    // Deallocate dynamic memory
    delete writer_listener;

    // Delete all entities (DataWriter, Topic, Publisher, DomainParticipant)
    return shutdown_participant(participant, "Shutting down", EXIT_SUCCESS);
}

// Delete all entities
static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status)
{
    DDS_ReturnCode_t retcode;

    std::cout << shutdown_message << std::endl;

    if (participant != NULL) {
        // Cleanup everything created by this Participant
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_contained_entities error " << retcode
                      << std::endl;
            status = EXIT_FAILURE;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_participant error " << retcode << std::endl;
            status = EXIT_FAILURE;
        }
    }

    return status;
}

int main(int argc, char *argv[])
{
    // Parse arguments and handle control-C
    ApplicationArguments arguments;
    parse_arguments(arguments, argc, argv, publisher);
    if (arguments.parse_result == PARSE_RETURN_EXIT) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == PARSE_RETURN_FAILURE) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    NDDSConfigLogger::get_instance()->set_verbosity(arguments.verbosity);

    int status = run_publisher_application(
            arguments.domain_id,
            arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error " << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
