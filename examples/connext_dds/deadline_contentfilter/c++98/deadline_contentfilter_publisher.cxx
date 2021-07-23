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

/* This example sets the deadline period to 1.5 seconds to trigger a deadline
   if the DataWriter does not update often enough.  The writer's updates are
   dependent on the application code, so the middleware can notify the
   application that it has failed to update often enough.

*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "deadline_contentfilter.h"
#include "deadline_contentfilterSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

/* Start changes for Deadline */
class deadline_contentfilterListener : public DDSDataWriterListener {
public:
    virtual void on_offered_deadline_missed(
            DDSDataWriter *writer,
            const DDS_OfferedDeadlineMissedStatus &status)
    {
        deadline_contentfilter dummy;
        DDS_ReturnCode_t retcode =
                ((deadline_contentfilterDataWriter *) writer)
                        ->get_key_value(dummy, status.last_instance_handle);

        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "get_key_value error " << retcode << std::endl;
            return;
        }

        std::cout << "Offered deadline missed on instance code = " << dummy.code
                  << std::endl;
    }
};

/* End changes for Deadline */

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
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
    const char *type_name = deadline_contentfilterTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode = deadline_contentfilterTypeSupport::register_type(
            participant,
            type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example deadline_contentfilter",
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

    /* Start changes for Deadline */
    // Create listener
    deadline_contentfilterListener *writer_listener = NULL;
    writer_listener = new deadline_contentfilterListener();
    if (writer_listener == NULL) {
        return shutdown_participant(
                participant,
                "listener instantiation error",
                EXIT_FAILURE);
    }

    // This DataWriter writes data on "Example deadline_contentfilter" Topic
    DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic,
            DDS_DATAWRITER_QOS_DEFAULT,
            writer_listener,
            DDS_OFFERED_DEADLINE_MISSED_STATUS);
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
     * In this case, we set the deadline period to 1.5 seconds to trigger
     * a deadline if the DataWriter does not update often enough.
     */
    /*
DDS_DataWriterQos datawriter_qos;
retcode = publisher->get_default_datawriter_qos(datawriter_qos);
if (retcode != DDS_RETCODE_OK) {
    std::cerr << "get_default_datawriter_qos error\n";
    return EXIT_FAILURE;
}

*/ /* Set deadline QoS */
    /*    DDS_Duration_t deadline_period = {1, 500000000};
        datawriter_qos.deadline.period = deadline_period;

        DDSDataWriter *untyped_writer = publisher->create_datawriter(
            topic, datawriter_qos, writer_listener,
            DDS_OFFERED_DEADLINE_MISSED_STATUS);
        if (untyped_writer == NULL) {
            return shutdown_participant(
                participant,
                "create_datawriter error",
                EXIT_FAILURE);
        }
            */

    /* End changes for Deadline */

    deadline_contentfilterDataWriter *typed_writer =
            deadline_contentfilterDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Start changes for Deadline */
    /* Create two instances for writing */

    deadline_contentfilter *instance0 = NULL;
    deadline_contentfilter *instance1 = NULL;
    DDS_InstanceHandle_t handle0 = DDS_HANDLE_NIL;
    DDS_InstanceHandle_t handle1 = DDS_HANDLE_NIL;

    /* Create data samples for writing */
    instance0 = deadline_contentfilterTypeSupport::create_data();
    instance1 = deadline_contentfilterTypeSupport::create_data();
    if (instance0 == NULL || instance1 == NULL) {
        return shutdown_participant(
                participant,
                "deadline_contentfilterTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* Set keys -- we specify 'code' as the key field in the .idl*/
    instance0->code = 0;
    instance1->code = 1;

    /* For data type that has key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */

    handle0 = typed_writer->register_instance(*instance0);
    handle1 = typed_writer->register_instance(*instance1);

    struct DDS_Duration_t send_period = { 1, 0 };

    instance0->x = instance0->y = instance1->x = instance1->y = 0;

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        NDDSUtility::sleep(send_period);

        instance0->x++;
        instance0->y++;
        instance1->x++;
        instance1->y++;

        std::cout << "Writing instance0, x = " << instance0->x
                  << ", y = " << instance0->y << std::endl;
        retcode = typed_writer->write(*instance0, handle0);
        if (retcode != DDS_RETCODE_OK) {
            return shutdown_participant(
                    participant,
                    "write error",
                    EXIT_FAILURE);
        }

        if (samples_written < 15) {
            std::cout << "Writing instance1, x = " << instance1->x
                      << ", y = " << instance1->y << std::endl;
            retcode = typed_writer->write(*instance1, handle1);
            if (retcode != DDS_RETCODE_OK) {
                return shutdown_participant(
                        participant,
                        "write error",
                        EXIT_FAILURE);
            }
        } else if (samples_written == 15) {
            std::cout << "Stopping writes to instance1\n";
        }
    }

    retcode = typed_writer->unregister_instance(*instance0, handle0);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "unregister instance error " << retcode << std::endl;
    }
    retcode = typed_writer->unregister_instance(*instance1, handle1);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "unregister instance error " << retcode << std::endl;
    }

    // deadline_contentfilterTypeSupport

    // Delete previously allocated data, including all contained elements
    retcode = deadline_contentfilterTypeSupport::delete_data(instance0);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "deadline_contentfilterTypeSupport::delete_data error "
                  << retcode << std::endl;
    }
    retcode = deadline_contentfilterTypeSupport::delete_data(instance1);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "deadline_contentfilterTypeSupport::delete_data error "
                  << retcode << std::endl;
    }

    delete writer_listener;

    /* End changes for Deadline */

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
