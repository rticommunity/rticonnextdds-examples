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
#include "StructArrayTransformation.hpp"
#include "SensorData.hpp"


using dds::core::xtypes::DynamicData;
using dds::sub::SampleInfo;
    
/*
 * @brief Maps each element of struct SensorData from
 * SensorDataExample (see SensorDataExample.idl) into the separate
 * arrays of TransformedSensorDataExample
 * (see TransformedSensorDataExample.idl).
 *
 */
DynamicData* StructArrayTransformation::convert_sample(
        const DynamicData* input_sample)
{    
    SensorAttributesCollection native_collection =
            rti::core::xtypes::convert<SensorAttributesCollection>(*input_sample);

    SensorData native_sensor_data;
    
    /* Map elements from SensorAttributesCollection to SensorData */
    for (int32_t count = 0;
         count < native_collection.sensor_array().size();
         ++count) {
        native_sensor_data.id().at(count) =
                native_collection.sensor_array().at(count).id();
        native_sensor_data.value().at(count) =
                native_collection.sensor_array().at(count).value();
        native_sensor_data.is_active().at(count) =
                native_collection.sensor_array().at(count).is_active();
    }
    
    return new DynamicData(rti::core::xtypes::convert(native_sensor_data));
}

void StructArrayTransformation::transform(
        std::vector<DynamicData*>& output_sample_seq,
        std::vector<SampleInfo*>& output_info_seq,
        const std::vector<DynamicData*>& input_sample_seq,
        const std::vector<SampleInfo*>& input_info_seq)
{


    // resize the output sample and info sequences to hold as many samples
    // as the input sequences
    output_sample_seq.resize(input_sample_seq.size());
    output_info_seq.resize(input_info_seq.size());
    
    // Convert each individual input sample
    for (size_t i = 0; i < input_sample_seq.size(); ++i) {
        // convert data 
        output_sample_seq[i] = convert_sample(input_sample_seq[i]);
        // copy info as is
        output_info_seq[i] = new SampleInfo(*input_info_seq[i]);
    }
}

void StructArrayTransformation::return_loan(
        std::vector<DynamicData*>& sample_seq,
        std::vector<SampleInfo*>& info_seq)
{
    for (size_t i = 0; i < sample_seq.size(); ++i) {
        delete sample_seq[i];
        delete info_seq[i];
    }
}


/*
 * --- StructArrayTransformationPlugin ----------------------------------------
 */
StructArrayTransformationPlugin::StructArrayTransformationPlugin(
        const rti::routing::PropertySet&)
{
    // no configuration properties for this plug-in
}

rti::routing::transf::Transformation *
StructArrayTransformationPlugin::create_transformation(
        const rti::routing::TypeInfo&,
        const rti::routing::TypeInfo&,
        const rti::routing::PropertySet&)
{
    return new StructArrayTransformation();
}

void StructArrayTransformationPlugin::delete_transformation(
        rti::routing::transf::Transformation *transformation) {

    delete transformation;
}


RTI_TRANSFORMATION_PLUGIN_CREATE_FUNCTION_DEF(StructArrayTransformationPlugin);