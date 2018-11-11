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

class ShapesAggregrator :
        public rti::routing::processor::NoOpProcessor {

public:
    void on_data_available(rti::routing::processor::Route&);

    void on_output_enabled(
            rti::routing::processor::Route& route,
            rti::routing::processor::Output& output);

    ShapesAggregrator();

    ~ShapesAggregrator();
    
private:
    void get_circles_data(
            rti::routing::processor::LoanedSamples<
                    dds::core::xtypes::DynamicData>& circles,
            const dds::core::InstanceHandle& handle);
    
    dds::core::xtypes::DynamicData output_data_;
};

class ShapesSplitter :
        public rti::routing::processor::NoOpProcessor {

public:
    void on_data_available(rti::routing::processor::Route&);

    void on_output_enabled(
            rti::routing::processor::Route& route,
            rti::routing::processor::Output& output);

    ShapesSplitter();

    ~ShapesSplitter();

private:
    dds::core::xtypes::DynamicData output_circle_;
    dds::core::xtypes::DynamicData output_triangle_;
};

class ShapesProcessorPlugin :
        public rti::routing::processor::ProcessorPlugin {
                  
public:        
    static const std::string PROCESSOR_KIND_PROPERTY_NAME;
    static const std::string PROCESSOR_AGGREGATOR_NAME;
    
    rti::routing::processor::Processor* create_processor(
            rti::routing::processor::Route& route,
            const rti::routing::PropertySet& properties) RTI_OVERRIDE;

    void delete_processor(
        rti::routing::processor::Route& route,
        rti::routing::processor::Processor *processor) RTI_OVERRIDE; 
    
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