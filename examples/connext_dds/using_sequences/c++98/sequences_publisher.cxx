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

#include "sequences.h"
#include "sequencesSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    /* Send a new sample every second */
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
    const char *type_name = sequencesTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            sequencesTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example sequences",
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

    // This DataWriter writes data on "Example prueba" Topic
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

    sequencesDataWriter *typed_writer =
            sequencesDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    /* Here we define two instances: owner_instance and borrower_instance. */

    /* owner_instance.data uses its own memory, as by default, a sequence
     * you create owns its memory unless you explicitly loan memory of your
     * own to it.
     */
    sequences *owner_instance = sequencesTypeSupport::create_data();
    if (owner_instance == NULL) {
        return shutdown_participant(
                participant,
                "sequencesTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* borrower_instance.data "borrows" memory from a buffer of shorts,
     * previously allocated, using DDS_ShortSeq_loan_contiguous(). */
    sequences *borrower_instance = sequencesTypeSupport::create_data();
    if (borrower_instance == NULL) {
        return shutdown_participant(
                participant,
                "sequencesTypeSupport::create_data error",
                EXIT_FAILURE);
    }

    /* If we want borrower_instance.data to loan a buffer of shorts, first we
     * have to allocate the buffer. Here we allocate a buffer of
     * MAX_SEQUENCE_LEN. */
    short *short_buffer = new short[MAX_SEQUENCE_LEN];

    /* Before calling loan_contiguous(), we need to set Seq.maximum to
     * 0, i.e., the sequence won't have memory allocated to it. */
    borrower_instance->data.maximum(0);

    /* Now that the sequence doesn't have memory allocated to it, we can use
     * loan_contiguous() to loan short_buffer to borrower_instance.
     * We set the allocated number of elements to MAX_SEQUENCE_LEN, the size of
     * the buffer and the maximum size of the sequence as we declared in the
     * IDL. */
    bool return_result = borrower_instance->data.loan_contiguous(
            short_buffer,     // Pointer to the sequence
            0,                // Initial Length
            MAX_SEQUENCE_LEN  // New maximum
    );
    if (return_result != DDS_BOOLEAN_TRUE) {
        return shutdown_participant(
                participant,
                "loan_contiguous error",
                EXIT_FAILURE);
    }

    /* Before starting to publish samples, set the instance id of each
     * instance*/
    strcpy(owner_instance->id, "owner_instance");
    strcpy(borrower_instance->id, "borrower_instance");

    /* To illustrate the use of the sequences, in the main loop we set a
     * new sequence length every iteration to the sequences contained in
     * both instances (instance->data). The sequence length value cycles between
     * 0 and MAX_SEQUENCE_LEN. We assign a random number between 0 and 100 to
     * each sequence's elements. */
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        /* We set a different sequence_length for both instances every
         * iteration. sequence_length is based on the value of count and
         * its value cycles between the values of 1 and MAX_SEQUENCE_LEN. */
        short sequence_length = (samples_written % MAX_SEQUENCE_LEN) + 1;

        std::cout << "Writing sequences, count " << samples_written << "...\n";

        owner_instance->count = samples_written;
        borrower_instance->count = samples_written;

        /* Here we set the new length of each sequence */
        owner_instance->data.length(sequence_length);
        borrower_instance->data.length(sequence_length);

        /* Now that the sequences have a new length, we assign a
         * random number between 0 and 100 to each element of
         * owner_instance->data and borrower_instance->data. */
        for (int i = 0; i < sequence_length; ++i) {
            owner_instance->data[i] = (short) (rand() / (RAND_MAX / 100));
            borrower_instance->data[i] = (short) (rand() / (RAND_MAX / 100));
        }

        /* Both sequences have the same length, so we only print the length
         * of one of them. */
        std::cout << "Instances length = " << owner_instance->data.length()
                  << std::endl;


        /* Write for each instance */
        retcode = typed_writer->write(*owner_instance, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        retcode = typed_writer->write(*borrower_instance, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        NDDSUtility::sleep(send_period);
    }


    /* Once we are done with the sequence, we unloan and free the buffer. Make
     * sure you don't call delete before unloan(); the next time you
     * access the sequence, you will be accessing freed memory. */
    return_result = borrower_instance->data.unloan();
    if (return_result != DDS_BOOLEAN_TRUE) {
        std::cerr << "unloan error \n";
    }

    delete[] short_buffer;

    // Delete previously allocated prueba, including all contained elements
    retcode = sequencesTypeSupport::delete_data(owner_instance);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "sequencesTypeSupport::delete_data error " << retcode
                  << std::endl;
    }

    retcode = sequencesTypeSupport::delete_data(borrower_instance);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "sequencesTypeSupport::delete_data error " << retcode
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
