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

const int PIXEL_COUNT = 10;

static int shutdown_participant(
        DDSDomainParticipant *participant,
        const char *shutdown_message,
        int status);

// Simplest way to create the data sample
bool build_data_sample(CameraImageBuilder &builder, int seed)
{
    // The Traditional C++ API doesn't use exceptions, so errors must be checked
    // after each operation.
    //
    // Depending on the function, the error can be checked via the return value
    // when they return a boolean, an Offset (is_null()), or a Builder
    // (is_valid()). A Builder also provides check_failure() when the error
    // can't be notified with the return value.

    if (!builder.add_format(RGB)) {
        return false;
    }

    if (seed % 3 == 0) {
        // All fields in a mutable FlatData type are in effect optional. For
        // illustration purposes, we will omit the resolution field in some
        // samples
        ResolutionOffset resolution_offset = builder.add_resolution();
        if (resolution_offset.is_null()) {
            return false;
        }
        if (!resolution_offset.height(100)) {
            return false;
        }
        if (!resolution_offset.width(200)) {
            return false;
        }
    }

    rti::flat::StringBuilder string_builder = builder.build_source();
    if (string_builder.check_failure()) {
        return false;
    }

    string_builder.set_string("CAM-1");
    if (string_builder.check_failure()) {
        return false;
    }

    string_builder.finish();
    if (string_builder.check_failure()) {
        return false;
    }

    // Method 1 - Build the pixel sequence element by element
    rti::flat::FinalSequenceBuilder<PixelOffset> pixels_builder =
            builder.build_pixels();
    if (!pixels_builder.is_valid()) {
        return false;
    }

    for (int i = 0; i < PIXEL_COUNT; i++) {
        PixelOffset pixel = pixels_builder.add_next();
        if (pixel.is_null()) {
            return false;
        }
        if (!pixel.red((seed + i) % 100)) {
            return false;
        }
        if (!pixel.green((seed + i + 1) % 100)) {
            return false;
        }
        if (!pixel.blue((seed + i + 2) % 100)) {
            return false;
        }
    }

    rti::flat::SequenceOffset<PixelOffset> pixels = pixels_builder.finish();
    if (pixels.is_null()) {
        return false;
    }

    return true;
}

//
// Note:
// build_data_sample_fast disabled due to known issue.
// Contact support@rti.com for a fix.
//
#if 0
// Creates the same data sample using a faster method
// to populate the pixel sequence
bool build_data_sample_fast(CameraImageBuilder& builder, int seed)
{
    // The initialization of these members doesn't change
    rti::flat::StringBuilder string_builder = builder.build_source();
    if (string_builder.check_failure()) {
        return false;
    }

    string_builder.set_string("CAM-1");
    if (string_builder.check_failure()) {
        return false;
    }

    string_builder.finish();
    if (string_builder.check_failure()) {
        return false;
    }

    if (!builder.add_format(RGB)) {
        return false;
    }

    ResolutionOffset resolution_offset = builder.add_resolution();
    if (resolution_offset.is_null()) {
        return false;
    }
    if (!resolution_offset.height(100)) {
        return false;
    }
    if (!resolution_offset.width(200)) {
        return false;
    }

    // Populate the pixel sequence accessing the byte buffer directly, instead
    // of iterating through each element Offset

    // 1) Use the builder to add all the elements at once. This operation is
    // cheap (it just advances the underlying buffer without initializing anything)
    rti::flat::FinalSequenceBuilder<PixelOffset> pixels_builder =
            builder.build_pixels();
    if (!pixels_builder.is_valid()) {
        return false;
    }

    pixels_builder.add_n(PIXEL_COUNT);
    if (pixels_builder.check_failure()) {
        return false;
    }

    rti::flat::SequenceOffset<PixelOffset> pixels = pixels_builder.finish();
    if (pixels.is_null()) {
        return false;
    }

    // 2) Use plain_cast to access the buffer as an array of regular C++ Pixels
    // (pixel_array's type is PixelPlainHelper*)
    PixelPlainHelper *pixel_array = rti::flat::plain_cast(pixels);
    if (pixel_array == NULL) {
        return false;
    }
    for (int i = 0; i < PIXEL_COUNT; i++) {
        PixelPlainHelper& pixel = pixel_array[i];
        pixel.red = (seed + i) % 100;
        pixel.green = (seed + i + 1) % 100;
        pixel.blue = (seed + i + 2) % 100;
    }

    return true;
}
#else  // See Note in #if above
bool build_data_sample_fast(CameraImageBuilder &builder, int seed)
{
    return build_data_sample(builder, seed);
}
#endif

int run_publisher_application(unsigned int domain_id, unsigned int sample_count)
{
    DDS_Duration_t send_period = { 4, 0 };

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

    // This DataWriter writes data on "Example StringLine" Topic
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

    // Narrow casts from an untyped DataWriter to a writer of your type
    CameraImageDataWriter *typed_writer =
            CameraImageDataWriter::narrow(untyped_writer);
    if (typed_writer == NULL) {
        return shutdown_participant(
                participant,
                "DataWriter narrow error",
                EXIT_FAILURE);
    }

    // Main loop, write data
    for (unsigned int samples_written = 0;
         !shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        CameraImageBuilder builder =
                rti::flat::build_data<CameraImage>(typed_writer);
        if (builder.check_failure()) {
            return shutdown_participant(
                    participant,
                    "builder creation error",
                    EXIT_FAILURE);
        }

        // Build the CameraImage data sample using the builder
        bool build_result = false;
        if (samples_written % 2 == 0) {
            build_result =
                    build_data_sample(builder, samples_written);  // method
                                                                  // 1
        } else {
            build_result =
                    build_data_sample_fast(builder, samples_written);  // method
                                                                       // 2
        }

        if (!build_result) {
            return shutdown_participant(
                    participant,
                    "error building the sample",
                    EXIT_FAILURE);
        }

        // Create the sample
        CameraImage *data = builder.finish_sample();
        if (data == NULL) {
            return shutdown_participant(
                    participant,
                    "finish_sample() error",
                    EXIT_FAILURE);
        }

        std::cout << "Writing CameraImage, count " << samples_written
                  << std::endl;

        retcode = typed_writer->write(*data, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK) {
            std::cerr << "write error " << retcode << std::endl;
        }

        NDDSUtility::sleep(send_period);
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
