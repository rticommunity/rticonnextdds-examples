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
using Rti.Dds.Core;
using Rti.Dds.Core.Status;
using Rti.Dds.Domain;
using Rti.Dds.Subscription;
using Rti.Dds.Topics;
using Rti.Types.Dynamic;

namespace DynamicDataExample
{
    /// <summary>
    /// Example application that subscribes to ShapeType.
    /// </summary>
    public sealed class ShapeTypeSubscriber : IShapeTypeApplication
    {
        private readonly DomainParticipant participant;
        private readonly DataReader<DynamicData> reader;

        private int samplesRead;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Subscriber and DataReader<ShapeType>.
        /// </summary>
        public ShapeTypeSubscriber(int domainId, string typeSource = "build")
        {
            participant = DomainParticipantFactory.Instance.CreateParticipant(domainId);

            // Get the ShapeType definition using one of the available options
            DynamicType type = ShapeTypeHelper.GetShapeType(typeSource);

            Topic<DynamicData> topic = participant.CreateTopic("Square", type);

            Subscriber subscriber = participant.CreateSubscriber();
            reader = subscriber.CreateDataReader(topic);
        }

        /// <summary>
        /// Processes the data received by the DataReader.
        /// </summary>
        public async Task Run(int sampleCount, CancellationToken cancellationToken)
        {
            // TakeAsync provides an IAsyncEnumerable that returns new data
            // samples when they are available, awaiting as necessary. The
            // cancellation token allows stopping the loop.
            await foreach (var sample in reader
                .TakeAsync()
                .WithCancellation(cancellationToken))
            {
                if (sample.Info.ValidData)
                {
                    samplesRead++;
                    Console.WriteLine(sample.Data);

                    if (samplesRead == sampleCount)
                    {
                        break;
                    }
                }
            }
        }

        /// <summary>
        /// Disposes all DDS entities created by this application.
        /// </summary>
        public void Dispose() => participant.Dispose();
    }
} // namespace DynamicDataExample
