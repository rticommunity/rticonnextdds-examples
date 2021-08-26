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

#include "waitset_statuscond.h"
#include "waitset_statuscondSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

/* We don't need to use listeners as we are going to use waitset_statuscond and
 * Conditions so we have removed the auto generated code for listeners here
 */

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    struct DDS_Duration_t wait_timeout = { 1, 500000000 };

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
    const char *type_name = waitset_statuscondTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode = waitset_statuscondTypeSupport::register_type(
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
            "Example waitset_statuscond",
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

    // This DataReader reads data on "Example waitset_statuscond" Topic
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_NONE);
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }


    /* Get status conditions
     * ---------------------
     * Each entity may have an attached Status Condition. To modify the
     * statuses we need to get the reader's Status Conditions first.
     */
    DDSStatusCondition *status_condition =
            untyped_reader->get_statuscondition();
    if (status_condition == NULL) {
        return shutdown_participant(
                participant,
                "get_statuscondition error",
                EXIT_FAILURE);
    }

    /* Set enabled statuses
     * --------------------
     * Now that we have the Status Condition, we are going to enable the
     * status we are interested in: knowing that data is available
     */
    retcode = status_condition->set_enabled_statuses(DDS_DATA_AVAILABLE_STATUS);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "set_enabled_statuses error",
                EXIT_FAILURE);
    }

    /* Create and attach conditions to the WaitSet
     * -------------------------------------------
     * Finally, we create the WaitSet and attach both the Read Conditions
     * and the Status Condition to it.
     */
    DDSWaitSet *waitset = new DDSWaitSet();


    // Attach Status Conditions
    retcode = waitset->attach_condition(status_condition);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "attach_condition error",
                EXIT_FAILURE);
    }

    // Narrow the reader into your specific data type
    waitset_statuscondDataReader *typed_reader =
            waitset_statuscondDataReader::narrow(untyped_reader);
    if (typed_reader == NULL) {
        return shutdown_participant(
                participant,
                "DataReader narrow error",
                EXIT_FAILURE);
    }

    // Main loop. Wait for data to arrive, and process when it arrives
    unsigned int samples_read = 0;
    while (!shutdown_requested && samples_read < sample_count) {
        DDSConditionSeq active_conditions_seq;

        /* wait() blocks execution of the thread until one or more attached
         * Conditions become true, or until a user-specified timeout expires.
         */
        retcode = waitset->wait(active_conditions_seq, wait_timeout);
        // We get to timeout if no conditions were triggered
        if (retcode == DDS_RETCODE_TIMEOUT) {
            std::cout << "Wait timed out!! No conditions were triggered.\n";
            continue;
        } else if (retcode != DDS_RETCODE_OK) {
            std::cerr << "wait returned error: " << retcode << std::endl;
            break;
        }

        // Get the number of active conditions
        int active_conditions = active_conditions_seq.length();
        std::cout << "Got " << active_conditions << " active conditions\n";

        /* In this case, we have only a single condition, but
            if we had multiple, we would need to iterate over
            them and check which one is true.  Leaving the logic
            for the more complex case. */
        for (int i = 0; i < active_conditions; ++i) {
            // Compare with Status Conditions
            if (active_conditions_seq[i] == status_condition) {
                /* Get the status changes so we can check which status
                 * condition triggered. */
                DDS_StatusMask triggeredmask =
                        typed_reader->get_status_changes();

                // Subscription matched
                if (triggeredmask & DDS_DATA_AVAILABLE_STATUS) {
                    /* Current conditions match our conditions to read data, so
                     * we can read data just like we would do in any other
                     * example. */
                    waitset_statuscondSeq data_seq;
                    DDS_SampleInfoSeq info_seq;

                    /* Access data using read(), take(), etc.  If you fail to do
                     * this the condition will remain true, and the WaitSet will
                     * wake up immediately - causing high CPU usage when it does
                     * not sleep in the loop */
                    retcode = DDS_RETCODE_OK;
                    while (retcode != DDS_RETCODE_NO_DATA) {
                        retcode = typed_reader->take(
                                data_seq,
                                info_seq,
                                DDS_LENGTH_UNLIMITED,
                                DDS_ANY_SAMPLE_STATE,
                                DDS_ANY_VIEW_STATE,
                                DDS_ANY_INSTANCE_STATE);

                        for (int j = 0; j < data_seq.length(); ++j) {
                            if (!info_seq[j].valid_data) {
                                std::cout << "Got metadata\n";
                                continue;
                            }
                            waitset_statuscondTypeSupport::print_data(
                                    &data_seq[j]);
                            samples_read++;
                        }
                        /* Return the loaned data */
                        typed_reader->return_loan(data_seq, info_seq);
                    }
                }
            }
        }
    }

    // Delete all entities
    delete waitset;

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
