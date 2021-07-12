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

#include "ordered.h"
#include "orderedSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

#define MAX_SUBSCRIBERS 2

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

/* Start changes for Ordered_Presentation */

/* No listener is needed; we poll readers in this function */
unsigned int poll_data(orderedDataReader *typed_reader[], int numreaders)
{
    DDS_ReturnCode_t retcode = DDS_RETCODE_OK;
    unsigned int samples_read = 0;
    for (int r = 0; r < numreaders; ++r) {
        DDS_SampleInfoSeq info_seq;
        orderedSeq data_seq;
        DDS_ReturnCode_t retcode = typed_reader[r]->take(
                data_seq,
                info_seq,
                DDS_LENGTH_UNLIMITED,
                DDS_ANY_SAMPLE_STATE,
                DDS_ANY_VIEW_STATE,
                DDS_ANY_INSTANCE_STATE);
        if (retcode == DDS_RETCODE_NO_DATA) {
            // Not an error
            continue;
        } else if (retcode != DDS_RETCODE_OK) {
            // Is an error
            std::cerr << "take error: " << retcode << std::endl;
            return 0;
        }

        for (int i = 0; i < data_seq.length(); ++i) {
            if (!info_seq[i].valid_data)
                continue;
            // Make things a bit easier to read.
            samples_read++;
            int ident = r;
            while (ident--) {
                std::cout << "\t";
            }
            std::cout << "Reader " << r << ": Instance" << data_seq[i].id
                      << "->value = " << data_seq[i].value << std::endl;
        }

        retcode = typed_reader[r]->return_loan(data_seq, info_seq);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "return loan error " << retcode << std::endl;
        }
    }

    return samples_read;
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    DDSSubscriber *subscriber[2] = { 0 };
    DDSDataReader *untyped_reader[2] = { 0 };
    DDS_Duration_t receive_period = { 4, 0 };
    char *profile_name[] = { (char *) "ordered_Profile_subscriber_instance",
                             (char *) "ordered_Profile_subscriber_topic" };

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

    // Register the datatype to use when creating the Topic
    const char *type_name = orderedTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            orderedTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example ordered",
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

    /* Start changes for ordered_presentation */

    /* Create two subscribers to illustrate different presentation QoS
     * This is a publisher/subscriber level QoS, so we have to
     * do it here instead of just making two datareaders
     */
    orderedDataReader *typed_reader[2] = { 0 };

    /* Subscriber[0], reader[0] and ordered_reader[0] is getting
     * the profile "ordered_Profile_subscriber_instance"
     */
    /* Subscriber[1], reader[1] and ordered_reader[1] is getting
     * the profile "ordered_Profile_subscriber_topic"
     */
    for (int i = 0; i < MAX_SUBSCRIBERS; ++i) {
        std::cout << "Subscriber " << i << " using " << profile_name[i]
                  << std::endl;

        subscriber[i] = participant->create_subscriber_with_profile(
                "ordered_Library",
                profile_name[i],
                NULL,
                DDS_STATUS_MASK_NONE);
        if (subscriber[i] == NULL) {
            return shutdown_participant(
                    participant,
                    "create_subscriber error",
                    EXIT_FAILURE);
        }

        untyped_reader[i] = subscriber[i]->create_datareader_with_profile(
                topic,
                "ordered_Library",
                profile_name[i],
                NULL,
                DDS_STATUS_MASK_ALL);
        if (untyped_reader[i] == NULL) {
            return shutdown_participant(
                    participant,
                    "create_datareader error",
                    EXIT_FAILURE);
        }

        typed_reader[i] = orderedDataReader::narrow(untyped_reader[i]);
        if (typed_reader[i] == NULL) {
            return shutdown_participant(
                    participant,
                    "DataReader narrow error",
                    EXIT_FAILURE);
        }
    }

    /* If you want to change the Publisher's QoS programmatically rather than
     * using the XML file, you will need to add the following lines to your
     * code and comment out the above 'for' loop.
     */
    /*    for (int i = 0; i < MAX_SUBSCRIBERS; ++i) {
     */        /* Get default subscriber QoS to customize */
    /*        DDS_SubscriberQos subscriber_qos;
            retcode = participant->get_default_subscriber_qos(subscriber_qos);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "get_default_subscriber_qos error\n";
                return EXIT_FAILURE;
            }

    */        /* Set this for both subscribers */
    /*        subscriber_qos.presentation.ordered_access = DDS_BOOLEAN_TRUE;

            if (i == 0) {
                std::cout << "Subscriber 0 using Instance access scope\n";
                subscriber_qos.presentation.access_scope =
                        DDS_INSTANCE_PRESENTATION_QOS;
            } else {
                std::cout << "Subscriber 1 using Topic access scope\n";
                subscriber_qos.presentation.access_scope =
                        DDS_TOPIC_PRESENTATION_QOS;
            }

    */        /* To create subscriber with default QoS, use
           * DDS_SUBSCRIBER_QOS_DEFAULT instead of subscriber_qos */
    /*        subscriber[i] = participant->create_subscriber(subscriber_qos,
       NULL, DDS_STATUS_MASK_NONE); if (subscriber[i] == NULL) {
                return shutdown_participant(
                    participant,
                    "create_subscriber error",
                    EXIT_FAILURE);
            }

    */        /* No listener needed, but we do need to increase history depth */

    /* Get default datareader QoS to customize */
    /*        DDS_DataReaderQos datareader_qos;
            retcode = subscriber[i]->get_default_datareader_qos(datareader_qos);
            if (retcode != DDS_RETCODE_OK) {
                std::cerr << "get_default_datareader_qos error\n";
                return EXIT_FAILURE;
            }

            datareader_qos.history.depth = 10;

    */        /* To create datareader with default QoS,
           * use DDS_DATAREADER_QOS_DEFAULT instead of datareader_qos */
    /*        untyped_reader[i] = subscriber[i]->create_datareader(topic,
       datareader_qos, NULL, DDS_STATUS_MASK_ALL); if (untyped_reader[i] ==
       NULL) { return shutdown_participant( participant, "create_datareader
       error", EXIT_FAILURE);
            }

            typed_reader[i] = orderedDataReader::narrow(untyped_reader[i]);
            if (typed_reader[i] == NULL) {
                return shutdown_participant(
                    participant,
                    "DataReader narrow error",
                    EXIT_FAILURE);
            }
        }

    */
    /* Poll for data every 4 seconds */

    // Main loop. Wait for data to arrive, and process when it arrives
    unsigned int samples_read = 0;
    while (!shutdown_requested && samples_read < sample_count) {
        std::cout << "ordered subscriber sleeping for " << receive_period.sec
                  << " sec...\n";
        NDDSUtility::sleep(receive_period);

        samples_read += poll_data(typed_reader, MAX_SUBSCRIBERS);
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
