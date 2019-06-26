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

#include <stdio.h>
#include <stdlib.h>

#include "CameraImage.h"
#include "CameraImageSupport.h"
#include "ndds/ndds_cpp.h"

/* Delete all entities */
static int publisher_shutdown(
    DDSDomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides finalize_instance() method on
    domain participant factory for people who want to release memory used
    by the participant factory. Uncomment the following block of code for
    clean destruction of the singleton. */
    /*

    retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "finalize_instance error %d\n", retcode);
        status = -1;
    }
    */

    return status;
}

const int PIXEL_COUNT = 10;

// Simplest way to create the data sample
bool build_data_sample(CameraImageBuilder& builder, int seed)
{
    // The Traditional C++ API doesn't use exceptions, so errors must be checked
    // after each operation.
    //
    // Depending on the function, the error can be checked via the return value
    // when they return a boolean, an Offset (is_null()), or a Builder (is_valid()).
    // A Builder also provides check_failure() when the error can't be notified
    // with the return value.

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
#else // See Note in #if above
bool build_data_sample_fast(CameraImageBuilder& builder, int seed)
{
    return build_data_sample(builder, seed);
}
#endif

extern "C" int publisher_main(int domainId, int sample_count)
{
    DDSDomainParticipant *participant = NULL;
    DDSPublisher *publisher = NULL;
    DDSTopic *topic = NULL;
    DDSDataWriter *writer = NULL;
    CameraImageDataWriter * CameraImage_writer = NULL;
    CameraImage *instance = NULL;
    DDS_ReturnCode_t retcode;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;
    DDS_Duration_t send_period = {4,0};

    /* To customize participant QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    participant = DDSTheParticipantFactory->create_participant(
        domainId, DDS_PARTICIPANT_QOS_DEFAULT, 
        NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize publisher QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    publisher = participant->create_publisher(
        DDS_PUBLISHER_QOS_DEFAULT, NULL /* listener */, DDS_STATUS_MASK_NONE);
    if (publisher == NULL) {
        fprintf(stderr, "create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Register type before creating topic */
    type_name = CameraImageTypeSupport::get_type_name();
    retcode = CameraImageTypeSupport::register_type(
        participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize topic QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic(
        "Example CameraImage",
        type_name, DDS_TOPIC_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize data writer QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    writer = publisher->create_datawriter(
        topic, DDS_DATAWRITER_QOS_DEFAULT, NULL /* listener */,
        DDS_STATUS_MASK_NONE);
    if (writer == NULL) {
        fprintf(stderr, "create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }
    CameraImage_writer = CameraImageDataWriter::narrow(writer);
    if (CameraImage_writer == NULL) {
        fprintf(stderr, "DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Main loop */
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {

        CameraImageBuilder builder = rti::flat::build_data<CameraImage>(CameraImage_writer);
        if (builder.check_failure()) {
            printf("builder creation error\n");
            publisher_shutdown(participant);
            return -1;
        }

        // Build the CameraImage data sample using the builder
        bool build_result = false;
        if (count % 2 == 0) {
            build_result = build_data_sample(builder, count); // method 1
        } else {
            build_result = build_data_sample_fast(builder, count); // method 2
        }

        if (!build_result) {
            printf("error building the sample\n");
            publisher_shutdown(participant);
            return -1;
        }

        // Create the sample
        CameraImage *instance = builder.finish_sample();
        if (instance == NULL) {
            printf("finish_sample() error\n");
            publisher_shutdown(participant);
            return -1;            
        }

        printf("Writing CameraImage, count %d\n", count);

        retcode = CameraImage_writer->write(*instance, instance_handle);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }

        NDDSUtility::sleep(send_period);
    }

    /* Delete all entities */
    return publisher_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
    NDDSConfigLogger::get_instance()->
    set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API, 
    NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    return publisher_main(domain_id, sample_count);
}

