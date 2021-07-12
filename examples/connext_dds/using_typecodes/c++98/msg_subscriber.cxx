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
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "application.h"

/* We use the typecode from built-in listeners, so we don't need to include
 * this code*/
/*
#include "msgSupport.h"
#include "ndds/ndds_cpp.h"
*/

/* We are going to use the BuiltinPublicationListener_on_data_available
 * to detect the topics that are being published on the domain
 *
 * Once we have detected a new topic, we will print out the Topic Name,
 * Participant ID, DataWriter id, and Data Type.
 */

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

class BuiltinPublicationListener : public DDSDataReaderListener {
public:
    virtual void on_data_available(DDSDataReader *reader);
};

void BuiltinPublicationListener::on_data_available(DDSDataReader *reader)
{
    DDSPublicationBuiltinTopicDataDataReader *builtin_reader =
            (DDSPublicationBuiltinTopicDataDataReader *) reader;
    DDS_PublicationBuiltinTopicDataSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;
    DDS_ExceptionCode_t exception_code;

    DDSSubscriber *subscriber = NULL;
    DDSDomainParticipant *participant = NULL;

    retcode = builtin_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    if (retcode != DDS_RETCODE_OK) {
        printf("***Error: failed to access data from the built-in reader\n");
        return;
    }

    std::cout << std::hex << std::setw(8) << std::setfill('0');
    for (int i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            std::cout << "-----\nFound topic \"" << data_seq[i].topic_name
                      << "\"\nparticipant: "
                      << data_seq[i].participant_key.value[0]
                      << data_seq[i].participant_key.value[1]
                      << data_seq[i].participant_key.value[2]
                      << "\ndatawriter: " << data_seq[i].key.value[0]
                      << data_seq[i].key.value[1] << data_seq[i].key.value[2]
                      << "\ntype:\n";

            if (data_seq[i].type_code == NULL) {
                std::cerr << "No type code received, perhaps increase "
                             "type_code_max_serialized_length?\n";
                continue;
            }

            /* Using the type_code propagated we print the data type
             * with print_IDL(). */
            data_seq[i].type_code->print_IDL(2, exception_code);
            if (exception_code != DDS_NO_EXCEPTION_CODE) {
                std::cerr << "print_IDL returns exception code "
                          << exception_code << std::endl;
            }
        }
    }
    builtin_reader->return_loan(data_seq, info_seq);
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    struct DDS_Duration_t receive_period = { 1, 0 };

    // Start communicating in a domain, usually one participant per application
    DDSDomainParticipant *participant =
            DDSTheParticipantFactory->create_participant(
                    domain_id,
                    DDS_PARTICIPANT_QOS_DEFAULT,
                    NULL,
                    DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }

    /* If you want to change the type_code_max_serialized_length
     * programmatically (e.g., to 3070) rather than using the XML file, you
     * will need to add the following lines to your code and comment out the
     * create_participant call above. */

    /*
    DDS_DomainParticipantQos participant_qos;
    retcode =
    DDSTheParticipantFactory->get_default_participant_qos(participant_qos);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "get_default_participant_qos error\n";
        return EXIT_FAILURE;
    }

    participant_qos.resource_limits.type_code_max_serialized_length = 3070;

    DDSDomainParticipant *participant =
    DDSTheParticipantFactory->create_participant( domain_id, participant_qos,
        NULL,
        DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        return shutdown_participant(
                participant,
                "create_participant error",
                EXIT_FAILURE);
    }
    */

    /* We don't actually care about receiving the samples, just the
       topic information.  To do this, we only need the builtin
       datareader for publications.
    */

    // First get the built-in subscriber
    DDSSubscriber *builtin_subscriber = participant->get_builtin_subscriber();
    if (builtin_subscriber == NULL) {
        return shutdown_participant(
                participant,
                "Failed to create builtin subscriber",
                EXIT_FAILURE);
    }

    // Then get the data reader for the built-in subscriber
    DDSPublicationBuiltinTopicDataDataReader *builtin_publication_datareader =
            (DDSPublicationBuiltinTopicDataDataReader *) builtin_subscriber
                    ->lookup_datareader(DDS_PUBLICATION_TOPIC_NAME);
    if (builtin_publication_datareader == NULL) {
        return shutdown_participant(
                participant,
                "Failed to create builtin publication data reader",
                EXIT_FAILURE);
    }

    // Finally install the listener
    BuiltinPublicationListener *builtin_publication_listener =
            new BuiltinPublicationListener();
    builtin_publication_datareader->set_listener(
            builtin_publication_listener,
            DDS_DATA_AVAILABLE_STATUS);

    // Main loop. Wait for data to arrive, and process when it arrives
    // Main loop, write data
    for (unsigned int count = 0; !shutdown_requested && count < sample_count;
         ++count) {
        NDDSUtility::sleep(receive_period);
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
    parse_arguments(arguments, argc, argv, subscriber);
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
