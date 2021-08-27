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

    // This DataWriter writes data on "Example keys" Topic
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

    /* If you want to set the writer_data_lifecycle QoS settings
     * programmatically rather than using the XML, you will need to add
     * the following lines to your code and comment out the create_datawriter
     * call above.
     */

    /*
    DDS_DataWriterQos datawriter_qos;
    retcode = publisher->get_default_datawriter_qos(datawriter_qos);
    if (retcode != DDS_RETCODE_OK) {
    std::cerr << "get_default_datawriter_qos error\n";
    return EXIT_FAILURE;
    }

    datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances =
    DDS_BOOLEAN_FALSE;

    DDSDataWriter *untyped_writer = publisher->create_datawriter(
    topic, datawriter_qos, NULL,
    DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
    return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }
    */

    // Narrow casts from an untyped DataWriter to a writer of your type
    keysDataWriter *typed_writer = keysDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Create data samples for writing
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

    // The keys must have been set before making this call
    std::cout << "Registering instance " << data[0]->code << std::endl;
    handle[0] = typed_writer->register_instance(*data[0]);

    // Modify the data to be sent here
    data[0]->x = 1000;
    data[1]->x = 2000;
    data[2]->x = 3000;

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        NDDSUtility::sleep(send_period);

        switch (samples_written) {
        case 5: { /* Start sending the second and third instances */
            std::cout << "----Registering instance " << data[1]->code
                      << std::endl;
            std::cout << "----Registering instance " << data[2]->code
                      << std::endl;
            handle[1] = typed_writer->register_instance(*data[1]);
            handle[2] = typed_writer->register_instance(*data[2]);
            active[1] = true;
            active[2] = true;
        } break;
        case 10: { /* Unregister the second instance */
            std::cout << "----Unregistering instance " << data[1]->code
                      << std::endl;
            retcode = typed_writer->unregister_instance(*data[1], handle[1]);
            if (retcode != DDS_RETCODE_OK) {
                return shutdown_participant(
                        participant,
                        "unregister instance error",
                        EXIT_FAILURE);
            }
            active[1] = false;
        } break;
        case 15: { /* Dispose the third instance */
            std::cout << "----Disposing instance " << data[2]->code
                      << std::endl;
            retcode = typed_writer->dispose(*data[2], handle[2]);
            if (retcode != DDS_RETCODE_OK) {
                return shutdown_participant(
                        participant,
                        "dispose instance error",
                        EXIT_FAILURE);
            }
            active[2] = false;
        } break;
        }

        // Modify the data to be sent here
        data[0]->y = samples_written;
        data[1]->y = samples_written;
        data[2]->y = samples_written;

        for (int i = 0; i < 3; ++i) {
            if (active[i]) {
                std::cout << "Writing instance " << data[i]->code
                          << ", x: " << data[i]->x << ", y: " << data[i]->y
                          << std::endl;
                retcode = typed_writer->write(*data[i], handle[i]);
                if (retcode != DDS_RETCODE_OK) {
                    return shutdown_participant(
                            participant,
                            "write error",
                            EXIT_FAILURE);
                }
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
