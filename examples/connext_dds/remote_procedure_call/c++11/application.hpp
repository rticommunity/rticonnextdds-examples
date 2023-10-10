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
#include <rti/config/Logger.hpp>

namespace application {

enum class ParseReturn { ok, failure, exit };

struct ApplicationArguments {
    ParseReturn parse_result;
    unsigned int domain_id;
    bool add;
    std::string item_name;
    unsigned int quantity;
    unsigned int delay;
    rti::config::Verbosity verbosity;

    ApplicationArguments(
            ParseReturn parse_result_param,
            unsigned int domain_id_param,
            bool add_param,
            const std::string &item_name_param,
            unsigned int quantity_param,
            unsigned int delay_param,
            rti::config::Verbosity verbosity_param)
            : parse_result(parse_result_param),
              domain_id(domain_id_param),
              add(add_param),
              item_name(item_name_param),
              quantity(quantity_param),
              delay(delay_param),
              verbosity(verbosity_param)
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
inline ApplicationArguments parse_arguments(int argc, char *argv[], bool client)
{
    int arg_processing = 1;
    bool show_usage = false;
    ParseReturn parse_result = ParseReturn::ok;
    unsigned int domain_id = 0;
    unsigned int quantity = 1;
    unsigned int delay = 0;
    std::string item_name = "";
    bool add = true;
    rti::config::Verbosity verbosity(rti::config::Verbosity::EXCEPTION);

    while (arg_processing < argc) {
        if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-d") == 0
                || strcmp(argv[arg_processing], "--domain") == 0)) {
            domain_id = atoi(argv[arg_processing + 1]);
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-q") == 0
                    || strcmp(argv[arg_processing], "--quantity") == 0)) {
            quantity = atoi(argv[arg_processing + 1]);
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-v") == 0
                    || strcmp(argv[arg_processing], "--verbosity") == 0)) {
            set_verbosity(verbosity, atoi(argv[arg_processing + 1]));
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-a") == 0
                    || strcmp(argv[arg_processing], "--add") == 0)) {
            add = true;
            item_name = argv[arg_processing + 1];
            arg_processing += 2;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-r") == 0
                    || strcmp(argv[arg_processing], "--remove") == 0)) {
            add = false;
            item_name = argv[arg_processing + 1];
            arg_processing += 2;
        } else if (
                strcmp(argv[arg_processing], "-h") == 0
                || strcmp(argv[arg_processing], "--help") == 0) {
            std::cout << "Example application." << std::endl;
            show_usage = true;
            parse_result = ParseReturn::exit;
            break;
        } else if (
                (argc > arg_processing + 1)
                && (strcmp(argv[arg_processing], "-t") == 0
                    || strcmp(argv[arg_processing], "--delay") == 0)) {
            delay = atoi(argv[arg_processing + 1]);
            arg_processing += 2;
        } else {
            std::cout << "Bad parameter." << std::endl;
            show_usage = true;
            parse_result = ParseReturn::failure;
            break;
        }
    }

    if (client && item_name.empty()) {
        std::cout << "Must add or remove an an item name." << std::endl;
        show_usage = true;
        parse_result = ParseReturn::failure;
    }

    if (show_usage) {
        std::cout << "Usage:\n"\
        "    -d, --domain       <int>   Domain ID this application will\n" \
        "                               subscribe in.  \n"
        "                               Default: 0\n"\
        "    -a, --add <item_name>      Add an item to the inventory\n"\
        "    -r, --remove <item_name>   Remove an item from the inventory\n"\
        "    -q, --quantity <int>       Number of items to add or remove\n"\
        "                               Default: 1\n"
        "    -v, --verbosity    <int>   How much debugging output to show.\n"\
        "                               Range: 0-3 \n"
        "                               Default: 1"
        << std::endl;
    }

    return ApplicationArguments(
            parse_result,
            domain_id,
            add,
            item_name,
            quantity,
            delay,
            verbosity);
}

}  // namespace application

#endif  // APPLICATION_HPP
