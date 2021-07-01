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

#include "ordered.h"
#include "orderedSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    DDS_Duration_t send_period = { 1, 0 };

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
    DDSPublisher *publisher = participant->create_publisher_with_profile(
            "ordered_Library",
            "ordered_Profile_subscriber_instance",
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        return shutdown_participant(
                participant,
                "create_publisher error",
                EXIT_FAILURE);
    }

    /* If you want to change the Publisher's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_publisher call above.
     *
     * In this case, we set the presentation publish mode ordered in the topic.
     */

    /* Get default publisher QoS to customize */
    /*    DDS_PublisherQos publisher_qos;
        retcode = participant->get_default_publisher_qos(publisher_qos);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "get_default_publisher_qos error\n";
            return EXIT_FAILURE;
        }

        publisher_qos.presentation.access_scope = DDS_TOPIC_PRESENTATION_QOS;
        publisher_qos.presentation.ordered_access = DDS_BOOLEAN_TRUE;

    */    /* To create publisher with default QoS, use DDS_PUBLISHER_QOS_DEFAULT
     instead of publisher_qos */
    /*  DDSPublisher *publisher = participant->create_publisher(
                publisher_qos,
                NULL,
                DDS_STATUS_MASK_NONE);
        if (publisher == NULL) {
            return shutdown_participant(
                participant,
                "create_publisher error",
                EXIT_FAILURE);
        }
    */

    // Register the datatype to use when creating the Topic
    const char *type_name = orderedTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            orderedTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example ordered",
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

    // This DataWriter writes data on "Example ordered" Topic
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

    // Narrow casts from an untyped DataWriter to a writer of your type
    orderedDataWriter *typed_writer = orderedDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Start changes for Ordered_Presentation */
    /* Create two instances*/
    ordered *instance0 = NULL;
    ordered *instance1 = NULL;
    DDS_InstanceHandle_t handle0 = DDS_HANDLE_NIL;
    DDS_InstanceHandle_t handle1 = DDS_HANDLE_NIL;

    /* Create data sample for writing */

    instance0 = orderedTypeSupport::create_data();

    if (instance0 == NULL) {
        return shutdown_participant(
                participant,
                "orderedTypeSupport::create_data0 error",
                EXIT_FAILURE);
    }

    instance1 = orderedTypeSupport::create_data();

    if (instance1 == NULL) {
        return shutdown_participant(
                participant,
                "orderedTypeSupport::create_data1 error",
                EXIT_FAILURE);
    }

    /* For a data type that has a key, if the same instance is going to be
     written multiple times, initialize the key here
     and register the keyed instance prior to writing */
    instance0->id = 0;
    instance1->id = 1;

    handle0 = typed_writer->register_instance(*instance0);
    handle1 = typed_writer->register_instance(*instance1);

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        instance0->value = samples_written;
        instance1->value = samples_written;

        std::cout << "writing instance0, value->" << instance0->value
                  << std::endl;
        retcode = typed_writer->write(*instance0, handle0);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        std::cout << "writing instance1, value->" << instance1->value
                  << std::endl;
        retcode = typed_writer->write(*instance1, handle1);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }
        NDDSUtility::sleep(send_period);
    }

    retcode = typed_writer->unregister_instance(*instance0, handle0);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "unregister instance0 error " << retcode << std::endl;
    }

    retcode = typed_writer->unregister_instance(*instance1, handle1);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "unregister instance1 error " << retcode << std::endl;
    }

    /* Delete data sample */
    retcode = orderedTypeSupport::delete_data(instance0);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "orderedTypeSupport::delete_data0 error " << retcode
                  << std::endl;
    }

    retcode = orderedTypeSupport::delete_data(instance1);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "orderedTypeSupport::delete_data1 error " << retcode
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
