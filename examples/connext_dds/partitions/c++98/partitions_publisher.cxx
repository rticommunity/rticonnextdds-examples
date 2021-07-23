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

#include "partitions.h"
#include "partitionsSupport.h"
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

    DDS_PublisherQos publisher_qos;
    DDS_ReturnCode_t retcode =
            participant->get_default_publisher_qos(publisher_qos);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "get_default_publisher_qos error",
                EXIT_FAILURE);
    }

    /* If you want to change the Partition name programmatically rather than
     * using the XML, you will need to add the following lines to your code
     * and comment out the create_publisher() call bellow.
     */
    publisher_qos.partition.name.ensure_length(2, 2);
    strcpy(publisher_qos.partition.name[0], "ABC");
    strcpy(publisher_qos.partition.name[1], "foo");

    std::cout << "Setting partition to '" << publisher_qos.partition.name[0]
              << "', '" << publisher_qos.partition.name[1] << "'...\n";

    // A Publisher allows an application to create one or more DataWriters
    DDSPublisher *publisher = participant->create_publisher(
            publisher_qos,
            NULL,
            DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        return shutdown_participant(
                participant,
                "create_publisher error",
                EXIT_FAILURE);
    }

    /*
    DDSPublisher *publisher = participant->create_publisher(
        DDS_PUBLISHER_QOS_DEFAULT,
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
    const char *type_name = partitionsTypeSupport::get_type_name();
    retcode = partitionsTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example partitions",
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


    /* In this example we set a Reliable datawriter, with Transient Local
     * durability. By default we set up these QoS settings via XML. If you
     * want to to it programmatically, use the following code, and comment out
     * the create_datawriter call bellow.
     */
    /*
    DDS_DataWriterQos datawriter_qos;
    retcode = publisher->get_default_datawriter_qos(datawriter_qos);
    if (retcode != DDS_RETCODE_OK) {
      return shutdown_participant(
                participant,
                "get_default_datawriter_qos error",
                EXIT_FAILURE);
    }
    datawriter_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
    datawriter_qos.durability.kind = DDS_TRANSIENT_LOCAL_DURABILITY_QOS;
    datawriter_qos.history.kind = DDS_KEEP_LAST_HISTORY_QOS;
    datawriter_qos.history.depth = 3;

    DDSDataWriter *untyped_writer = publisher->create_datawriter(
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

    // This DataWriter writes data on "Example partitions" Topic
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_NONE);
    if (untyped_writer == NULL) {
        return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
    }

    partitionsDataWriter *typed_writer =
            partitionsDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Create data for writing, allocating all members
    partitions *data = partitionsTypeSupport::create_data();
    if (data == NULL) {
        return shutdown_participant(
                participant,
                "partitionsTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* For a data type that has a key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    /*
        instance_handle = typed_writer->register_instance(*data);
    */

    /// Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        std::cout << "Writing partitions, count " << samples_written
                  << std::endl;
        data->x = samples_written;

        retcode = typed_writer->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        /* Every 5 samples we will change the Partition name. These are the
         * partition expressions we are going to try:
         * "bar", "A*", "A?C", "X*Z", "zzz", "A*C"
         */
        if ((samples_written + 1) % 25 == 0) {
            // Matches "ABC" -- name[1] here can match name[0] there,
            // as long as there is some overlapping name
            strcpy(publisher_qos.partition.name[0], "zzz");
            strcpy(publisher_qos.partition.name[1], "A*C");
            std::cout << "Setting partition to '"
                      << publisher_qos.partition.name[0] << "', '"
                      << publisher_qos.partition.name[1] << "'...\n";
            publisher->set_qos(publisher_qos);
        } else if ((samples_written + 1) % 20 == 0) {
            // Strings that are regular expressions aren't tested for
            // literal matches, so this won't match "X*Z"
            strcpy(publisher_qos.partition.name[0], "X*Z");
            std::cout << "Setting partition to '"
                      << publisher_qos.partition.name[0] << "', '"
                      << publisher_qos.partition.name[1] << "'...\n";
            publisher->set_qos(publisher_qos);
        } else if ((samples_written + 1) % 15 == 0) {
            // Matches "ABC"
            strcpy(publisher_qos.partition.name[0], "A?C");
            std::cout << "Setting partition to '"
                      << publisher_qos.partition.name[0] << "', '"
                      << publisher_qos.partition.name[1] << "'...\n";
            publisher->set_qos(publisher_qos);
        } else if ((samples_written + 1) % 10 == 0) {
            // Matches "ABC"
            strcpy(publisher_qos.partition.name[0], "A*");
            std::cout << "Setting partition to '"
                      << publisher_qos.partition.name[0] << "', '"
                      << publisher_qos.partition.name[1] << "'...\n";
            publisher->set_qos(publisher_qos);
        } else if ((samples_written + 1) % 5 == 0) {
            // No literal match for "bar"
            strcpy(publisher_qos.partition.name[0], "bar");
            std::cout << "Setting partition to '"
                      << publisher_qos.partition.name[0] << "', '"
                      << publisher_qos.partition.name[1] << "'...\n";
            publisher->set_qos(publisher_qos);
        }

        NDDSUtility::sleep(send_period);
    }

    /*
        retcode = partitions_writer->unregister_instance(
            *data, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "unregister instance error " << retcode << std::endl;
        }
    */

    /* Delete data sample */
    retcode = partitionsTypeSupport::delete_data(data);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "partitionsTypeSupport::delete_data error " << retcode
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
