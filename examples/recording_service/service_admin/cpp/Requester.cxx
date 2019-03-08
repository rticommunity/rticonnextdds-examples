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

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>

#include "Requester.hpp"

using namespace RTI::Service::Admin;
using namespace RTI::RecordingService;


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

uint32_t ArgumentsParser::parse_domain_id(char *arg)
{
    std::stringstream stream(arg);
    uint32_t domain_id;
    stream >> domain_id;
    if (stream.bad()) {
        std::stringstream error_stream;
        error_stream << "Error: could not parse uint32 value provided, '"
                << arg << "'";
        throw std::runtime_error(error_stream.str());
    }
    return domain_id;
}

void ArgumentsParser::print_usage(const std::string& program_name)
{
    std::cout <<
            "Usage:" << std::endl;
    std::cout <<
            "    " << program_name
            << " <COMMAND_KIND> <RESOURCE_ID> <COMMAND_PARAMS> [optional args]"
            << std::endl;
    std::cout <<
            "    Where:" << std::endl;
    std::cout <<
            "        COMMAND_KIND   = {CREATE|GET|UPDATE|DELETE}; required"
            << std::endl;
    std::cout <<
            "        RESOURCE_ID    = the identity of the target resource in " << std::endl <<
            "                         the service's resource tree that the " << std::endl <<
            "                         command action should be applied to; " << std::endl <<
            "                         required" << std::endl;
    std::cout <<
            "        COMMAND_PARAMS = additional parameters needed by the " << std::endl <<
            "                         command action, e.g. 'pause'; optional " << std::endl <<
            "                         (some command kinds need no parameters)" << std::endl;
    std::cout <<
            "    The following is a list of additional arguments that can "  << std::endl <<
            "    be provided to the application:" << std::endl;
    std::cout <<
            "        --domain-id:" << std::endl;
    std::cout <<
            "            Format: --domain-id <uint32>" << std::endl;
    std::cout <<
            "            Description: the DDS domain ID to use with the " << std::endl <<
            "                requester (has to be the same as the service's " << std::endl <<
            "                administration domain ID). Default: 0." << std::endl;
    std::cout <<
            "        --time-tag:" << std::endl;
    std::cout <<
            "            Format: --time-tag <name> [<description>]" << std::endl;
    std::cout <<
            "            Description: have Recorder create a symbolic " << std::endl <<
            "                time-stamp with the given name and description " << std::endl <<
            "                and use the current time at the moment of " << std::endl <<
            "                sending the command. Description is optional." << std::endl;
}

ArgumentsParser::ArgumentsParser(int argc, char *argv[]) :
    admin_domain_id_(0)
{
    const std::string DOMAIN_ID_ARG_NAME("--domain-id");
    const std::string TIME_TAG_ARG_NAME("--time-tag");

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
        // Command parameters; it's optional
        if (DOMAIN_ID_ARG_NAME.compare(argv[3]) != 0
                && TIME_TAG_ARG_NAME.compare(argv[3]) != 0) {
            /*
             * If this parameter is not one of the extra arguments, it has to be
             * the optional command parameters string
             */
            command_params_ = argv[3];
            current_arg = 4;
        } else {
            current_arg = 3;
        }
        // Extra arguments
        while (current_arg < argc) {
            if (DOMAIN_ID_ARG_NAME.compare(argv[current_arg]) == 0) {
                // This parameter needs one argument
                if (current_arg + 1 >= argc) {
                    print_usage(argv[0]);
                    std::stringstream error_stream;
                    error_stream << "Error: no uint32 value provided for "
                            << DOMAIN_ID_ARG_NAME << " parameter";
                    throw std::runtime_error(error_stream.str());
                }
                admin_domain_id_ = parse_domain_id(argv[current_arg + 1]);
                current_arg += 2;
            } else if (TIME_TAG_ARG_NAME.compare(argv[current_arg]) == 0) {
                // This parameter may use one or two arguments
                if (current_arg + 1 < argc) {
                    time_tag_params_.name = argv[current_arg + 1];
                    // Check if a description has been provided
                    if (current_arg + 2 < argc) {
                        time_tag_params_.description = argv[current_arg + 2];
                        current_arg += 3;
                    } else {
                        current_arg += 2;
                    }
                } else {
                    // No name provided for the time tag
                    print_usage(argv[0]);
                    std::stringstream error_stream;
                    error_stream << "Error: no name provided for "
                            << TIME_TAG_ARG_NAME << " parameter";
                    throw std::runtime_error(error_stream.str());
                }
            } else {
                // Unrecognised parameter
                print_usage(argv[0]);
                std::stringstream error_stream;
                error_stream << "Error: unrecognized parameter, '"
                        << argv[current_arg] << "'";
                throw std::runtime_error(error_stream.str());
            }
        }
    }
}

ArgumentsParser::~ArgumentsParser()
{

}

CommandActionKind ArgumentsParser::command_kind() const
{
    return command_kind_;
}

const std::string& ArgumentsParser::resource_identifier() const
{
    return resource_id_;
}

const std::string& ArgumentsParser::command_params() const
{
    return command_params_;
}

uint32_t ArgumentsParser::admin_domain_id() const
{
    return admin_domain_id_;
}

const ArgumentsParser::TimeTagParams& ArgumentsParser::time_tag_params() const
{
    return time_tag_params_;
}

Application::Application(ArgumentsParser& args_parser) :
    args_parser_(args_parser),
    participant_(args_parser.admin_domain_id()),
    requester_params_(participant_)
{
    /*
     * Prepare the requester params: we need topic names for the request and
     * reply types. These are defined in the ServiceAdmin.idl file and generated
     * when RTI DDS Code Generator is run.
     */
    requester_params_.request_topic_name(COMMAND_REQUEST_TOPIC_NAME);
    requester_params_.reply_topic_name(COMMAND_REPLY_TOPIC_NAME);
    /*
     * Note: the QoS library and profile we are using comes from the
     * request/reply example provided in the Connext installation.
     * Recall that the profile must be augmented with the properties needed
     * for unbounded support in the Data Writer QoS.
     */
    requester_params_.datareader_qos(
            dds::core::QosProvider::Default().datareader_qos(
                    "ServiceAdministrationProfiles::ServiceAdminRequesterProfile"));
    requester_params_.datawriter_qos(
            dds::core::QosProvider::Default().datawriter_qos(
                    "ServiceAdministrationProfiles::ServiceAdminRequesterProfile"));
    /*
     * Now that we have set up all the parameters for the requester instance, we
     * can create it.
     */
    command_requester_ = dds::core::external<ServiceAdminRequester>::shared_ptr(
            new ServiceAdminRequester(requester_params_));

    CommandRequest command_request;

    // Check if we have time tag parameters
    const ArgumentsParser::TimeTagParams& time_tag_params =
            args_parser_.time_tag_params();
    if (!time_tag_params.name.empty()) {
        DataTagParams data_tag_params;
        data_tag_params.tag_name(time_tag_params.name);
        data_tag_params.tag_description(time_tag_params.description);
        data_tag_params.timestamp_offset(0);
        dds::topic::topic_type_support<DataTagParams>::to_cdr_buffer(
                reinterpret_cast<std::vector<char>&>(
                        command_request.octet_body()),
                data_tag_params);
    }

    command_request.action(args_parser_.command_kind());
    command_request.resource_identifier(args_parser_.resource_identifier());
    command_request.string_body(args_parser_.command_params());

    /*
     * Wait until the command requester's request data writer has matched at
     * least one publication.
     */
    int32_t current_pub_matches = command_requester_->request_datawriter()
            .publication_matched_status().current_count();
    int32_t wait_cycles = 1200;
    while (current_pub_matches < 1 && wait_cycles > 0) {
        rti::util::sleep(dds::core::Duration::from_millisecs(50));
        current_pub_matches = command_requester_->request_datawriter()
                .publication_matched_status().current_count();
        wait_cycles--;
    }
    if (wait_cycles == 0) {
        throw std::runtime_error("Error: command requester's request data "
                "writer matched no subscriptions");
    }
    /*
     * Wait until the command requester's reply data reader has matched at least
     * one subscription.
     */
    int32_t current_sub_matches = command_requester_->reply_datareader()
            .subscription_matched_status().current_count();
    wait_cycles = 1200;
    while (current_sub_matches < 1 && wait_cycles > 0) {
        rti::util::sleep(dds::core::Duration::from_millisecs(50));
        current_sub_matches = command_requester_->reply_datareader()
                .subscription_matched_status().current_count();
        wait_cycles--;
    }
    if (wait_cycles == 0) {
        throw std::runtime_error("Error: command requester's reply data "
                "reader matched no publications");
    }

    std::cout << "Command request about to be sent:" << std::endl
            << command_request << std::endl;

    /*
     * Send the request, obtaining this request's ID. We will use it later to
     * correlate the replies with the request.
     */
    rti::core::SampleIdentity request_id =
            command_requester_->send_request(command_request);
    /*
     * Wait for replies. This version of the method waits for the first reply
     * directed to the request we produced, a maximum of 60 seconds.
     */
    if (!command_requester_->wait_for_replies(
            1,
            dds::core::Duration::from_secs(60),
            request_id)) {
        throw std::runtime_error("Error: no reply received for request "
                "(60 seconds timeout)");
    }
    // Getting to this point means we have received reply(ies) for our request
    dds::sub::LoanedSamples<CommandReply> replies =
            command_requester_->take_replies(request_id);
    std::cout << "Received " << replies.length() << " replies from service"
            << std::endl;
    for (unsigned int i = 0; i < replies.length(); ++i) {
        std::cout << "    Reply " << i + 1 << ":" << std::endl;
        if (replies[i].info().valid()) {
            std::cout << "        Retcode        = "
                    << replies[i].data().retcode() << std::endl;
            std::cout << "        Native retcode = "
                    << replies[i].data().native_retcode() << std::endl;
            std::cout << "        String body    = "
                    << replies[i].data().string_body() << std::endl;
        } else {
            std::cout << "        Sample Invalid" << std::endl;
        }
    }
}

Application::~Application()
{

}

int main(int argc, char *argv[])
{
    try {
        /*
         * The main method is quite simple. We have a class that is a
         * command-line interpreter (ArgumentsParser) for the main application
         * class (Application). This class will produce a command request with
         * the given command-line parameters and wait for any replies. It will
         * then exit. This can be understand as a one-shot application,
         * producing just one service administration request.
         */
        ArgumentsParser args_parser(argc, argv);
        Application requester_app(args_parser);
    } catch (std::runtime_error& ex) {
        std::cerr << ex.what() << std:: endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
