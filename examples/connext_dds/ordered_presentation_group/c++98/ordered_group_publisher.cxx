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

#include "ndds/ndds_cpp.h"
#include "ordered_group.h"
#include "ordered_groupSupport.h"
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
    const char *type_name = ordered_groupTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            ordered_groupTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    /* TOPICS */

    DDSTopic *topic1 = participant->create_topic(
            "Topic1",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic1 == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    DDSTopic *topic2 = participant->create_topic(
            "Topic2",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic2 == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    DDSTopic *topic3 = participant->create_topic(
            "Topic3",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic3 == NULL) {
        return shutdown_participant(
                participant,
                "create_topic error",
                EXIT_FAILURE);
    }

    /* DATAWRITERS */

    DDSDataWriter *untyped_writer1 = publisher->create_datawriter(
            topic1,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer1 == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    ordered_groupDataWriter *typed_writer1 =
            ordered_groupDataWriter::narrow(untyped_writer1);
    if (typed_writer1 == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    DDSDataWriter *untyped_writer2 = publisher->create_datawriter(
            topic2,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer2 == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    ordered_groupDataWriter *typed_writer2 =
            ordered_groupDataWriter::narrow(untyped_writer2);
    if (typed_writer2 == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    DDSDataWriter *untyped_writer3 = publisher->create_datawriter(
            topic3,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer3 == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    ordered_groupDataWriter *typed_writer3 =
            ordered_groupDataWriter::narrow(untyped_writer3);
    if (typed_writer3 == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* INSTANCES */

    ordered_group *instance1 = ordered_groupTypeSupport::create_data();

    if (instance1 == NULL) {
        return shutdown_participant(
                participant,
                "ordered_groupTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    ordered_group *instance2 = ordered_groupTypeSupport::create_data();

    if (instance2 == NULL) {
        return shutdown_participant(
                participant,
                "ordered_groupTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    ordered_group *instance3 = ordered_groupTypeSupport::create_data();

    if (instance3 == NULL) {
        return shutdown_participant(
                participant,
                "ordered_groupTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* For a data type that has a key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    /*
    instance_handle = typed_writer->register_instance(*instance);
     */

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        std::cout << "Writing ordered_group, count " << samples_written
                  << std::endl;

        // Modify the data to be sent and Write data
        strcpy(instance1->message,
               "First sample, Topic 1 sent by DataWriter number 1");

        retcode = typed_writer1->write(*instance1, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        strcpy(instance1->message,
               "Second sample, Topic 1 sent by DataWriter number 1");

        retcode = typed_writer1->write(*instance1, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        strcpy(instance2->message,
               "First sample, Topic 2 sent by DataWriter number 2");

        retcode = typed_writer2->write(*instance2, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        strcpy(instance2->message,
               "Second sample, Topic 2 sent by DataWriter number 2");

        retcode = typed_writer2->write(*instance2, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        strcpy(instance3->message,
               "First sample, Topic 3 sent by DataWriter number 3");

        retcode = typed_writer3->write(*instance3, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        strcpy(instance3->message,
               "Second sample, Topic 3 sent by DataWriter number 3");

        retcode = typed_writer3->write(*instance3, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        NDDSUtility::sleep(send_period);
    }

    /*
    retcode = typed_writer->unregister_instance(
     *instance, instance_handle);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "unregister instance error " << retcode << std::endl;
    }
     */

    // Delete data sample
    retcode = ordered_groupTypeSupport::delete_data(instance1);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "ordered_groupTypeSupport::delete_data error " << retcode
                  << std::endl;
    }

    // Delete data sample
    retcode = ordered_groupTypeSupport::delete_data(instance2);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "ordered_groupTypeSupport::delete_data error " << retcode
                  << std::endl;
    }

    // Delete data sample
    retcode = ordered_groupTypeSupport::delete_data(instance3);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "ordered_groupTypeSupport::delete_data error " << retcode
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
