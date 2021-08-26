/*
* (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

using System;
using Rti.Dds.Core;
using Rti.Types.Dynamic;

namespace DynamicDataExample
{
    /// <summary>
    /// Helper functions that show different ways to get or build ShapeType
    /// </summary>
    public static class ShapeTypeHelper
    {
        /// <summary>
        /// Uses one of the different ways to get the ShapeType definition
        /// </summary>
        /// <param name="typeSource">Picks one of the available ways to get the type</param>
        public static DynamicType GetShapeType(string typeSource)
            => typeSource switch
            {
                "idl" => GetShapeTypeFromIdlDefinition(),
                "xml" => LoadShapeTypeFromXmlDefinition(),
                "build" => BuildShapeType(),
                "extended" => BuildExtendedShapeType(),
                _ => throw new ArgumentException($"Invalid typeSource='{typeSource}'")
            };

        /// <summary>
        /// Get the type generated from Shape.idl
        /// </summary>
        public static DynamicType GetShapeTypeFromIdlDefinition()
        {
            // The code generated from Shapes.idl provides the type.
            return ShapeTypeSupport.Instance.DynamicType;
        }

        /// <summary>
        /// Get the type from an XML definition
        /// </summary>
        public static DynamicType LoadShapeTypeFromXmlDefinition()
        {
            // To generate the XML definition of the type, Shapes.xml, run:
            //   $ rtiddsgen -convertToXml Shapes.idl
            var provider = new QosProvider("Shapes.xml");
            return provider.GetType("ShapeType");
        }

        /// <summary>
        /// Build the type dynamically
        /// </summary>
        public static DynamicType BuildShapeType()
        {
            var factory = DynamicTypeFactory.Instance;
            return factory.BuildStruct()
                .WithName("ShapeType")
                .AddMember(new StructMember("color", factory.CreateString(128), isKey: true))
                .AddMember(new StructMember("x", factory.GetPrimitiveType<int>()))
                .AddMember(new StructMember("y", factory.GetPrimitiveType<int>()))
                .AddMember(new StructMember("shapesize", factory.GetPrimitiveType<int>()))
                .Create();
        }

        /// <summary>
        /// Creates a type based on <see cref="GetShapeTypeFromIdlDefinition"/>
        /// with two extra fields: fillKind and angle
        /// </summary>
        public static DynamicType BuildExtendedShapeType()
        {
            var factory = DynamicTypeFactory.Instance;

            // Create enum ShapeFillKind
            var shapeFillKind = factory.BuildEnum()
                .WithName("ShapeFillKind")
                .AddMember(new EnumMember("SOLID_FILL", ordinal: 0))
                .AddMember(new EnumMember("TRANSPARENT_FILL", ordinal: 1))
                .AddMember(new EnumMember("HORIZONTAL_HATCH_FILL", ordinal: 2))
                .AddMember(new EnumMember("VERTICAL_HATCH_FILL", ordinal: 3))
                .Create();

            var startingType = (StructType) GetShapeTypeFromIdlDefinition();

            // Copies the definition of startingType and adds two extra
            // members, creating a new type
            return factory.BuildStruct(startingType)
                .AddMember(new StructMember("fillKind", shapeFillKind))
                .AddMember(new StructMember("angle", factory.GetPrimitiveType<float>()))
                .Create();
        }
    }
} // namespace DynamicDataExample
