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

#include "keys.h"
#include "keysSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    // Creates three instances
    keys *data[3] = { NULL, NULL, NULL };
    // Creates three handles for managing the registrations
    DDS_InstanceHandle_t handle[3] = { DDS_HANDLE_NIL,
                                       DDS_HANDLE_NIL,
                                       DDS_HANDLE_NIL };
    // We only will send data over the instances marked as active
    bool active[3] = { true, false, false };

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
    const char *type_name = keysTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            keysTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example keys",
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

    // We are going to load different QoS profiles for the two DWs
    DDS_DataWriterQos datawriter_qos;

    // This DataWriter writes data on "Example keys" Topic
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);

    retcode = DDSTheParticipantFactory->get_datawriter_qos_from_profile(
            datawriter_qos,
            "keys_Library",
            "keys_Profile_dw2");
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "get_datawriter_qos_from_profile error",
                EXIT_FAILURE);
    }

    /* If you want to set the writer_data_lifecycle QoS settings
     * programmatically rather than using the XML, you will need to add
     * the following lines to your code and comment out the create_datawriter
     * and get_datawriter_qos_from_profile calls above.
     */

    /*
    retcode = publisher->get_default_datawriter_qos(datawriter_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "get_default_datawriter_qos error",
                EXIT_FAILURE);
    }

    datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances =
    DDS_BOOLEAN_FALSE; datawriter_qos.ownership.kind =
    DDS_EXCLUSIVE_OWNERSHIP_QOS; datawriter_qos.ownership_strength.value = 10;

    DDSDataWriter *untyped_writer1 = publisher->create_datawriter(
        topic, datawriter_qos, NULL,
        DDS_STATUS_MASK_NONE);

    datawriter_qos.ownership_strength.value = 5;
    */

    // This DataWriter writes data on "Example keys" Topic
    DDSDataWriter *untyped_writer2 = publisher->create_datawriter(
            topic,
            datawriter_qos,
            NULL,
            DDS_STATUS_MASK_NONE);

    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter1 error",
                EXIT_FAILURE);
    }

    if (untyped_writer2 == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter2 error",
                EXIT_FAILURE);
    }

    // Narrow casts from an untyped DataWriter to a writer of your type
    keysDataWriter *typed_writer = keysDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    keysDataWriter *typed_writer2 = keysDataWriter::narrow(untyped_writer2);
    if (typed_writer2 == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Create data samples for writing */
    data[0] = keysTypeSupport::create_data();
    data[1] = keysTypeSupport::create_data();
    data[2] = keysTypeSupport::create_data();

    if (data[0] == NULL || data[1] == NULL || data[2] == NULL) {
        return shutdown_participant(
                participant,
                "keysTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* RTI Connext could examine the key fields each time it needs to determine
     * which data-instance is being modified.
     * However, for performance and semantic reasons, it is better
     * for your application to declare all the data-instances it intends to
     * modify prior to actually writing any samples. This is known as
     * registration.
     */

    /* In order to register the instances, we must set their associated keys
     * first */
    data[0]->code = 0;
    data[1]->code = 1;
    data[2]->code = 2;

    /* The keys must have been set before making this call */
    std::cout << "----DW1 registering instance " << data[0]->code << std::endl;
    handle[0] = typed_writer->register_instance(*data[0]);

    /* Modify the data to be sent here */
    data[0]->x = 1;
    data[1]->x = 1;
    data[2]->x = 1;

    /* Make variables for the instance for the second datawriter to use.
    Note that it actually refers to the same logical instance, but
    because we're running both datawriters in the same thread, we
    to create separate variables so they don't clobber each other.
    */
    keys *instance_dw2 = NULL;
    instance_dw2 = keysTypeSupport::create_data();
    if (instance_dw2 == NULL) {
        return shutdown_participant(
                participant,
                "keysTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* instance_dw2 and instance[1] have the same key, and thus
    will write to the same instance (ins1).
    */
    instance_dw2->code = data[1]->code;
    instance_dw2->x = 2;
    DDS_InstanceHandle_t handle_dw2 =
            typed_writer2->register_instance(*instance_dw2);
    bool active_dw2 = true;

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        NDDSUtility::sleep(send_period);

        // Modify the data to be sent here
        data[0]->y = samples_written;
        data[1]->y = samples_written + 1000;
        data[2]->y = samples_written + 2000;

        instance_dw2->y = -samples_written - 1000;

        /* We control two datawriters via a state machine here rather than
        introducing separate threads.
        */

        // Control first DataWriter
        switch (samples_written) {
        case 4: {
            // Start sending the second (ins1) and third instances (ins2)
            std::cout << "----DW1 registering instance " << data[1]->code
                      << std::endl;
            std::cout << "----DW1 registering instance " << data[2]->code
                      << std::endl;
            handle[1] = typed_writer->register_instance(*data[1]);
            handle[2] = typed_writer->register_instance(*data[2]);
            active[1] = true;
            active[2] = true;
        } break;
        case 8: { /* Dispose the second instance (ins1) */
            std::cout << "----DW1 disposing instance " << data[1]->code
                      << std::endl;
            retcode = typed_writer->dispose(*data[1], handle[1]);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "dispose instance error" << retcode << std::endl;
                return EXIT_FAILURE;
            }
            active[1] = false;
        } break;
        case 10: {
            // Unregister the second instance (ins1)
            std::cout << "----DW1 unregistering instance " << data[1]->code
                      << std::endl;
            retcode = typed_writer->unregister_instance(*data[1], handle[1]);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "unregister instance error " << retcode
                          << std::endl;
                return EXIT_FAILURE;
            }
            active[1] = false;
        } break;
        case 12: {
            // Unregister the third instance (ins2)
            std::cout << "----DW1 unregistering instance " << data[2]->code
                      << std::endl;
            retcode = typed_writer->unregister_instance(*data[2], handle[2]);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "unregister instance error " << retcode
                          << std::endl;
                return EXIT_FAILURE;
            }
            active[2] = false;

            // Re-register the second instance (ins1)
            std::cout << "----DW1 re-registering instance " << data[1]->code
                      << std::endl;
            handle[1] = typed_writer->register_instance(*data[1]);
            active[1] = true;
        } break;
        case 16: {
            // Re-register the third instance (ins2)
            std::cout << "----DW1 re-registering instance " << data[2]->code
                      << std::endl;
            handle[2] = typed_writer->register_instance(*data[2]);
            active[2] = true;
        } break;
        }

        for (int i = 0; i < 3; ++i) {
            if (active[i]) {
                std::cout << "DW1 write; code: " << data[i]->code
                          << ", x: " << data[i]->x << ", y: " << data[i]->y
                          << std::endl;

                retcode = typed_writer->write(*data[i], handle[i]);
                if (retcode != DDS_RETCODE_OK) {
                    std::cerr << "write error " << retcode << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }

        // Control second datawriter
        switch (samples_written) {
        case 16: { /* Dispose the instance (ins1).
                   Since it has lower ownership strength, this does nothing */
            std::cout << "----DW2 disposing instance " << instance_dw2->code
                      << std::endl;
            retcode = typed_writer2->dispose(*instance_dw2, handle_dw2);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "DW2 dispose instance error " << retcode
                          << std::endl;
            }
            active_dw2 = false;
        } break;
        }

        if (active_dw2) {
            std::cout << "DW2 write; code: " << instance_dw2->code
                      << ", x: " << instance_dw2->x
                      << ", y: " << instance_dw2->y << std::endl;

            retcode = typed_writer2->write(*instance_dw2, handle_dw2);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "write error " << retcode << std::endl;
                return EXIT_FAILURE;
            }
        }
    }

    // Delete data samples
    for (int i = 0; i < 3; ++i) {
        retcode = keysTypeSupport::delete_data(data[i]);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "keysTypeSupport::delete_data error " << retcode
                      << std::endl;
        }
    }

    retcode = keysTypeSupport::delete_data(instance_dw2);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "keysTypeSupport::delete_data error " << retcode
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
