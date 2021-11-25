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
using System.Threading;
using System.Threading.Tasks;
using Rti.Dds.Domain;
using Rti.Dds.Publication;
using Rti.Dds.Topics;
using Rti.Types.Dynamic;

namespace DynamicDataExample
{
    /// <summary>
    /// Example application that publishes ShapeType
    /// </summary>
    public sealed class ShapeTypePublisher : IShapeTypeApplication
    {
        private readonly DomainParticipant participant;
        private readonly DataWriter<DynamicData> writer;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Publisher and DataWriter.
        /// </summary>
        public ShapeTypePublisher(int domainId, string typeSource = "build")
        {
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // Get the ShapeType definition using one of the available options
            DynamicType type = ShapeTypeHelper.GetShapeType(typeSource);

            Topic<DynamicData> topic = participant.CreateTopic("Square", type);

            Publisher publisher = participant.CreatePublisher();
            writer = publisher.CreateDataWriter(topic);
        }

        /// <summary>
        /// Publishes the data
        /// </summary>
        public async Task Run(int sampleCount, CancellationToken cancellationToken)
        {
            DynamicData sample = writer.CreateData();
            sample.SetValue("color", "BLUE");
            sample.SetValue("y", 100); // we'll use a constant y coordinate

            if (sample.MemberExistsInType("angle")) // extended type?
            {
                sample.SetValue("angle", 45.0f); // angle is a float

                // SetAnyValue can convert between types. In this case it can
                // translate a enumerator name to its integer value
                sample.SetAnyValue("fillKind", "TRANSPARENT_FILL");
            }

            int direction = 1;
            int x = 50;

            for (int count = 0;
                count < sampleCount && !cancellationToken.IsCancellationRequested;
                count++)
            {
                // Set the shape size from 30 to 50
                int size = 30 + (count % 20);
                sample.SetValue("shapesize", size);

                // Set the x coordinate
                sample.SetValue("x", x);

                Console.WriteLine($"Writing [shapesize={size}, x = {x}]");
                writer.Write(sample);

                // Update the x coordinate so it changes directions when it
                // hits a border.
                x += direction * 2;
                if (x >= 150)
                {
                    direction = -1;
                }
                else if (x <= 50)
                {
                    direction = 1;
                }

                await Task.Delay(100, cancellationToken);
            }
        }

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();
    }
} // namespace DynamicDataExample
