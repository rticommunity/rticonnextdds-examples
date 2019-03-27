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
#include <rti/util/util.hpp> // for sleep()
#include <rti/config/Logger.hpp>

#include "CameraImage.hpp"

const int PIXEL_COUNT = 10;

// Simplest way to create the data sample
void build_data_sample(CameraImageBuilder& builder, int seed)
{
    builder.add_format(Format::RGB);

    auto resolution_offset = builder.add_resolution();
    resolution_offset.height(100);
    resolution_offset.width(200);

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
void build_data_sample_fast(CameraImageBuilder& builder, int seed)
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
    // cheap (it just advances the underlying buffer without initializing anything)
    auto pixels_builder = builder.build_pixels();
    pixels_builder.add_n(PIXEL_COUNT);
    auto pixels = pixels_builder.finish();

    // 2) Use plain_cast to access the buffer as an array of regular C++ Pixels
    // (pixel_array's type is PixelPlainHelper*)
    auto pixel_array = rti::flat::plain_cast(pixels);
    for (int i = 0; i < PIXEL_COUNT; i++) {
        auto& pixel = pixel_array[i];
        pixel.red((seed + i) % 100);
        pixel.green((seed + i + 1) % 100);
        pixel.blue((seed + i + 2) % 100);
    }
}

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<CameraImage> topic (participant, "Example CameraImage");

    // Create a DataWriter with default Qos (Publisher created in-line)
    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            topic);

    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        CameraImageBuilder builder = rti::flat::build_data(writer);

        // Build the CameraImage data sample using the builder
        if (count % 2 == 0) {
            build_data_sample(builder, count); // method 1
        } else {
            build_data_sample_fast(builder, count); // method 2
        }

        // Create the sample
        CameraImage *sample = builder.finish_sample();

        std::cout << "Writing CameraImage, count " << count << std::endl;

        writer.write(*sample);

        rti::util::sleep(dds::core::Duration(4));
    }
}

int main(int argc, char *argv[])
{

    int domain_id = 0;
    int sample_count = 0; // infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    rti::config::Logger::instance().verbosity(rti::config::Verbosity::WARNING);

    try {
        publisher_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

