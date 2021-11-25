/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <sstream>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#include "ArgumentsParser.hpp"

using namespace RTI::Service::Admin;
using namespace rti::request;


CommandActionKind ArgumentsParser::parse_command_kind(char *arg)
{
    std::string arg_str(arg);
    CommandActionKind command_kind;

    if (arg_str.compare("CREATE") == 0) {
        command_kind = CommandActionKind::CREATE_ACTION;
    } else if (arg_str.compare("GET") == 0) {
        command_kind = CommandActionKind::GET_ACTION;
    } else if (arg_str.compare("UPDATE") == 0) {
        command_kind = CommandActionKind::UPDATE_ACTION;
    } else if (arg_str.compare("DELETE") == 0) {
        command_kind = CommandActionKind::DELETE_ACTION;
    } else {
        std::string ex_what("Invalid command action kind: ");
        ex_what += arg;
        throw std::runtime_error(ex_what);
    }
    return command_kind;
}

std::string ArgumentsParser::parse_file_to_str(char *arg)
{
    std::ifstream t(arg);
    std::string str(
            (std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());

    return str;
}

void ArgumentsParser::print_usage(const std::string &program_name)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    " << program_name
              << " <COMMAND_KIND> <RESOURCE_ID> <FILE_NAME> " << std::endl;
    std::cout << "    Where:" << std::endl;
    std::cout << "        COMMAND_KIND   = {CREATE|GET|UPDATE|DELETE}; required"
              << std::endl;
    std::cout
            << "        RESOURCE_ID    = the identity of the target resource "
               "in "
            << std::endl
            << "                         the service's resource tree that the "
            << std::endl
            << "                         command action should be applied to; "
            << std::endl
            << "                         required" << std::endl;
    std::cout
            << "        FILE_NAME = file name where the body (xml) is written "
            << std::endl
            << "                         command action, e.g. 'pause'; "
               "optional "
            << std::endl
            << "                         (some command kinds need no "
               "parameters)"
            << std::endl;
}

void ArgumentsParser::report_argument_error(
        const std::string &program_name,
        const std::string &tag,
        const std::string &error)
{
    print_usage(program_name);
    std::stringstream error_stream;
    error_stream << "Error: " << error << tag << " parameter";
    throw std::runtime_error(error_stream.str());
}

ArgumentsParser::ArgumentsParser(int argc, char *argv[])
{
    int current_arg = 0;
    // Parse mandatory arguments, we at least need 4 (includes program name)
    if (argc < 3) {
        print_usage(argv[0]);
        throw std::runtime_error("Error: invalid number of arguments");
    }
    // Command action kind
    command_kind_ = parse_command_kind(argv[1]);
    // Resource identifier
    resource_id_ = argv[2];
    // The following code requires more arguments than just the required ones
    if (argc > 3) {
        // Next argument
        body_str_ = argv[3];
    }
}

ArgumentsParser::~ArgumentsParser()
{
}

CommandActionKind ArgumentsParser::command_kind() const
{
    return command_kind_;
}

const std::string &ArgumentsParser::resource_identifier() const
{
    return resource_id_;
}

const std::string &ArgumentsParser::body_str() const
{
    return body_str_;
}
