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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <csignal>
#include <climits>

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

    enum ParseReturn { PARSE_RETURN_OK, PARSE_RETURN_FAILURE, PARSE_RETURN_EXIT };

    struct ApplicationArguments {
        ParseReturn parse_result;
        unsigned int domain_id;
        unsigned int sample_count;
        NDDS_Config_LogVerbosity verbosity;
    };

    inline void set_verbosity(
        ApplicationArguments& arguments,
        int verbosity)
    {
        switch (verbosity) {
            case 0:
            arguments.verbosity = NDDS_CONFIG_LOG_VERBOSITY_SILENT;
            break;
            case 1:
            arguments.verbosity = NDDS_CONFIG_LOG_VERBOSITY_ERROR;
            break;
            case 2:
            arguments.verbosity = NDDS_CONFIG_LOG_VERBOSITY_WARNING;
            break;
            case 3:
            arguments.verbosity = NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL;
            break;
            default:
            arguments.verbosity = NDDS_CONFIG_LOG_VERBOSITY_ERROR;
            break;
        }
    }

    // Parses application arguments for example.  Returns whether to exit.
    inline void parse_arguments(
        ApplicationArguments& arguments,
        int argc,
        char *argv[])
    {
        int arg_processing = 1;
        bool show_usage = false;
        arguments.domain_id = 0;
        arguments.sample_count = INT_MAX;
        arguments.verbosity = NDDS_CONFIG_LOG_VERBOSITY_ERROR;
        arguments.parse_result = PARSE_RETURN_OK;

        while (arg_processing < argc) {
            if ((argc > arg_processing + 1) 
            && (strcmp(argv[arg_processing], "-d") == 0
            || strcmp(argv[arg_processing], "--domain") == 0)) {
                arguments.domain_id = atoi(argv[arg_processing + 1]);
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-s") == 0
            || strcmp(argv[arg_processing], "--sample-count") == 0)) {
                arguments.sample_count = atoi(argv[arg_processing + 1]);
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-v") == 0
            || strcmp(argv[arg_processing], "--verbosity") == 0)) {
                set_verbosity(arguments, atoi(argv[arg_processing + 1]));
                arg_processing += 2;
            } else if (strcmp(argv[arg_processing], "-h") == 0
            || strcmp(argv[arg_processing], "--help") == 0) {
                std::cout << "Example application." << std::endl;
                show_usage = true;
                arguments.parse_result = PARSE_RETURN_EXIT;
                break;
            } else {
                std::cout << "Bad parameter." << std::endl;
                show_usage = true;
                arguments.parse_result = PARSE_RETURN_FAILURE;
                break;
            }
        }
        if (show_usage) {
            std::cout << "Usage:\n"\
            "    -d, --domain       <int>   Domain ID this application will\n" \
            "                               subscribe in.  \n"
            "                               Default: 0\n"\
            "    -s, --sample_count <int>   Number of samples to receive before\n"\
            "                               cleanly shutting down. \n"
            "                               Default: infinite\n"
            "    -v, --verbosity    <int>   How much debugging output to show.\n"\
            "                               Range: 0-3 \n"
            "                               Default: 1"
            << std::endl;
        }
    }

}  // namespace application

#endif  // APPLICATION_H
