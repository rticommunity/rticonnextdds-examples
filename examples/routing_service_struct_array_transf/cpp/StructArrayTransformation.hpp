/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

/* ========================================================================= */
/* RTI Routing Service transformation Example                                */
/* ========================================================================= */
/*                                                                           */
/* See SensorDataTransformation.cpp                                          */
/*                                                                           */
/* ========================================================================= */

#ifndef STRUCT_ARRAY_TRANSF_HPP_
#define STRUCT_ARRAY_TRANSF_HPP_

#include <rti/routing/transf/TransformationPlugin.hpp>

/**
 *  @class StructArrayTransformation
 *
 * @brief implementation of the Transformation.
 *                                                                        
 * This example preforms a transformation from type SensorAttributesCollection
 * to SensorData (see SensorData.idl in source folder).
 */
 class StructArrayTransformation :
        public rti::routing::transf::DynamicDataTransformation {

 public:
    void transform(
            std::vector<dds::core::xtypes::DynamicData*>& output_sample_seq,
            std::vector<dds::sub::SampleInfo*>& output_info_seq,
            const std::vector<dds::core::xtypes::DynamicData*>& input_sample_seq,
            const std::vector<dds::sub::SampleInfo*>& input_info_seq);

    void return_loan(
            std::vector<dds::core::xtypes::DynamicData*>& sample_seq,
            std::vector<dds::sub::SampleInfo*>& info_seq);

private:
    dds::core::xtypes::DynamicData* convert_sample(
            const dds::core::xtypes::DynamicData *input_sample);
};


/**
 * @class StructArrayTransformationPlugin
 *
 * @brief This class will be used by Routing Service to create and initialize
 * our custom Transformation Subclass. In this example, that class is 
 * StructArrayTransformation.
 *
 * This class must use the macro
 * RTI_TRANSFORMATION_PLUGIN_CREATE_FUNCTION_DECL(classname) in order to
 * create a C wrapper function that will be the dynamic library entry point
 * used by Routing Service.
 * 
 */

class StructArrayTransformationPlugin :
        public rti::routing::transf::TransformationPlugin {

public:
    StructArrayTransformationPlugin(
            const rti::routing::PropertySet& properties);

    /* 
     * @brief Creates an instance of StructArrayTransformation
     */
    rti::routing::transf::Transformation * create_transformation(
            const rti::routing::TypeInfo& input_type_info,
            const rti::routing::TypeInfo& output_type_info,
            const rti::routing::PropertySet& properties);

    void delete_transformation(
            rti::routing::transf::Transformation *transformation);
};



/**
 * This macro defines a C-linkage symbol that can be used as create function
 * for plug-in registration through XML.
 *
 * The generated symbol has the name:
 *
 * \code
 * StructArrayTransformationPlugin_create_transformation_plugin
 * \endcode
 */
RTI_TRANSFORMATION_PLUGIN_CREATE_FUNCTION_DECL(StructArrayTransformationPlugin)

#endif /* STRUCT_ARRAY_TRANSF_HPP_ */