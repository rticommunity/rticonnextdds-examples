/*
 * (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <iostream>
#include <csignal>
#include <dds/core/ddscore.hpp>

namespace application {

// Catch control-C and tell application to shut down
bool shutdown_requested = false;

inline void stop_handler(int)
{
    shutdown_requested = true;
    std::cout << "preparing to shut down..." << std::endl;
}

inline void setup_signal_handlers()
{
    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);
}

enum class ParseReturn { ok, failure, exit };

enum class ApplicationKind { publisher, subscriber };

struct ApplicationArguments {
    ParseReturn parse_result;
    unsigned int domain_id;
    unsigned int sample_count;
    rti::config::Verbosity verbosity;
    unsigned int mode;
    unsigned int execution_time;
    bool display_sample;
    std::string nic;

    ApplicationArguments(
            ParseReturn parse_result_param,
            unsigned int domain_id_param,
            unsigned int sample_count_param,
            rti::config::Verbosity verbosity_param,
            unsigned int mode_param,
            unsigned int execution_time_param,
            bool display_sample_param,
            std::string nic_param)
            : parse_result(parse_result_param),
              domain_id(domain_id_param),
              sample_count(sample_count_param),
              verbosity(verbosity_param),
              mode(mode_param),
              execution_time(execution_time_param),
              display_sample(display_sample_param),
              nic(nic_param)
    {
    }
};

inline void set_verbosity(
        rti::config::Verbosity &verbosity,
        int verbosity_value)
{
    switch (verbosity_value) {
    case 0:
        verbosity = rti::config::Verbosity::SILENT;
        break;
    case 1:
        verbosity = rti::config::Verbosity::EXCEPTION;
        break;
    case 2:
        verbosity = rti::config::Verbosity::WARNING;
        break;
    case 3:
        verbosity = rti::config::Verbosity::STATUS_ALL;
        break;
    default:
        verbosity = rti::config::Verbosity::EXCEPTION;
        break;
    }
}

// Parses application arguments for example.
inline ApplicationArguments parse_arguments(
        int argc,
        char *argv[],
        ApplicationKind current_application)
{
    int arg_processing = 1;
    bool show_usage = false;
    ParseReturn parse_result = ParseReturn::ok;
    unsigned int domain_id = 0;
    unsigned int sample_count = (std::numeric_limits<unsigned int>::max)();
    rti::config::Verbosity verbosity(rti::config::Verbosity::EXCEPTION);
    unsigned int mode = 1;
    bool display_sample = false;
    unsigned int execution_time = 30000000;
    std::string nic;


    while (arg_processing < argc) {
        if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-d") == 0
                || strcmp(argv[arg_processing], "--domain") == 0)) {
            domain_id = atoi(argv[arg_processing + 1]);
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-m") == 0
                    || strcmp(argv[arg_processing], "--mode") == 0)) {
            mode = atoi(argv[arg_processing + 1]);
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (current_application == ApplicationKind::publisher)
                && (strcmp(argv[arg_processing], "-s") == 0
                    || strcmp(argv[arg_processing], "--sample_count") == 0)) {
            sample_count = atoi(argv[arg_processing + 1]);
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (current_application == ApplicationKind::publisher)
                && (strcmp(argv[arg_processing], "-e") == 0
                    || strcmp(argv[arg_processing], "--exec_time") == 0)) {
            execution_time = atoi(argv[arg_processing + 1]);
            arg_processing += 2;
        } else if (
                (current_application == ApplicationKind::subscriber)
                && (strcmp(argv[arg_processing], "-ds") == 0
                    || strcmp(argv[arg_processing], "--display_samples") == 0)) {
            display_sample = true;
            arg_processing += 1;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-n") == 0
                    || strcmp(argv[arg_processing], "--nic") == 0)) {
            nic = argv[arg_processing + 1];
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-v") == 0
                    || strcmp(argv[arg_processing], "--verbosity") == 0)) {
            set_verbosity(verbosity, atoi(argv[arg_processing + 1]));
            arg_processing += 2;
        } else if (
                strcmp(argv[arg_processing], "-h") == 0
                || strcmp(argv[arg_processing], "--help") == 0) {
            std::cout << "Example application." << std::endl;
            show_usage = true;
            parse_result = ParseReturn::exit;
            break;
        } else {
            std::cout << "Bad parameter." << std::endl;
            show_usage = true;
            parse_result = ParseReturn::failure;
            break;
        }
    }
    if (show_usage) {
        std::cout
                << "Usage:\n"
                   "    -d, --domain       <int>   Domain ID this "
                   "application will\n"
                   "                               subscribe in.  \n"
                   "                               Default: 0\n"
                   "    -m, --mode         <int>   Publisher modes\n"
                   "                                   1. publisher_flat\n"
                   "                                   2. publisher_zero_copy\n"
                   "                                   3. "
                   "publisher_flat_zero_copy\n"
                   "                                   4. publisher_plain\n"
                   "                               Default: 1\n";

        if (current_application == ApplicationKind::publisher) {
            std::cout
                    << "    -s, --sample_count <int>   Number of samples to "
                       "receive before\n"
                       "                               cleanly shutting down.\n"
                       "                               Default: infinite\n"
                       "    -e, --exec_time    <int>   Execution time in "
                       "seconds.\n";
        } else {
            std::cout
                    << "    -d, --display_sample       Displays the sample.\n";
        }

        std::cout << "    -n, --nic       <string>   Use the nic specified by "
                     "<ipaddr> to send.\n"
                     "    -v, --verbosity    <int>   How much debugging output "
                     "to show.\n"
                     "                               Range: 0-3 \n"
                     "                               Default: 1"
                  << std::endl;
    }

    return ApplicationArguments(
            parse_result,
            domain_id,
            sample_count,
            verbosity,
            mode,
            execution_time,
            display_sample,
            nic);
}

}  // namespace application

#endif  // APPLICATION_HPP