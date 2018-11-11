/*
 * (c) Copyright, Real-Time Innovations, 2019.
 * All rights reserved.
 *
 * No duplications, whole or partial, manual or electronic, may be made
 * without express written permission.  Any such copies, or
 * revisions thereof, must display this notice unaltered.
 * This code contains trade secrets of Real-Time Innovations, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iterator>

#include <dds/core/corefwd.hpp>

#include <rti/routing/processor/ProcessorPlugin.hpp>
#include <rti/routing/processor/Processor.hpp>
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/core/xtypes/StructType.hpp>

#include "ShapesProcessor.hpp"

using namespace rti::routing;
using namespace rti::routing::processor;
using namespace rti::routing::adapter;
using namespace dds::core::xtypes;

/*
 * --- ShapesAggregator -------------------------------------------------------
 */

ShapesAggregrator::ShapesAggregrator()
    :output_data_(dds::core::xtypes::StructType("dummy"))
{
}

ShapesAggregrator::~ShapesAggregrator()
{
}

void ShapesAggregrator::on_output_enabled(
        rti::routing::processor::Route& route,
        rti::routing::processor::Output& output)
{
    // initialize the output_data buffer using the type from the output
    output_data_ = TypedOutput<DynamicData>(&output).create_data();
}

void ShapesAggregrator::get_circles_data(
    rti::routing::processor::LoanedSamples<DynamicData>& circles,
    const dds::core::InstanceHandle& handle)
{
    for (auto circle_sample : circles) {       
        if (circle_sample.info().instance_handle() == handle
                && circle_sample.info().state().instance_state() == dds::sub::status::InstanceState::alive()) {
            output_data_.value<int32_t>(
                    "y",
                    circle_sample.data().value<int32_t>("y"));
            break;
        }
    }
}

void ShapesAggregrator::on_data_available(
        rti::routing::processor::Route& route)
{
    // Use squares as 'driver' input. For each Square instance, get the
    // equivalent instance from the Circle topic
    auto squares = route.input<DynamicData>("Square").read();
    auto circles = route.input<DynamicData>("Circle").read();
    for (auto square_sample : squares) {
        if (square_sample.info().valid()) {
            output_data_ = square_sample.data();
            // find and existing existing instance in the Circles stream
            get_circles_data(circles, square_sample.info().instance_handle());
            // Write aggregated sample intro Triangles
            route.output<DynamicData>("Triangle").write(output_data_);
        } else {
            route.output<DynamicData>("Triangle").write(
                    output_data_,
                    square_sample.info());
            //clear instance instance
            route.input<DynamicData>("Square").select()
                    .instance(square_sample.info().instance_handle()).take();
        }
    }
}


/*
 * --- ShapesSplitter ---------------------------------------------------------
 */

ShapesSplitter::ShapesSplitter()
    :output_circle_(dds::core::xtypes::StructType("dummy")),
     output_triangle_(dds::core::xtypes::StructType("dummy"))
{
}

ShapesSplitter::~ShapesSplitter()
{
}

void ShapesSplitter::on_output_enabled(
        rti::routing::processor::Route& route,
        rti::routing::processor::Output& output)
{
    // initialize the output_data buffer using the type from the output
    if (output.stream_info().stream_name() == "Circle") {
        output_circle_ = TypedOutput<DynamicData>(&output).create_data();
    } else if (output.stream_info().stream_name() == "Triangle") {
        output_triangle_ = TypedOutput<DynamicData>(&output).create_data();
    }
}

void ShapesSplitter::on_data_available(
        rti::routing::processor::Route& route)
{
    // Split squares into monodimensional circles and triangles
    auto squares = route.input<DynamicData>("Square").take();
    for (auto square_sample : squares) {
        if (square_sample.info().valid()) {
            output_circle_ = square_sample.data();
            output_circle_.value<int32_t>("y", 0);
            output_triangle_ = square_sample.data();
            output_triangle_.value<int32_t>("x", 0);
        }

        route.output<DynamicData>("Circle").write(
                output_circle_,
                square_sample.info());
        route.output<DynamicData>("Triangle").write(
                output_triangle_,
                square_sample.info());
    }
}



/*
 * --- ShapesProcessorPlugin --------------------------------------------------
 */

const std::string ShapesProcessorPlugin::PROCESSOR_KIND_PROPERTY_NAME =
        "shapes_processor.kind";

const std::string ShapesProcessorPlugin::PROCESSOR_AGGREGATOR_NAME =
        "aggregator";

ShapesProcessorPlugin::ShapesProcessorPlugin(
        const rti::routing::PropertySet&)
{
    
}



rti::routing::processor::Processor* ShapesProcessorPlugin::create_processor(
        rti::routing::processor::Route&,
        const rti::routing::PropertySet& properties)
{
    PropertySet::const_iterator it = properties.find(PROCESSOR_KIND_PROPERTY_NAME);
    if (it != properties.end()
            && it->second == PROCESSOR_AGGREGATOR_NAME) {
        return new ShapesAggregrator();
    }

    return new ShapesSplitter();
}

void ShapesProcessorPlugin::delete_processor(
        rti::routing::processor::Route&,
        rti::routing::processor::Processor *processor)
{
    delete processor;
}




RTI_PROCESSOR_PLUGIN_CREATE_FUNCTION_DEF(ShapesProcessorPlugin);