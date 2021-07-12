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

#include "async.h"
#include "asyncSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domainId, unsigned int sample_count)
{
    /* To customize participant QoS, use
       the configuration file USER_QOS_PROFILES.xml */
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domainId,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    NULL /* listener */,
                    DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    // To customize publisher QoS, use the configuration file
    // USER_QOS_PROFILES.xml
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

    // Register the datatype to use when creating the Topic
    const char *type_name = asyncTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            asyncTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example async",
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

    /* To customize data writer QoS, use
       the configuration file USER_QOS_PROFILES.xml */
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_datawriter call above.
     *
     * In this case, we set the publish mode qos to asynchronous publish mode,
     * which enables asynchronous publishing.  We also set the flow controller
     * to be the fixed rate flow controller, and we increase the history depth.
     */
    /* Get default datawriter QoS to customize * /
    DDS_DataWriterQos datawriter_qos;
    retcode = publisher->get_default_datawriter_qos(datawriter_qos);

    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(participant, "get_default_datawriter_qos
    error", EXIT_FAILURE);
    }

    // Since samples are only being sent once per second, datawriter will need
    // to keep them on queue.  History defaults to only keeping the last
    // sample enqueued, so we increase that here.
    datawriter_qos.history.depth = 12;

    // Set flowcontroller for datawriter
    datawriter_qos.publish_mode.kind = DDS_ASYNCHRONOUS_PUBLISH_MODE_QOS;
    datawriter_qos.publish_mode.flow_controller_name =
    DDS_FIXED_RATE_FLOW_CONTROLLER_NAME;

    /* To create datawriter with default QoS, use DDS_DATAWRITER_QOS_DEFAULT
       instead of datawriter_qos * /
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
        topic, datawriter_qos, NULL /* listener * /,
        DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    //// End changes for Asynchronous_Publication
        */

    // Narrow casts from an untyped DataWriter to a writer of your type
    asyncDataWriter *typed_writer = asyncDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Create data for writing, allocating all members
    async *data = asyncTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "asyncTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* For a data type that has a key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    /*
        DDS_InstanceHandle_t instance_handle = typed_writer
                ->register_instance(*data);
    */

    /* Main loop */
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        std::cout << "Writing async, count " << samples_written << std::endl;

        // send count as data.
        data->x = samples_written;

        retcode = typed_writer->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        DDS_Duration_t send_period = { 0, 100000000 };
        NDDSUtility::sleep(send_period);
    }

    // Delete previously allocated async, including all contained elements
    retcode = asyncTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "asyncTypeSupport::delete_data error " << retcode
                  << std::endl;
    }

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
    parse_arguments(arguments, argc, argv);
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
