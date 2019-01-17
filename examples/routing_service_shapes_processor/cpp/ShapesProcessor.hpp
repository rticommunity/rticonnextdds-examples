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

#ifndef SHAPES_PROCESSOR_HPP_
#define SHAPES_PROCESSOR_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <iterator>

#include <dds/core/corefwd.hpp>

#include <rti/routing/processor/ProcessorPlugin.hpp>
#include <rti/routing/processor/Processor.hpp>
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/core/xtypes/StructType.hpp>
#include <dds/core/Optional.hpp>

class ShapesAggregatorSimple :
        public rti::routing::processor::NoOpProcessor {

public:
    void on_data_available(rti::routing::processor::Route&);

    void on_output_enabled(
            rti::routing::processor::Route& route,
            rti::routing::processor::Output& output);

    ShapesAggregatorSimple();

    ~ShapesAggregatorSimple();

private:
    // Optional member for deferred initialization: this object can be created
    // only when the output is enabled.
    // You can use std::optional if supported in your platform
    dds::core::optional<dds::core::xtypes::DynamicData> output_data_;
};


class ShapesAggregatorAdv :
        public rti::routing::processor::NoOpProcessor {

public:
    void on_data_available(rti::routing::processor::Route&);

    void on_output_enabled(
            rti::routing::processor::Route& route,
            rti::routing::processor::Output& output);

    ShapesAggregatorAdv(int32_t leading_input_index);

    ~ShapesAggregatorAdv();
    
private:
    // Optional member for deferred initialization: this object can be created
    // only when the output is enabled.
    // You can use std::optional if supported in your platform
    dds::core::optional<dds::core::xtypes::DynamicData> output_data_;
    // index of leading input from which data is read first
    int32_t leading_input_index_;
};

class ShapesSplitter :
        public rti::routing::processor::NoOpProcessor {

public:
    void on_data_available(rti::routing::processor::Route&) override;

    void on_input_enabled(
            rti::routing::processor::Route& route,
            rti::routing::processor::Input& input) override;

    ShapesSplitter();

    ~ShapesSplitter();

private:
    // Optional member for deferred initialization: this object can be created
    // only when the output is enabled.
    // You can use std::optional if supported in your platform
    dds::core::optional<dds::core::xtypes::DynamicData> output_data_;
};

class ShapesProcessorPlugin :
        public rti::routing::processor::ProcessorPlugin {
                  
public:        
    static const std::string PROCESSOR_KIND_PROPERTY_NAME;
    static const std::string PROCESSOR_LEADING_INPUT_PROPERTY_NAME;
    static const std::string PROCESSOR_AGGREGATOR_SIMPLE_NAME;
    static const std::string PROCESSOR_AGGREGATOR_ADV_NAME;
    static const std::string PROCESSOR_SPLITTER_NAME;
    
    rti::routing::processor::Processor* create_processor(
            rti::routing::processor::Route& route,
            const rti::routing::PropertySet& properties) override;

    void delete_processor(
            rti::routing::processor::Route& route,
            rti::routing::processor::Processor *processor) override;
    
    ShapesProcessorPlugin(
            const rti::routing::PropertySet& properties);

};


/**
 * This macro defines a C-linkage symbol that can be used as create function
 * for plug-in registration through XML.
 *
 * The generated symbol has the name:
 * 
 * \code
 * ShapesProcessorPlugin_create_processor_plugin
 * \endcode
 */
RTI_PROCESSOR_PLUGIN_CREATE_FUNCTION_DECL(ShapesProcessorPlugin);

#endif