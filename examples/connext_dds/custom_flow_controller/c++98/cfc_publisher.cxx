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

#include "cfc.h"
#include "cfcSupport.h"
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
    const char *cfc_name = "custom_flowcontroller";

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

    /* Start changes for custom_flowcontroller */

    /* If you want to change the Participant's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_participant call above.
     */

    /* Get default participant QoS to customize */
    /*    DDS_DomainParticipantQos participant_qos;
        DDS_ReturnCode_t retcode = DDSTheParticipantFactory->
            get_default_participant_qos(participant_qos);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "get_default_participant_qos error\n";
            return EXIT_FAILURE;
        }

    */    /* By default, data will be sent via shared memory _and_ UDPv4.  Because
     * the flowcontroller limits writes across all interfaces, this halves the
     * effective send rate.  To avoid this, we enable only the UDPv4 transport
     */
    /*    participant_qos.transport_builtin.mask = DDS_TRANSPORTBUILTIN_UDPv4;

    */    /* To create participant with default QoS, use DDS_PARTICIPANT_QOS_DEFAULT
     instead of participant_qos */
    /*    DDSDomainParticipant *participant = DDSTheParticipantFactory
            ->create_participant(
                domain_id,
                participant_qos,
                NULL,
                DDS_STATUS_MASK_NONE);
        if (participant == NULL) {
            return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
        }
    */
    /* End changes for custom_flowcontroller */

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

    // Register the datatype to use when creating the Topic
    const char *type_name = cfcTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            cfcTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example cfc",
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

    // This DataWriter writes data on "Example cfc" Topic
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

    /* Start changes for custom_flowcontroller */

    /* If you want to change the Datawriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the create_datawriter call above.
     *
     * In this case we create the flowcontroller and the neccesary QoS
     * for the datawriter.
     */

    /*    DDS_FlowControllerProperty_t custom_fcp;
        retcode = participant->get_default_flowcontroller_property(custom_fcp);
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                participant,
                "get_default_flowcontroller_property error",
                EXIT_FAILURE);
        }

    */    /* Don't allow too many tokens to accumulate */
    /*    custom_fcp.token_bucket.max_tokens =
                custom_fcp.token_bucket.tokens_added_per_period = 2;
        custom_fcp.token_bucket.tokens_leaked_per_period = DDS_LENGTH_UNLIMITED;

    */    /* 100ms */
    /*    custom_fcp.token_bucket.period.sec = 0;
        custom_fcp.token_bucket.period.nanosec = 100000000;

    */    /* The sample size is 1000, but the minimum bytes_per_token is 1024.
     * Furthermore, we want to allow some overhead.
     */
    /*    custom_fcp.token_bucket.bytes_per_token = 1024;

    */    /* So, in summary, each token can be used to send about one message,
     * and we get 2 tokens every 100ms, so this limits transmissions to
     * about 20 messages per second.
     */

    /* Create flowcontroller and set properties */
    /*    DDSFlowController* cfc = NULL;
        cfc = participant->create_flowcontroller(DDS_String_dup(cfc_name),
                custom_fcp);
        if (cfc == NULL) {
            return shutdown_participant(
                participant,
                "create_flowcontroller error",
                EXIT_FAILURE);
        }

    */    /* Get default datawriter QoS to customize */
    /*    DDS_DataWriterQos datawriter_qos;

        retcode = publisher->get_default_datawriter_qos(datawriter_qos);
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                participant,
                "get_default_datawriter_qos error",
                EXIT_FAILURE);
        }

    */    /* As an alternative to increasing h istory depth, we can just
     * set the qos to keep all samples
     */
    /*    datawriter_qos.history.kind = DDS_KEEP_ALL_HISTORY_QOS;

    */    /* Set flowcontroller for datawriter */
    /*    datawriter_qos.publish_mode.kind = DDS_ASYNCHRONOUS_PUBLISH_MODE_QOS;
        datawriter_qos.publish_mode.flow_controller_name =
       DDS_String_dup(cfc_name);

    */    /* To create datawriter with default QoS, use DDS_DATAWRITER_QOS_DEFAULT
     instead of datawriter_qos */
    /*    DDSDataWriter *untyped_writer =
            publisher->create_datawriter(
                topic,
                datawriter_qos,
                NULL,
                DDS_STATUS_MASK_NONE);
        if (untyped_writer == NULL) {
            return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
        }
    */
    /* End changes for custom_flowcontroller */

    cfcDataWriter *typed_writer = cfcDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Create data sample for writing */

    cfc *data = cfcTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "cfcTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    /* For a data type that has a key, if the same instance is going to be
     written multiple times, initialize the key here
     and register the keyed instance prior to writing */
    /*
        instance_handle = cfc_writer->register_instance(*data);
     */

    // Main loop, write data
    int sample;
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        /* Changes for custom_flowcontroller */
        /* Simulate bursty writer */
        NDDSUtility::sleep(send_period);
        for (int i = 0; i < 10; ++i) {
            sample = samples_written * 10 + i;
            std::cout << "Writing cfc, sample " << sample << std::endl;
            data->x = sample;
            memset(data->str, 1, 999);
            data->str[999] = 0;
            retcode = typed_writer->write(*data, instance_handle);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "write error " << retcode << std::endl;
            }
        }
    }

    /* This new sleep it is for let time to the subscriber to read all the
     * sent samples.
     */
    send_period.sec = 4;
    NDDS_Utility_sleep(&send_period);

    /*
     retcode = typed_writer->unregister_instance(
     *data, instance_handle);
     if (retcode != DDS_RETCODE_OK) {
     std::cerr << "unregister instance error " << retcode << std::endl;
     }
     */

    // Delete previously allocated cfc, including all contained elements
    retcode = cfcTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "cfcTypeSupport::delete_data error " << retcode
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
