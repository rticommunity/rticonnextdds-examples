/*
 * (c) Copyright, Real-Time Innovations, 2017.
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
#include <dds/core/Optional.hpp>

class ShapesAggregrator :
        public rti::routing::processor::NoOpProcessor {

public:
    void on_data_available(rti::routing::processor::Route&);

    void on_output_enabled(
            rti::routing::processor::Route& route,
            rti::routing::processor::Output& output);

    ShapesAggregrator(int32_t leading_input_index);

    ~ShapesAggregrator();
    
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
    static const std::string PROCESSOR_AGGREGATOR_NAME;
    
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