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

#include <algorithm>
#include <iostream>

#include <dds/core/ddscore.hpp>
#include <dds/sub/ddssub.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "CameraImage.hpp"
#include "application.hpp"

// Shows how to access a flat data sample (this is the simplest code, for
// a more efficient implementation, see print_average_pixel_fast).
void print_average_pixel_simple(const CameraImage &sample)
{
    auto pixels = sample.root().pixels();
    auto pixel_count = pixels.element_count();
    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;

    for (auto &&pixel : pixels) {
        red_sum += pixel.red();
        green_sum += pixel.green();
        blue_sum += pixel.blue();
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", " << blue_sum / pixel_count
              << ")";
}

// Shows how to access a flat data sample in a more efficient way
void print_average_pixel_fast(const CameraImage &sample)
{
    auto pixels = sample.root().pixels();
    auto pixel_count = pixels.element_count();

    if (pixel_count == 0) {
        return;
    }

    auto pixel_array = rti::flat::plain_cast(pixels);

    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;
    for (unsigned int i = 0; i < pixel_count; i++) {
        const auto &pixel = pixel_array[i];
        red_sum += pixel.red();
        green_sum += pixel.green();
        blue_sum += pixel.blue();
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", " << blue_sum / pixel_count
              << ")";
}

int process_data(dds::sub::DataReader<CameraImage> &reader)
{
    // Take all samples
    int count = 0;
    auto samples = rti::sub::valid_data(reader.take());
    for (const auto &sample : samples) {
        count++;
        auto root = sample.data().root();

        // Print the source name. We assume the publisher always sets this
        // field; if not, root.source().is_null() is true, and get_string() will
        // throw an exception
        std::cout << root.source().get_string() << ": ";

        // Print the field resolution. In this case we assume the publisher may
        // decide not to send this field
        auto resolution = root.resolution();
        if (!resolution.is_null()) {
            std::cout << "(Resolution: " << resolution.height() << " x "
                      << resolution.width() << ") ";
        }

        // print_average_pixel_simple(sample.data()); // Method 1
        print_average_pixel_fast(sample.data());  // Method 2
        std::cout << std::endl;
    }

    return count;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<CameraImage> topic(participant, "Example CameraImage");

    // Create a DataReader with default Qos (Subscriber created in-line)
    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            topic);

    // Create a ReadCondition for any data on this reader and associate a
    // handler
    int count = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [&reader, &count](/* dds::core::cond::Condition condition */) {
                count += process_data(reader);
            });

    // Create a WaitSet and attach the ReadCondition
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    while (!application::shutdown_requested && count < sample_count) {
        // Dispatch will call the handlers associated to the WaitSet conditions
        // when they activate
        std::cout << "CameraImage subscriber sleeping for 4 sec..."
                  << std::endl;

        waitset.dispatch(dds::core::Duration(4));  // Wait up to 4s each time
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
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
