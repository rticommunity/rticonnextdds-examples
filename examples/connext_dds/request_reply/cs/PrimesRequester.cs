/*
* (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
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
using Rti.Dds.Domain;
using Rti.RequestReply;
using Rti.Dds.Subscription;

namespace PrimeCalculator
{
    /// <summary>
    /// Example application that publishes PrimeNumberReply
    /// </summary>
    public sealed class PrimesRequester : IApplication
    {
        private const string qosProfileName =
            "RequestReplyExampleProfiles::RequesterExampleProfile";
        private readonly DomainParticipant participant;
        private readonly Requester<PrimeNumberRequest, PrimeNumberReply> requester;
        private readonly int n;
        private readonly int primesPerReply;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Requester and DataWriter<PrimeNumberReply>.
        /// </summary>
        public PrimesRequester(int domainId, int n, int primesPerReply)
        {
            this.n = n;
            this.primesPerReply = primesPerReply;

            // The requester is created within a DomainParticipant
            participant = DomainParticipantFactory.Instance
                .CreateParticipant(domainId);

            var qosProvider = QosProvider.Default;

            // Create a Requester with a service name, and a QoS profile
            //
            // BuildRequester is a DomainParticipant extension method included
            // in the namespace Rti.Dds.RequestReply. It returns a builder
            // that allows configuring the arguments to create the Requester.
            requester = participant.BuildRequester<PrimeNumberRequest, PrimeNumberReply>()
                .WithServiceName("PrimeCalculator")
                .WithDataWriterQos(qosProvider.GetDataWriterQos(qosProfileName))
                .WithDataReaderQos(qosProvider.GetDataReaderQos(qosProfileName))
                .Create();
        }

        /// <summary>
        /// Sends the request, waits for the replies asynchronously, and prints
        /// the results.
        /// </summary>
        public async Task Run(CancellationToken cancellationToken)
        {
            // Wait for at least one replier
            while (requester.GetMatchedReplierCount() == 0)
            {
                await Task.Delay(millisecondsDelay: 100, cancellationToken);
            }

            requester.SendRequest(new PrimeNumberRequest(n, primesPerReply));

            bool inProgress = true;
            while (inProgress)
            {
                await requester.WaitForRepliesAsync(1, cancellationToken);

                using LoanedSamples<PrimeNumberReply> replies =
                    requester.TakeReplies();

                // Print the prime numbers we receive
                foreach (var reply in replies.ValidData())
                {
                    foreach (var prime in reply.primes)
                    {
                        Console.Write($"{prime} ");
                    }

                    if (reply.status != PrimeNumberCalculationStatus.REPLY_IN_PROGRESS)
                    {
                        inProgress = false;
                        if (reply.status == PrimeNumberCalculationStatus.REPLY_ERROR)
                        {
                            Console.Write("Replier returned an error");
                        }
                        else
                        {
                            Console.Write("DONE");
                        }
                    }

                    Console.WriteLine();
                }
            }
        }

        /// <summary>
        /// Disposes the requester and the participant.
        /// </summary>
        public void Dispose()
        {
            requester.Dispose();
            participant.Dispose();
        }
    }
} // namespace PrimeCalculator
