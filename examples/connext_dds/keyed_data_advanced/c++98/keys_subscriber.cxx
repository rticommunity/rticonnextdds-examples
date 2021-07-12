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

enum Instance_State { inactive, active, no_writers, disposed };

void new_instance_found(
        Instance_State *states,
        const DDS_SampleInfo *info,
        const keys *msg)
{
    /* There are three cases here:
    1.) truly new instance
    2.) instance lost all writers, but now we're getting data again
    3.) instance was disposed, but a new one has been created

    We distinguish these cases by examining generation counts, BUT
    note that if the instance resources have been reclaimed, the
    generation counts may be reset to 0.

    Instances are eligible for resource cleanup if there are no
    active writers and all samples have been taken.  To reliably
    determine which case a 'new' instance falls into, the application
    must store state information on a per-instance basis.

    Note that this example assumes that state changes only occur via
    explicit register_instance(), unregister_instance() and dispose()
    calls from the datawriter.  In reality, these changes could also
    occur due to lost liveliness or missed deadlines, so those
    listeners would also need to update the instance state.
    */

    switch (states[msg->code]) {
    case inactive:
        std::cout << "New instance found; code = " << msg->code << std::endl;
        break;
    case active:
        // An active instance should never be interpreted as new
        std::cout << "Error, 'new' already-active instance found; code = "
                  << msg->code << std::endl;
        break;
    case no_writers:
        std::cout << "Found writer for instance; code = " << msg->code
                  << std::endl;
        break;
    case disposed:
        std::cout << "Found reborn instance; code = " << msg->code << std::endl;
        break;
    }
    states[msg->code] = active;
}

void instance_lost_writers(
        Instance_State *states,
        const DDS_SampleInfo *info,
        const keys *msg)
{
    std::cout << "Instance has no writers; code = " << msg->code << std::endl;
    states[msg->code] = no_writers;
}

void instance_disposed(
        Instance_State *states,
        const DDS_SampleInfo *info,
        const keys *msg)
{
    std::cout << "Instance disposed; code = " << msg->code << std::endl;
    states[msg->code] = disposed;
}

/* Called to handle relevant data samples */
void handle_data(const DDS_SampleInfo *info, const keys *msg)
{
    std::cout << "code: " << msg->code << ", x: " << msg->x << ", y: " << msg->y
              << std::endl;
}

bool key_is_relevant(const keys *msg)
{
    /* For this example we just care about codes > 0,
    which are the ones related to instances ins1 and ins2 .*/
    return (msg->code > 0);
}

unsigned int process_data(keysDataReader *typed_reader, Instance_State *states)
{
    keysSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    unsigned int samples_read = 0;
    DDS_ReturnCode_t retcode;

    while (true) {
        /* Given DDS_HANDLE_NIL as a parameter, take_next_instance returns
        a sequence containing samples from only the next (in a well-determined
        but unspecified order) un-taken instance.
        */
        retcode = typed_reader->take_next_instance(
                data_seq,
                info_seq,
                DDS_LENGTH_UNLIMITED,
                DDS_HANDLE_NIL,
                DDS_ANY_SAMPLE_STATE,
                DDS_ANY_VIEW_STATE,
                DDS_ANY_INSTANCE_STATE);

        if (retcode == DDS_RETCODE_NO_DATA) {
            break;
        } else if (retcode != DDS_RETCODE_OK) {
            std::cerr << "read error " << retcode << std::endl;
            break;
        }

        /* We process all the obtained samples for a particular instance */
        for (int i = 0; i < data_seq.length(); ++i) {
            /* We first check if the sample includes valid data */
            if (info_seq[i].valid_data) {
                if (info_seq[i].view_state == DDS_NEW_VIEW_STATE) {
                    new_instance_found(states, &info_seq[i], &data_seq[i]);
                }

                /* We check if the obtained samples are associated to one
                of the instances of interest.
                Since take_next_instance gives sequences of the same instance,
                we only need to test this for the first sample obtained.
                */
                if (i == 0 && !key_is_relevant(&data_seq[i])) {
                    break;
                }

                handle_data(&info_seq[i], &data_seq[i]);

                samples_read++;
            } else {
                /* Since there is not valid data, it may include metadata */
                keys dummy;
                retcode = typed_reader->get_key_value(
                        dummy,
                        info_seq[i].instance_handle);
                if (retcode != DDS_RETCODE_OK) {
                    std::cerr << "get_key_value error " << retcode << std::endl;
                    continue;
                }

                /* Here we print a message and change the instance state
                   if the instance state is ALIVE_NO_WRITERS or ALIVE_DISPOSED
                 */
                if (info_seq[i].instance_state
                    == DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE) {
                    instance_lost_writers(states, &info_seq[i], &dummy);
                } else if (
                        info_seq[i].instance_state
                        == DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE) {
                    instance_disposed(states, &info_seq[i], &dummy);
                }
            }
        }

        /* Prepare sequences for next take_next_instance */
        retcode = typed_reader->return_loan(data_seq, info_seq);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "return loan error " << retcode << std::endl;
        }
        data_seq.maximum(0);
        info_seq.maximum(0);
    }

    return samples_read;
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    DDS_Duration_t wait_timeout = { 1, 0 };
    Instance_State states[3] = { inactive, inactive, inactive };

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

    // A Subscriber allows an application to create one or more DataReaders
    DDSSubscriber *subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        return shutdown_participant(
                participant,
                "create_subscriber error",
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

    // This DataReader reads data on "Example keys" Topic
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_ALL);

    /* If you want to set the writer_data_lifecycle QoS settings
     * programmatically rather than using the XML, you will need to add
     * the following lines to your code and comment out the create_datareader
     * call above.
     */
    /*
    DDS_DataReaderQos datareader_qos;
    retcode = subscriber->get_default_datareader_qos(datareader_qos);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "get_default_datareader_qos error\n";
        return EXIT_FAILURE;
    }

    datareader_qos.ownership.kind = DDS_EXCLUSIVE_OWNERSHIP_QOS;

    DDSDataReader *untyped_reader = subscriber->create_datareader(
        topic, datareader_qos, reader_listener,
        DDS_STATUS_MASK_ALL);
    */

    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    // Narrow casts from a untyped DataReader to a reader of your type
    keysDataReader *typed_reader = keysDataReader::narrow(untyped_reader);
    if (typed_reader == NULL) {
        return shutdown_participant(
                participant,
                "DataReader narrow error",
                EXIT_FAILURE);
    }

    // Create ReadCondition that triggers when unread data in reader's queue
    DDSReadCondition *read_condition = typed_reader->create_readcondition(
            DDS_NOT_READ_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);
    if (read_condition == NULL) {
        return shutdown_participant(
                participant,
                "create_readcondition error",
                EXIT_FAILURE);
    }

    // WaitSet will be woken when the attached condition is triggered
    DDSWaitSet waitset;
    retcode = waitset.attach_condition(read_condition);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "attach_condition error",
                EXIT_FAILURE);
    }

    // Main loop. Wait for data to arrive, and process when it arrives
    unsigned int samples_read = 0;
    while (!shutdown_requested && samples_read < sample_count) {
        DDSConditionSeq active_conditions_seq;

        // Wait for data
        retcode = waitset.wait(active_conditions_seq, wait_timeout);

        if (retcode == DDS_RETCODE_OK) {
            // If the read condition is triggered, process data
            samples_read += process_data(typed_reader, states);
        }
    }

    // Cleanup
    return shutdown_participant(participant, "Shutting down", 0);
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
            std::cerr << "delete_contained_entities error" << retcode
                      << std::endl;
            status = EXIT_FAILURE;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "delete_participant error" << retcode << std::endl;
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

    int status = run_subscriber_application(
            arguments.domain_id,
            arguments.sample_count);

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    DDS_ReturnCode_t retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "finalize_instance error" << retcode << std::endl;
        status = EXIT_FAILURE;
    }

    return status;
}
