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

#include "coherent.h"
#include "coherentSupport.h"
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
    int mod;

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

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_publisher call above.
     */

    /* Start changes for coherent_presentation */

    /* Get default publisher QoS to customize */

    /*    retcode = participant->get_default_publisher_qos(publisher_qos);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "get_default_publisher_qos error\n");
            return EXIT_FAILURE;
        }

    */    /* Topic access scope means that writes from a particular datawriter to 
     * multiple instances will be viewed coherently.
     */
    /*    publisher_qos.presentation.access_scope = DDS_TOPIC_PRESENTATION_QOS;
        publisher_qos.presentation.coherent_access = DDS_BOOLEAN_TRUE;

        publisher = participant->create_publisher(publisher_qos, NULL,
                DDS_STATUS_MASK_NONE);
        if (publisher == NULL) {
            return shutdown_participant(participant, "create_publisher error",
       EXIT_FAILURE);
        }

    */    /* End changes for coherent_presentation */

    // Register the datatype to use when creating the Topic
    const char *type_name = coherentTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            coherentTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example coherent",
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

    // This DataWriter writes data on "Example async" Topic
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
     */
    /* Start changes for coheren_presentation */

    /*    retcode = publisher->get_default_datawriter_qos(datawriter_qos);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "get_default_datawriter_qos error\n";
            return EXIT_FAILURE;
        }

        datawriter_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
        datawriter_qos.history.depth = 10;
        datawriter_qos.protocol.rtps_reliable_writer.heartbeats_per_max_samples
       = 0;

        untyped_writer = publisher->create_datawriter(topic, datawriter_qos,
       NULL, DDS_STATUS_MASK_NONE); if (untyped_writer == NULL) { return
       shutdown_participant(participant, "create_datawriter error",
       EXIT_FAILURE);
        }
    */    /* End changes for coherent_presentation */

    coherentDataWriter *typed_writer =
            coherentDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Create data sample for writing */

    coherent *data = coherentTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "asyncTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* For a data type that has a key, if the same instance is going to be
     written multiple times, initialize the key here
     and register the keyed instance prior to writing */

    data->id = 0;
    DDS_InstanceHandle_t instance_handle =
            typed_writer->register_instance(*data);

    publisher->begin_coherent_changes();
    std::cout << "Begin Coherent Changes\n";

    /* Main loop */
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        NDDSUtility::sleep(send_period);

        mod = samples_written % 7;
        if (mod == 6) {
            publisher->begin_coherent_changes();
            std::cout << "Begin Coherent Changes\n";
            continue;
        }

        // Choose a random value for each field
        data->field = 'a' + mod;
        data->value = (int) (rand() / (RAND_MAX / 10.0));

        std::cout << "  Updating instance, " << data->field << "->"
                  << data->value << std::endl;

        retcode = typed_writer->write(*data, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        if (mod == 5) {
            publisher->end_coherent_changes();
            std::cout << "End Coherent Changes\n\n";
        }
    }

    // Delete previously allocated async, including all contained elements
    retcode = typed_writer->unregister_instance(*data, instance_handle);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "unregister instance error " << retcode << std::endl;
    }

    /* Delete data sample */
    retcode = coherentTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "coherentTypeSupport::delete_data error " << retcode
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
