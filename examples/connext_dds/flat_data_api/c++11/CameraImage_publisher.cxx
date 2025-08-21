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

#include <dds/pub/ddspub.hpp>
#include <rti/config/Logger.hpp>
#include <rti/util/util.hpp>  // for sleep()

#include "CameraImage.hpp"
#include "application.hpp"

const int PIXEL_COUNT = 10;

// Simplest way to create the data sample
void build_data_sample(CameraImageBuilder &builder, int seed)
{
    builder.add_format(Format::RGB);

    if (seed % 3 == 0) {
        // All fields in a mutable FlatData type are in effect optional. For
        // illustration purposes, we will omit the resolution field in some
        // samples
        auto resolution_offset = builder.add_resolution();
        resolution_offset.height(100);
        resolution_offset.width(200);
    }

    auto string_builder = builder.build_source();
    string_builder.set_string("CAM-1");
    string_builder.finish();

    // Method 1 - Build the pixel sequence element by element
    auto pixels_builder = builder.build_pixels();
    for (int i = 0; i < PIXEL_COUNT; i++) {
        auto pixel = pixels_builder.add_next();
        pixel.red((seed + i) % 100);
        pixel.green((seed + i + 1) % 100);
        pixel.blue((seed + i + 2) % 100);
    }
    pixels_builder.finish();
}

// Creates the same data sample using a faster method
// to populate the pixel sequence
void build_data_sample_fast(CameraImageBuilder &builder, int seed)
{
    // The initialization of these members doesn't change
    builder.build_source().set_string("CAM-1");
    builder.add_format(Format::RGB);

    auto resolution_offset = builder.add_resolution();
    resolution_offset.height(100);
    resolution_offset.width(200);


    // Populate the pixel sequence accessing the byte buffer directly, instead
    // of iterating through each element Offset

    // 1) Use the builder to add all the elements at once. This operation is
    // cheap (it just advances the underlying buffer without initializing
    // anything)
    auto pixels_builder = builder.build_pixels();
    pixels_builder.add_n(PIXEL_COUNT);
    auto pixels = pixels_builder.finish();

    // 2) Use plain_cast to access the buffer as an array of regular C++ Pixels
    // (pixel_array's type is PixelPlainHelper*)
    auto pixel_array = rti::flat::plain_cast(pixels);
    for (int i = 0; i < PIXEL_COUNT; i++) {
        auto &pixel = pixel_array[i];
        pixel.red = (seed + i) % 100;
        pixel.green = (seed + i + 1) % 100;
        pixel.blue = (seed + i + 2) % 100;
    }
}

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<CameraImage> topic(participant, "Example CameraImage");

    // Create a DataWriter with default Qos (Publisher created in-line)
    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            topic);

    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        CameraImageBuilder builder = rti::flat::build_data(writer);

        // Build the CameraImage data sample using the builder
        if (samples_written % 2 == 0) {
            build_data_sample(builder, samples_written);  // method 1
        } else {
            build_data_sample_fast(builder, samples_written);  // method 2
        }

        // Create the sample
        CameraImage *sample = builder.finish_sample();

        std::cout << "Writing CameraImage, count " << samples_written
                  << std::endl;

        writer.write(*sample);

        rti::util::sleep(dds::core::Duration(4));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_publisher_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
