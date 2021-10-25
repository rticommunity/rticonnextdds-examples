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

#include "Shapes.h"
#include "ShapesSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    DDS_Duration_t send_period = { 0, 100000000 }; /* 100 ms */
    /*** Shape direction variables ***/
    int direction = 1;   /* 1 means left to right and -1, right to left */
    int x_position = 50; /* 50 is the initial position */

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

    /* Create DynamicData using TypeCode from Shapes.cxx
     * If you are NOT using a type generated with rtiddsgen, you
     * need to create this TypeCode from scratch.
     */
    struct DDS_TypeCode *type_code = ShapeType_get_typecode();
    if (type_code == NULL) {
        return shutdown_participant(
                participant,
                "get_typecode error",
                EXIT_FAILURE);
    }

    // Create the Dynamic data type support object
    struct DDS_DynamicDataTypeProperty_t props;
    DDSDynamicDataTypeSupport *type_support =
            new DDSDynamicDataTypeSupport(type_code, props);
    if (type_support == NULL) {
        return shutdown_participant(
                participant,
                "constructor DynamicDataTypeSupport error",
                EXIT_FAILURE);
    }

    // Register the datatype to use when creating the Topic
    const char *type_name = ShapeTypeTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            type_support->register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    /* Make sure both publisher and subscriber share the same topic
     * name.
     * In the Shapes example: we are publishing a Square, which is the
     * topic name. If you want to publish other shapes (Triangle or
     * Circle), you just need to update the topic name. */
    DDSTopic *topic = participant->create_topic(
            "Square",
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

    // This DataWriter writes data on "Example batch_data" Topic
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

    /* Then, to use DynamicData, we need to assign the generic
     * DataWriter to a DynamicDataWriter, using
     * DDS_DynamicDataWriter_narrow.
     * The following narrow function should never fail, as it performs
     * only a safe cast.
     */
    DDSDynamicDataWriter *typed_writer =
            DDSDynamicDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Create an instance of the sparse data we are about to send */
    DDS_DynamicData *data = type_support->create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "create_data error",
                EXIT_FAILURE);
    }

    /* Initialize the DynamicData object */
    data->set_string("color", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, "BLUE");
    data->set_long("x", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 100);
    data->set_long("y", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 100);
    data->set_long("shapesize", DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 30);

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        std::cout << "Sending shapesize " << 30 + (samples_written % 20)
                  << std::endl;
        std::cout << "Sending x position " << x_position << std::endl;

        /* Modify the shapesize from 30 to 50 */
        retcode = data->set_long(
                "shapesize",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
                30 + (samples_written % 20));
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                    participant,
                    "! Unable to set shapesize",
                    EXIT_FAILURE);
        }

        /* Modify the position */
        retcode = data->set_long(
                "x",
                DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
                x_position);
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                    participant,
                    "! Unable to set x",
                    EXIT_FAILURE);
        }

        /* The x_position will be modified adding or substracting
         * 2 to the previous x_position depending on the direction.
         */
        x_position += (direction * 2);
        /* The x_position will stay between 50 and 150 pixels.
         * When the position is greater than 150 'direction' will be negative
         * (moving to the left) and when it is lower than 50 'direction' will
         * be possitive (moving to the right).
         */
        if (x_position >= 150) {
            direction = -1;
        }
        if (x_position <= 50) {
            direction = 1;
        }

        /* Write data */
        retcode = typed_writer->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                    participant,
                    "write error",
                    EXIT_FAILURE);
        }

        NDDSUtility::sleep(send_period);
    }

    // Delete previously allocated ShapeType, including all contained elements
    retcode = type_support->delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "ShapeTypeTypeSupport::delete_data error " << retcode
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
