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

#include <dds/core/types.hpp>
#include <dds/core/External.hpp>
#include <dds/domain/domainfwd.hpp>
#include <rti/request/Requester.hpp>

#include "ServiceAdmin.hpp"
#include "ServiceCommon.hpp"

/*
 * This class acts as an interpreter between the command-line arguments provided
 * to the executable call and the main application (class Application).
 */
class ArgumentsParser {
public:
    ArgumentsParser(int argc, char *argv[]);
    ~ArgumentsParser();

    RTI::Service::Admin::CommandActionKind command_kind() const;

    const std::string &resource_identifier() const;

    const std::string &body_str() const;

private:
    RTI::Service::Admin::CommandActionKind command_kind_;
    std::string resource_id_;
    std::string body_str_;

    void print_usage(const std::string &program_name);

    void report_argument_error(
            const std::string &program_name,
            const std::string &tag,
            const std::string &error);

    static RTI::Service::Admin::CommandActionKind parse_command_kind(char *arg);
    static std::string parse_file_to_str(char *arg);
};
