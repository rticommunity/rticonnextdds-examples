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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging
#include <rti/core/xtypes/DynamicDataTuples.hpp>

#include "Shapes.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void print_shape_data(const dds::core::xtypes::DynamicData &data)
{
    // First method: using C++ tuples (C++11 only).
    auto shape_tuple_data = rti::core::xtypes::
            get_tuple<std::string, int32_t, int32_t, int32_t>(data);

    std::cout << "\t color: " << std::get<0>(shape_tuple_data) << std::endl
              << "\t x: " << std::get<1>(shape_tuple_data) << std::endl
              << "\t y: " << std::get<2>(shape_tuple_data) << std::endl
              << "\t shapesize: " << std::get<3>(shape_tuple_data) << std::endl;

    // Second method: using DynamicData getters.
    std::cout << "\t color: " << data.value<std::string>("color") << std::endl
              << "\t x: " << data.value<int32_t>("x") << std::endl
              << "\t y: " << data.value<int32_t>("y") << std::endl
              << "\t shapesize: " << data.value<int32_t>("shapesize")
              << std::endl;

    // Third method: automatic with the '<<' operator overload.
    std::cout << data << std::endl;
}

void run_subscriber_application(
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
    // Make sure both publisher and subscriber share the same topic name.
    // In the Shapes example: we are subscribing to a Square, wich is the
    // topic name. If you want to publish other shapes (Triangle or Circle),
    // you just need to update the topic name.
    dds::topic::Topic<dds::core::xtypes::DynamicData> topic(
            participant,
            "Square",
            shape_type);

    // Create a subscriber
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader.
    dds::sub::DataReader<dds::core::xtypes::DynamicData> reader(
            subscriber,
            topic);

    // Create a ReadCondition for any data on this reader and associate
    // a handler.
    int count = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [&reader, &count]() {
                // Take all samples
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData>
                        samples = reader.take();
                for (auto sample : samples) {
                    if (sample.info().valid()) {
                        count++;

                        // Print sample with different methods.
                        print_shape_data(sample.data());
                    }
                }
            });

    // Create a WaitSet and attach the ReadCondition
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    while (!application::shutdown_requested && count < sample_count) {
        // Dispatch will call the handlers associated to the WaitSet conditions
        // when they activate
        std::cout << "ShapeType subscriber sleeping for 1 sec..." << std::endl;
        waitset.dispatch(dds::core::Duration(1));  // Wait up to 1s each time
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
