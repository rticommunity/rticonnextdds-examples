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

namespace PrimeCalculator
{
    /// <summary>
    /// Interface shared by PrimesRequester and PrimesReplier.
    /// </summary>
    public interface IApplication : IDisposable
    {
        /// <summary>
        /// Runs the application asynchronously.
        /// </summary>
        Task Run(CancellationToken cancellationToken);
    }

    /// <summary>
    /// Runs PrimesRequester or PrimesReplier.
    /// </summary>
    public static class PrimeNumberReplyProgram
    {
        /// <summary>
        /// The Main function runs the requester or the replier.
        /// </summary>
        public static async Task Main(string[] args)
        {
            var arguments = ParseArguments(args);
            if (arguments == null)
            {
                return;
            }

            // Run the requester, the replier, or both.
            var applications = new List<IApplication>();
            if (arguments.Requester)
            {
                Console.WriteLine($"Running PrimesRequester on domain {arguments.Domain}");
                applications.Add(new PrimesRequester(
                    arguments.Domain,
                    arguments.N,
                    arguments.PrimesPerReply));
            }

            if (arguments.Replier)
            {
                Console.WriteLine($"Running PrimesReplier on domain {arguments.Domain}");
                applications.Add(new PrimesReplier(arguments.Domain));
            }

            // Set up signal handler to Dispose the DDS entities
            var cancellationSource = arguments.Timeout > 0
                ? new CancellationTokenSource(TimeSpan.FromSeconds(arguments.Timeout))
                : new CancellationTokenSource();
            Console.CancelKeyPress += (_, eventArgs) =>
            {
                Console.WriteLine("Shutting down...");
                eventArgs.Cancel = true;
                cancellationSource.Cancel();
            };

            try
            {
                // Run the applications and await until they finish.
                await Task.WhenAll(
                    applications.Select(
                        app => app.Run(cancellationSource.Token)
                    ).ToArray()
                );
            }
            catch (TaskCanceledException)
            {
                // ignore
            }
            finally
            {
                foreach (var app in applications)
                {
                    app.Dispose();
                }
            }
        }

        private class Arguments
        {
            public bool Requester { get; set; }
            public bool Replier { get; set; }
            public int Domain { get; set; }
            public int N { get; set; } = int.MaxValue;
            public int PrimesPerReply { get; set; }
            public int Timeout { get; set; }
        }

        // Uses the System.CommandLine package to parse the program arguments.
        private static Arguments ParseArguments(string[] args)
        {
            // Create a root command with some options
            var rootCommand = new System.CommandLine.RootCommand
            {
                new System.CommandLine.Option<bool>(
                    new string[] { "--requester" },
                description: "Whether to run the requester application"),
                new System.CommandLine.Option<bool>(
                    new string[] { "--replier" },
                description: "Whether to run the replier application"),
                new System.CommandLine.Option<int>(
                    new string[] { "--domain", "-d" },
                getDefaultValue: () => 0,
                description: "Domain ID used to create the DomainParticipant"),
                new System.CommandLine.Option<int>(
                    new string[] { "--number", "-n" },
                getDefaultValue: () => 0,
                description: "The number to calculate primes up to (only applicable with --requester)"),
                new System.CommandLine.Option<int>(
                    new string[] { "--primes-per-reply", "-p" },
                getDefaultValue: () => 5),
                new System.CommandLine.Option<int>(
                    new string[] { "--timeout" },
                getDefaultValue: () => 0,
                description: "Timeout in seconds to wait for the application to finish (default: infinite)"),
            };

            rootCommand.Description = "Example RTI Connext Requester and Replier";

            Arguments result = null;
            rootCommand.Handler = System.CommandLine.Invocation.CommandHandler.Create(
                (Arguments arguments) => result = arguments);

            System.CommandLine.CommandExtensions.Invoke(rootCommand, args);

            if (result == null)
            {
                return null;
            }

            if (!result.Requester && !result.Replier)
            {
                Console.WriteLine(rootCommand.Description);
                Console.Write("Which one do you want to run? Enter 'requester' or 'replier' > ");
                var choice = Console.ReadLine();
                result.Requester = choice.StartsWith("req", StringComparison.OrdinalIgnoreCase);
            }

            if (result.Requester && result.N <= 0)
            {
                Console.WriteLine(rootCommand.Description);
                Console.Write("Enter the number to calculate primes up to > ");
                var choice = Console.ReadLine();
                result.N = int.Parse(choice);
            }

            return result;
        }
    }
} // namespace PrimeCalculator
