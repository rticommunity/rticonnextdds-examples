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
#include <iterator>
#include <stdio.h>
#include <stdlib.h>

#include <dds/core/corefwd.hpp>

#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/core/xtypes/StructType.hpp>
#include <rti/routing/Logger.hpp>
#include <rti/routing/processor/Processor.hpp>
#include <rti/routing/processor/ProcessorPlugin.hpp>

#include "ShapesAggregator.hpp"

using namespace rti::routing;
using namespace rti::routing::processor;
using namespace rti::routing::adapter;
using namespace dds::core::xtypes;
using namespace dds::sub::status;


/*
 * --- ShapesAggregator -------------------------------------------------
 */

ShapesAggregator::ShapesAggregator()
{
}

ShapesAggregator::~ShapesAggregator()
{
}

void ShapesAggregator::on_output_enabled(
        rti::routing::processor::Route &route,
        rti::routing::processor::Output &output)
{
    // initialize the output_data buffer using the type from the output
    output_data_ = output.get<DynamicData>().create_data();
}

void ShapesAggregator::on_data_available(
        rti::routing::processor::Route &route)
{
    // Use squares as 'leading' input. For each Square instance, get the
    // equivalent instance from the Circle topic
    auto squares = route.input<DynamicData>("Square").read();
    for (auto square_sample : squares) {
        if (square_sample.info().valid()) {
            output_data_ = square_sample.data();
            // read equivalent existing instance in the Circles stream
            auto circles =
                    route.input<DynamicData>("Circle")
                            .select()
                            .instance(square_sample.info().instance_handle())
                            .read();
            if (circles.length() != 0 && circles[0].info().valid()) {
                output_data_.get().value<int32_t>(
                        "shapesize",
                        circles[0].data().value<int32_t>("y"));
            }
            // Write aggregated sample intro Triangles
            route.output<DynamicData>("Triangle").write(output_data_.get());
        } else {
            // propagate the dispose
            route.output<DynamicData>("Triangle")
                    .write(output_data_.get(), square_sample.info());
            // clear instance instance
            route.input<DynamicData>("Square")
                    .select()
                    .instance(square_sample.info().instance_handle())
                    .take();
        }
    }
}


/*
 * --- ShapesAggregatorPlugin --------------------------------------------------
 */


ShapesAggregatorPlugin::ShapesAggregatorPlugin(const rti::routing::PropertySet &)
{
    rti::routing::Logger::instance().local("ShapesAggregator Plugin loaded");
}


rti::routing::processor::Processor *ShapesAggregatorPlugin::create_processor(
        rti::routing::processor::Route &,
        const rti::routing::PropertySet &)
{
    rti::routing::Logger::instance().local("ShapesAggregator Processor created");
    return new ShapesAggregator();
}

void ShapesAggregatorPlugin::delete_processor(
        rti::routing::processor::Route &,
        rti::routing::processor::Processor *processor)
{
    delete processor;
}


RTI_PROCESSOR_PLUGIN_CREATE_FUNCTION_DEF(ShapesAggregatorPlugin);