/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging
#include <rti/core/xtypes/DynamicDataTuples.hpp>

#include "application.hpp"  // for command line parsing and ctrl-c
#include "Shapes.hpp"

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create the participant.
    dds::domain::DomainParticipant participant(domain_id);

    // Create DynamicData using the type defined in the IDL file.
    const dds::core::xtypes::StructType &shape_type =
            rti::topic::dynamic_type<ShapeType>::get();

    // If you want to create the type from code instead of using an IDL
    // file with rtiddsgen, comment out the previous declaration and
    // uncomment the following code.

    // StructType shape_type("ShapeType");
    // shape_type.add_member(Member("color", StringType(128)).key(true));
    // shape_type.add_member(Member("x", primitive_type<int32_t>()));
    // shape_type.add_member(Member("y", primitive_type<int32_t>()));
    // shape_type.add_member(Member("shapesize", primitive_type<int32_t>()));

    // Create a Topic -- and automatically register the type.
    // Make sure both publisher and subscriber share the same topic
    // name. In the Shapes example: we are publishing a Square, which is the
    // topic name. If you want to publish other shapes (Triangle or
    // Circle), you just need to update the topic name.
    dds::topic::Topic<dds::core::xtypes::DynamicData> topic(
            participant,
            "Square",
            shape_type);

    // Create a Publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter.
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> writer(
            publisher,
            topic);

    // Create an instance of DynamicData.
    dds::core::xtypes::DynamicData shape_data(shape_type);

    // Initialize the data values.
    int direction = 1;
    int x_position = 50;
    int shape_size = 30;

    // Set data with C++ tuples (C++11 only).
    rti::core::xtypes::set_tuple(
            shape_data,
            std::make_tuple(std::string("BLUE"), x_position, 100, shape_size));

    // Standard method with setters (C++03 compatible).
    // shape_data.value<std::string>("color", "BLUE");
    // shape_data.value("x", x_position);
    // shape_data.value("y", 100);
    // shape_data.value("shapesize", shape_size);

    // Main loop
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Modify and set the shape size from 30 to 50.
        shape_size = 30 + (samples_written % 20);
        shape_data.value("shapesize", shape_size);

        // Set the position X.
        shape_data.value("x", x_position);

        // Publish data.
        std::cout << "Sending [shapesize=" << shape_size << ", x=" << x_position
                  << "]" << std::endl;
        writer.write(shape_data);

        // Update the position X.
        // It will be modified adding or substraction 2 to the previous value
        // depending on the direction. The value will be between 50 and 150.
        // When it reachs one border, the direction is inverted.
        x_position += (direction * 2);
        if (x_position >= 150) {
            direction = -1;
        } else if (x_position <= 50) {
            direction = 1;
        }

        rti::util::sleep(dds::core::Duration::from_millisecs(100));
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
