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

#include "ShapesSplitter.hpp"

using namespace rti::routing;
using namespace rti::routing::processor;
using namespace rti::routing::adapter;
using namespace dds::core::xtypes;
using namespace dds::sub::status;


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
        rti::routing::processor::Route &route,
        rti::routing::processor::Input &input)
{
    // The type this processor works with is the ShapeType, which shall
    // be the type the input as well as the two outputs. Hence we can use
    // the input type to initialize the output data buffer
    output_data_ = input.get<DynamicData>().create_data();
}

void ShapesSplitter::on_data_available(rti::routing::processor::Route &route)
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
 * --- ShapesSplitterPlugin --------------------------------------------------
 */

ShapesSplitterPlugin::ShapesSplitterPlugin(const rti::routing::PropertySet &)
{
    rti::routing::Logger::instance().local("ShapesSplitter Plugin loaded");
}


rti::routing::processor::Processor *ShapesSplitterPlugin::create_processor(
        rti::routing::processor::Route &,
        const rti::routing::PropertySet &properties)
{
    ti::routing::Logger::instance().local("ShapesSplitter Processor created");
    return new ShapesSplitter();
}

void ShapesSplitterPlugin::delete_processor(
        rti::routing::processor::Route &,
        rti::routing::processor::Processor *processor)
{
    delete processor;
}


RTI_PROCESSOR_PLUGIN_CREATE_FUNCTION_DEF(ShapesSplitterPlugin);