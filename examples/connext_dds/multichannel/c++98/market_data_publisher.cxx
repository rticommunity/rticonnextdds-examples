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

#include "market_data.h"
#include "market_dataSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    DDS_Duration_t send_period = { 0, 100000000 };

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

    // Register the datatype to use when creating the Topic
    const char *type_name = market_dataTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            market_dataTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example market_data",
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

    /* If you want to change the DataWriter's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and modify the datawriter creation fuction using writer_qos.
     *
     * In this case, we set the publish as multichannel using the differents
     * channel to send differents symbol. Every channel have a IP to send the
     * data.
     */

    /* Start changes for MultiChannel */
    /*
     retcode = publisher->get_default_datawriter_qos(writer_qos);
     if (retcode != DDS_RETCODE_OK) {
     return shutdown_participant(
                participant,
                "get_default_datawriter_qos error",
                EXIT_FAILURE);
     }
     */
    /* Create 8 channels based on Symbol */
    /*
        writer_qos.multi_channel.channels.ensure_length(8, 8);
        writer_qos.multi_channel.channels[0].filter_expression = DDS_String_dup(
                "Symbol MATCH '[A-C]*'");
        writer_qos.multi_channel.channels[0].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[0].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.2");
        writer_qos.multi_channel.channels[1].filter_expression = DDS_String_dup(
                "Symbol MATCH '[D-F]*'");
        writer_qos.multi_channel.channels[1].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[1].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.3");
        writer_qos.multi_channel.channels[2].filter_expression = DDS_String_dup(
                "Symbol MATCH '[G-I]*'");
        writer_qos.multi_channel.channels[2].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[2].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.4");
        writer_qos.multi_channel.channels[3].filter_expression = DDS_String_dup(
                "Symbol MATCH '[J-L]*'");
        writer_qos.multi_channel.channels[3].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[3].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.5");
        writer_qos.multi_channel.channels[4].filter_expression = DDS_String_dup(
                "Symbol MATCH '[M-O]*'");
        writer_qos.multi_channel.channels[4].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[4].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.6");
        writer_qos.multi_channel.channels[5].filter_expression = DDS_String_dup(
                "Symbol MATCH '[P-S]*'");
        writer_qos.multi_channel.channels[5].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[5].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.7");
        writer_qos.multi_channel.channels[6].filter_expression = DDS_String_dup(
                "Symbol MATCH '[T-V]*'");
        writer_qos.multi_channel.channels[6].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[6].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.8");
        writer_qos.multi_channel.channels[7].filter_expression = DDS_String_dup(
                "Symbol MATCH '[W-Z]*'");
        writer_qos.multi_channel.channels[7].multicast_settings.ensure_length(1,
       1);
        writer_qos.multi_channel.channels[7].multicast_settings[0].receive_address
       = DDS_String_dup("239.255.0.9");
    */

    // This DataWriter writes data on "Example prueba" Topic
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            /*writer_qos*/ DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }
    /* End changes for MultiChannel */

    market_dataDataWriter *typed_writer =
            market_dataDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Create data for writing, allocating all members
    market_data *data = market_dataTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "market_dataTypeSupport::create_data error",
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
        // Changes for MultiChannel
        // Modify the data to be sent here
        sprintf(data->Symbol, "%c", 'A' + (samples_written % 26));
        data->Price = samples_written;

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
     std::cerr << "unregister instance error " << retcode << std::endl;
     }
     */

    // Delete previously allocated prueba, including all contained elements
    retcode = market_dataTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "market_dataTypeSupport::delete_data error " << retcode
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
