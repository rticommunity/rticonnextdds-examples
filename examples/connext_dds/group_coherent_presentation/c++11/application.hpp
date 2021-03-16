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

    enum class ParseReturn {
        ok,
        failure,
        exit
    };

    struct ApplicationArguments {
        ParseReturn parse_result;
        unsigned int domain_id;
        unsigned int set_count;
        rti::config::Verbosity verbosity;
        bool use_xml_qos;

        ApplicationArguments(
            ParseReturn parse_result_param,
            unsigned int domain_id_param,
            unsigned int set_count_param,
            rti::config::Verbosity verbosity_param, 
            bool use_xml_qos_param)
            : parse_result(parse_result_param),
            domain_id(domain_id_param),
            set_count(set_count_param),
            verbosity(verbosity_param),
            use_xml_qos(use_xml_qos_param) {}
    };

    inline void set_verbosity(
        rti::config::Verbosity& verbosity,
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
    inline ApplicationArguments parse_arguments(int argc, char *argv[])
    {
        int arg_processing = 1;
        bool show_usage = false;
        ParseReturn parse_result = ParseReturn::ok;
        unsigned int domain_id = 0;
        unsigned int set_count = (std::numeric_limits<unsigned int>::max)();
        rti::config::Verbosity verbosity(rti::config::Verbosity::EXCEPTION);
        bool use_xml_qos = true;

        while (arg_processing < argc) {
            if ((argc > arg_processing + 1) 
            && (strcmp(argv[arg_processing], "-d") == 0
            || strcmp(argv[arg_processing], "--domain") == 0)) {
                domain_id = atoi(argv[arg_processing + 1]);
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-s") == 0
            || strcmp(argv[arg_processing], "--set-count") == 0)) {
                set_count = atoi(argv[arg_processing + 1]);
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-v") == 0
            || strcmp(argv[arg_processing], "--verbosity") == 0)) {
                set_verbosity(verbosity, atoi(argv[arg_processing + 1]));
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-x") == 0
            || strcmp(argv[arg_processing], "--xml_qos") == 0)) {
                use_xml_qos = atoi(argv[arg_processing + 1]) == 0 ? false : true;
                arg_processing += 2;
            } else if (strcmp(argv[arg_processing], "-h") == 0
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
            std::cout << "Usage:\n"\
            "    -d, --domain       <int>   Domain ID this application will\n" \
            "                               join.  \n"
            "                               Default: 0\n"\
            "    -s, --set_count <int>   (publisher only) Number of coherent \n" \
            "                               sets to send before cleanly shutting \n" \
            "                               down. \n"
            "                               Default: infinite\n"
            "    -v, --verbosity    <int>   Logging verbosity.\n"\
            "                               Range: 0-3 \n"
            "                               Default: 1\n"
            "    -x, --xml_qos     <0|1>    Whether to set the QoS using XML or programatically.\n"\
            "                               Default: 1 (use XML) \n"
            << std::endl;
        }

        return ApplicationArguments(
                parse_result, domain_id, set_count, verbosity, use_xml_qos);
    }

}  // namespace application

#endif  // APPLICATION_HPP