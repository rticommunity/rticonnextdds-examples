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
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Rti.Dds.Core;
using Rti.Dds.Domain;
using Rti.Dds.RequestReply;
using Rti.Dds.Subscription;

namespace PrimeCalculator
{
    /// <summary>
    /// Example application that subscribes to PrimeNumberReply.
    /// </summary>
    public sealed class PrimesReplier : IApplication
    {
        private const string qosProfileName =
            "RequestReplyExampleProfiles::ReplierExampleProfile";
        private readonly DomainParticipant participant;
        private readonly Replier<PrimeNumberRequest, PrimeNumberReply> replier;

        /// <summary>
        /// Creates a DomainParticipant, Topic, Replier and DataReader<PrimeNumberReply>.
        /// </summary>
        public PrimesReplier(int domainId)
        {
            // The replier is created within a DomainParticipant
            participant = DomainParticipantFactory.Instance
                .CreateParticipant(domainId);

            var qosProvider = QosProvider.Default;

            // Create a Replier with a service name, and a QoS profile
            //
            // BuildReplier is a DomainParticipant extension method included
            // in the namespace Rti.Dds.RequestReply. It returns a builder
            // that allows configuring the arguments to create the Replier.
            replier = participant.BuildReplier<PrimeNumberRequest, PrimeNumberReply>()
                .WithServiceName("PrimeCalculator")
                .WithDataWriterQos(qosProvider.GetDataWriterQos(qosProfileName))
                .WithDataReaderQos(qosProvider.GetDataReaderQos(qosProfileName))
                .Create();
        }

        /// <summary>
        /// Processes the data received by the DataReader.
        /// </summary>
        public async Task Run(CancellationToken cancellationToken)
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                await replier.WaitForRequestsAsync(1, cancellationToken);

                using var requests = replier.TakeRequests();
                foreach (var request in requests)
                {
                    if (!request.Info.ValidData)
                    {
                        continue;
                    }

                    if (request.Data.n <= 0 ||
                        request.Data.primes_per_reply <= 0 ||
                        request.Data.primes_per_reply > PRIME_SEQUENCE_MAX_LENGTH.Value)
                    {
                        Console.WriteLine($"Cannot process request: {request}");

                        // Send a reply indicating an error
                        var errorReply = new PrimeNumberReply
                        {
                            status = PrimeNumberCalculationStatus.REPLY_ERROR
                        };
                        replier.SendReply(errorReply, request.Info);
                    }
                    else
                    {
                        Console.WriteLine($"Calculating prime numbers below {request.Data.n}...");
                        CalculateAndSendPrimes(replier, request);
                        Console.WriteLine("DONE");
                    }
                }
            }
        }

        private static void CalculateAndSendPrimes(
                    Replier<PrimeNumberRequest, PrimeNumberReply> replier,
                    LoanedSample<PrimeNumberRequest> request)
        {
            int n = request.Data.n;
            int primesPerReply = request.Data.primes_per_reply;

            var reply = new PrimeNumberReply();
            reply.primes.Capacity = primesPerReply;
            reply.status = PrimeNumberCalculationStatus.REPLY_IN_PROGRESS;

            // prime[i] indicates if i is a prime number
            // Initially, we assume all of them except 0 and 1 are prime
            var prime = new List<bool>(Enumerable.Repeat(true, n + 1))
            {
                [0] = false,
                [1] = false
            };

            int m = (int)Math.Sqrt(n);

            for (int i = 2; i <= m; i++)
            {
                if (prime[i])
                {
                    for (int j = i * i; j <= n; j += i)
                    {
                        prime[j] = false;
                    }

                    // Add a new prime number to the reply
                    reply.primes.Add(i);

                    if (reply.primes.Count == primesPerReply)
                    {
                        // Send a reply now
                        replier.SendReply(reply, request.Info);
                        reply.primes.Clear();
                    }
                }
            }

            // Calculation is done. Send remaining prime numbers
            for (int i = m + 1; i <= n; i++)
            {
                if (prime[i])
                {
                    reply.primes.Add(i);
                }

                if (reply.primes.Count == primesPerReply)
                {
                    replier.SendReply(reply, request.Info);
                    reply.primes.Clear();
                }
            }

            // Send the last reply. Indicate that the calculation is complete and
            // send any prime number left in the sequence
            reply.status = PrimeNumberCalculationStatus.REPLY_COMPLETED;
            replier.SendReply(reply, request.Info);
        }

        /// <summary>
        /// Disposes the replier and the participant.
        /// </summary>
        public void Dispose()
        {
            replier.Dispose();
            participant.Dispose();
        }
    }
} // namespace PrimeCalculator
