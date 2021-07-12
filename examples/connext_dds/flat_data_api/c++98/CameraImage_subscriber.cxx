
/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "CameraImage.h"
#include "CameraImageSupport.h"
#include "ndds/ndds_cpp.h"
#include "application.h"

using namespace application;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

// Shows how to access a flat data sample (this is the simplest code, for
// a more efficient implementation, see print_average_pixel_fast).
bool print_average_pixel_simple(const CameraImage &sample)
{
    CameraImageConstOffset camera = sample.root();
    if (camera.is_null()) {
        return false;
    }

    rti::flat::SequenceOffset<PixelConstOffset> pixels = camera.pixels();
    if (pixels.is_null()) {
        return false;
    }

    unsigned int pixel_count = pixels.element_count();
    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;

    for (rti::flat::SequenceOffset<PixelConstOffset>::iterator pixels_it =
                 pixels.begin();
         pixels_it != pixels.end();
         ++pixels_it) {
        PixelConstOffset pixel = *pixels_it;
        if (pixel.is_null()) {
            return false;
        }

        red_sum += pixel.red();
        green_sum += pixel.green();
        blue_sum += pixel.blue();
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", " << blue_sum / pixel_count
              << ")";

    return true;
}


// Shows how to access a flat data sample in a more efficient way
bool print_average_pixel_fast(const CameraImage &sample)
{
    CameraImageConstOffset camera = sample.root();
    if (camera.is_null()) {
        return false;
    }

    rti::flat::SequenceOffset<PixelConstOffset> pixels = camera.pixels();
    if (pixels.is_null()) {
        return false;
    }

    unsigned int pixel_count = pixels.element_count();

    if (pixel_count == 0) {
        std::cout << "WARNING: the pixels count is 0" << std::endl;
        return false;
    }

    PixelPlainHelper *pixel_array = rti::flat::plain_cast(pixels);
    if (pixel_array == NULL) {
        return false;
    }

    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;
    for (unsigned int i = 0; i < pixel_count; i++) {
        const PixelPlainHelper &pixel = pixel_array[i];
        red_sum += pixel.red;
        green_sum += pixel.green;
        blue_sum += pixel.blue;
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", " << blue_sum / pixel_count
              << ")";

    return true;
}

unsigned int process_data(CameraImageDataReader *typed_reader)
{
    CameraImageSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    unsigned int samples_read = 0;

    typed_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    for (int i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            CameraImageOffset root = data_seq[i].root();
            if (root.is_null()) {
                std::cerr << "error taking sample root\n";
                continue;
            }

            // Print the source name
            rti::flat::StringOffset source = root.source();
            if (!source.is_null()) {
                std::cout << root.source().get_string() << ": ";
            } else {
                std::cout << "(Unknown source): ";
            }

            // Print the field resolution (if it was published)
            ResolutionOffset resolution = root.resolution();
            if (!resolution.is_null()) {
                std::cout << "(Resolution: " << resolution.height() << " x "
                          << resolution.width() << ") ";
            }

            // print_average_pixel_simple(data_seq[i]); // Method 1
            print_average_pixel_fast(data_seq[i]);  // Method 2
            std::cout << std::endl;

            samples_read++;
        }
    }

    DDS_ReturnCode_t retcode = typed_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        std::cerr << "return loan error " << retcode << std::endl;
    }

    return samples_read;
}

int run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    DDS_Duration_t wait_timeout = { 4, 0 };

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
    const char *type_name = CameraImageTypeSupport::get_type_name();
    DDS_ReturnCode_t retcode =
            CameraImageTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        return shutdown_participant(
                participant,
                "register_type error",
                EXIT_FAILURE);
    }

    // Create a Topic with a name and a datatype
    DDSTopic *topic = participant->create_topic(
            "Example CameraImage",
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

    // This DataReader reads data on "Example CameraImage" Topic
    DDSDataReader *untyped_reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            NULL,
            DDS_STATUS_MASK_ALL);
    if (untyped_reader == NULL) {
        return shutdown_participant(
                participant,
                "create_datareader error",
                EXIT_FAILURE);
    }

    // Narrow casts from a untyped DataReader to a reader of your type
    CameraImageDataReader *typed_reader =
            CameraImageDataReader::narrow(untyped_reader);
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

        retcode = waitset.wait(active_conditions_seq, wait_timeout);

        if (retcode == DDS_RETCODE_OK) {
            // If the read condition is triggered, process data
            samples_read += process_data(typed_reader);
        } else if (retcode == DDS_RETCODE_TIMEOUT) {
            std::cout << "No data after " << wait_timeout.sec << " seconds"
                      << std::endl;
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
