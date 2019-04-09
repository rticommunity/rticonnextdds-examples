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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
// Or simply include <dds/dds.hpp>

#include "CameraImage.hpp"

// Shows how to access a flat data sample (this is the simplest code, for
// a more efficient implementation, see print_average_pixel_fast).
void print_average_pixel_simple(const CameraImage& sample)
{
    auto pixels = sample.root().pixels();
    auto pixel_count = pixels.element_count();
    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;

    for (auto&& pixel : pixels) {
        red_sum += pixel.red();
        green_sum += pixel.green();
        blue_sum += pixel.blue();
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", "
              << blue_sum / pixel_count << ")";
}

// Shows how to access a flat data sample in a more efficient way
void print_average_pixel_fast(const CameraImage& sample)
{
    auto pixels = sample.root().pixels();
    auto pixel_count = pixels.element_count();
    auto pixel_array = rti::flat::plain_cast(pixels);

    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;
    for (unsigned int i = 0; i < pixel_count; i++) {
        const auto& pixel = pixel_array[i];
        red_sum += pixel.red();
        green_sum += pixel.green();
        blue_sum += pixel.blue();
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", "
              << blue_sum / pixel_count << ")";
}

int process_data(dds::sub::DataReader<CameraImage>& reader)
{
    // Take all samples
    int count = 0;
    auto samples = rti::sub::valid_data(reader.take());
    for (const auto& sample : samples) {
        count++;
        auto root = sample.data().root();

        std::cout << root.source().get_string() << ": ";
        // print_average_pixel_simple(sample.data()); // Method 1
        print_average_pixel_fast(sample.data()); // Method 2
        std::cout << std::endl;
    }

    return count;
}

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<CameraImage> topic(participant, "Example CameraImage");

    // Create a DataReader with default Qos (Subscriber created in-line)
    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            topic);

    // Create a ReadCondition for any data on this reader and associate a handler
    int count = 0;
    dds::sub::cond::ReadCondition read_condition(
        reader,
        dds::sub::status::DataState::any(),
        [&reader, &count](/* dds::core::cond::Condition condition */)
        {
            count += process_data(reader);
        }
    );

    // Create a WaitSet and attach the ReadCondition
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    while (count < sample_count || sample_count == 0) {
        // Dispatch will call the handlers associated to the WaitSet conditions
        // when they activate
        std::cout << "CameraImage subscriber sleeping for 4 sec..." << std::endl;

        waitset.dispatch(dds::core::Duration(4)); // Wait up to 4s each time
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
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

