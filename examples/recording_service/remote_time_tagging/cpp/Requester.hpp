/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <dds/domain/domainfwd.hpp>
#include <dds/core/External.hpp>
#include <rti/request/Requester.hpp>

#include "ServiceCommon.hpp"
#include "ServiceAdmin.hpp"
#include "RecordingServiceTypes.hpp"

/*
 * This class acts as an interpreter between the command-line arguments provided
 * to the executable call and the main application (class Application).
 */
class ArgumentsParser {
public:
    ArgumentsParser(int argc, char *argv[]);
    ~ArgumentsParser();

    RTI::Service::Admin::CommandActionKind command_kind() const;

    const std::string& resource_identifier() const;

    const std::string& command_params() const;

    uint32_t admin_domain_id() const;

    struct TimeTagParams {
        std::string name;
        std::string description;
    };

    const TimeTagParams& time_tag_params() const;

private:

    RTI::Service::Admin::CommandActionKind command_kind_;

    std::string resource_id_;

    std::string command_params_;

    uint32_t admin_domain_id_;

    TimeTagParams time_tag_params_;

    void print_usage(const std::string& program_name);

    static RTI::Service::Admin::CommandActionKind parse_command_kind(char *arg);

    static uint32_t parse_domain_id(char *arg);

};

/*
 * The main application class. This application can be understood as a one-shot
 * application. It will use the ArgumentsParser facade to access the parameters
 * necessary to generate a service administration command request, and then send
 * that request. It will wait to receive replies for the sent command and print
 * those out. Then it will exit.
 */
class Application {
public:

    /*
     * A requester type that can send command requests and receive replies to
     * those requests.
     */
    typedef rti::request::Requester<
            RTI::Service::Admin::CommandRequest,
            RTI::Service::Admin::CommandReply> ServiceAdminRequester;

    Application(ArgumentsParser& args_parser);
    ~Application();

private:

    ArgumentsParser& args_parser_;

    dds::domain::DomainParticipant participant_;

    rti::request::RequesterParams requester_params_;

    /*
     * This is the main Request/Reply type that will be used to ask the
     * service administration to execute commands.
     *
     */
    dds::core::external<ServiceAdminRequester>::shared_ptr command_requester_;

};
