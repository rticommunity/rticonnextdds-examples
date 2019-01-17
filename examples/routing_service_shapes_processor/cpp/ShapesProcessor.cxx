/*
 * (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
using namespace dds::sub::status;


/*
 * --- ShapesAggregatorSimple -------------------------------------------------
 */

ShapesAggregatorSimple::ShapesAggregatorSimple()
{
}

ShapesAggregatorSimple::~ShapesAggregatorSimple()
{
}

void ShapesAggregatorSimple::on_output_enabled(
        rti::routing::processor::Route& route,
        rti::routing::processor::Output& output)
{
    // initialize the output_data buffer using the type from the output
    output_data_ = output.get<DynamicData>().create_data();
}

void ShapesAggregatorSimple::on_data_available(
        rti::routing::processor::Route& route)
{
    // Use squares as 'leading' input. For each Square instance, get the
    // equivalent instance from the Circle topic
    auto squares = route.input<DynamicData>("Square").read();
    for (auto square_sample : squares) {
        if (square_sample.info().valid()) {
            output_data_ = square_sample.data();
            // read equivalent existing instance in the Circles stream
            auto circles = route.input<DynamicData>("Circle").select()
                .instance(square_sample.info().instance_handle()).read();
            if (circles.length() != 0
                    && circles[0].info().valid()) {
                output_data_.get().value<int32_t>(
                        "shapesize",
                        circles[0].data().value<int32_t>("y"));
            }
            // Write aggregated sample intro Triangles
            route.output<DynamicData>("Triangle").write(output_data_.get());
        } else {
            // propagate the dispose
            route.output<DynamicData>("Triangle").write(
                    output_data_.get(),
                    square_sample.info());
            //clear instance instance
            route.input<DynamicData>("Square").select()
                    .instance(square_sample.info().instance_handle()).take();
        }
    }
}

/*
 * --- ShapesAggregatorAdv -------------------------------------------------------
 */

ShapesAggregatorAdv::ShapesAggregatorAdv(int32_t leading_input_index)
    : leading_input_index_(leading_input_index)
{
}

ShapesAggregatorAdv::~ShapesAggregatorAdv()
{
}

void ShapesAggregatorAdv::on_output_enabled(
        rti::routing::processor::Route& route,
        rti::routing::processor::Output& output)
{
    // initialize the output_data buffer using the type from the output
    output_data_ = output.get<DynamicData>().create_data();
}

void ShapesAggregatorAdv::on_data_available(
        rti::routing::processor::Route& route)
{
    // Read all data from leading input, then select equivalent instance from
    // other inputs
    DataState new_data = DataState(
            SampleState::not_read(),
            ViewState::any(),
            InstanceState::any());
    auto leading_samples = route.input<DynamicData>(leading_input_index_)
            .select().state(new_data).read();
    for (auto leading : leading_samples) {
        std::pair<int, int> output_xy(0, 0);

        if (leading.info().valid()) {
            output_data_ = leading.data();

            output_xy.first += leading.data().value<int32_t>("x");
            output_xy.second += leading.data().value<int32_t>("y");
            for (int32_t i = 0; i < route.input_count(); i++) {
                if (i == leading_input_index_) {
                    continue;
                }

                auto aggregated_samples = route.input<DynamicData>(i).select()
                        .instance(leading.info().instance_handle()).read();
                if (aggregated_samples.length() != 0) {
                    //use last value cached
                    output_xy.first += aggregated_samples[0].data().value<int32_t>("x");
                    output_xy.second +=  aggregated_samples[0].data().value<int32_t>("y");
                }
            }
            output_xy.first /= route.input_count();
            output_xy.second /= route.input_count();
            output_data_.get().value<int32_t>(
                    "x",
                    output_xy.first);
            output_data_.get().value<int32_t>(
                    "y",
                    output_xy.second);
             // Write aggregated sample into single output
            route.output<DynamicData>(0).write(output_data_.get());
        } else {
            // propagate the dispose
            route.output<DynamicData>(0).write(
                    output_data_.get(),
                    leading.info());
        }
    }
}


/*
 * --- ShapesSplitter ---------------------------------------------------------
 */

ShapesSplitter::ShapesSplitter()
{
}

ShapesSplitter::~ShapesSplitter()
{
}

void ShapesSplitter::on_input_enabled(
        rti::routing::processor::Route& route,
        rti::routing::processor::Input& input)
{
    // The type this processor works with is the ShapeType, which shall
    // be the type the input as well as the two outputs. Hence we can use
    // the input type to initialize the output data buffer
    output_data_ = input.get<DynamicData>().create_data();
}

void ShapesSplitter::on_data_available(
        rti::routing::processor::Route& route)
{
    // Split input shapes  into mono-dimensional output shapes
    auto input_samples = route.input<DynamicData>(0).take();
    for (auto sample : input_samples) {
        if (sample.info().valid()) {
            // split into first output
            output_data_ = sample.data();
            output_data_.get().value<int32_t>("y", 0);
            route.output<DynamicData>(0).write(
                    output_data_.get(),
                    sample.info());
            // split into second output
            output_data_ = sample.data();
            output_data_.get().value<int32_t>("x", 0);
            route.output<DynamicData>(1).write(
                    output_data_.get(),
                    sample.info());
        } else {
            // propagate dispose 
            route.output<DynamicData>(0).write(
                    output_data_.get(),
                    sample.info());
            route.output<DynamicData>(1).write(
                    output_data_.get(),
                    sample.info());
        }
    }
}

/*
 * --- ShapesProcessorPlugin --------------------------------------------------
 */

const std::string ShapesProcessorPlugin::PROCESSOR_KIND_PROPERTY_NAME =
        "shapes_processor.kind";

const std::string ShapesProcessorPlugin::PROCESSOR_LEADING_INPUT_PROPERTY_NAME =
        "shapes_processor.leading_input_index";

const std::string ShapesProcessorPlugin::PROCESSOR_AGGREGATOR_SIMPLE_NAME =
        "aggregator_simple";

const std::string ShapesProcessorPlugin::PROCESSOR_AGGREGATOR_ADV_NAME =
        "aggregator_adv";

const std::string ShapesProcessorPlugin::PROCESSOR_SPLITTER_NAME =
        "splitter";

ShapesProcessorPlugin::ShapesProcessorPlugin(
        const rti::routing::PropertySet&)
{
    
}


rti::routing::processor::Processor* ShapesProcessorPlugin::create_processor(
        rti::routing::processor::Route&,
        const rti::routing::PropertySet& properties)
{
    PropertySet::const_iterator it = properties.find(PROCESSOR_KIND_PROPERTY_NAME);
    if (it != properties.end()) {
        if (it->second == PROCESSOR_AGGREGATOR_ADV_NAME) {
            int32_t leading_input_index = 0;

            it = properties.find(PROCESSOR_LEADING_INPUT_PROPERTY_NAME);
            if (it != properties.end()) {
                std::stringstream stream;

                stream << it->second;
                stream >> leading_input_index;
            }

            return new ShapesAggregatorAdv(leading_input_index);
        } else  if (it->second == PROCESSOR_AGGREGATOR_SIMPLE_NAME) {
            return new ShapesAggregatorSimple();
        } else if (it->second != PROCESSOR_SPLITTER_NAME) {
            throw dds::core::InvalidArgumentError(
                    "unknown processor kind with name=" + it->second);
        }
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