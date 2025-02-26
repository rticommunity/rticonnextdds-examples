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

#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>

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

uint64_t ArgumentsParser::parse_number(char *arg)
{
    std::stringstream stream(arg);
    uint64_t value;
    if (!(stream >> value)) {
        std::stringstream error_stream;
        error_stream << "Error: could not parse uint64 value provided, '" << arg
                     << "'";
        throw std::runtime_error(error_stream.str());
    }
    return value;
}

bool ArgumentsParser::is_number(char *arg)
{
    bool value = true;
    std::stringstream stream(arg);
    uint64_t value_number;
    if (!(stream >> value_number)) {
        value = false;
    }
    return value;
}

void ArgumentsParser::print_usage(const std::string &program_name)
{
    std::cout << "Usage:" << std::endl;
    std::cout
            << "    " << program_name
            << " <COMMAND_KIND> <RESOURCE_ID> <COMMAND_PARAMS> [optional args]"
            << std::endl;
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
    std::cout << "        COMMAND_PARAMS = additional parameters needed by the "
              << std::endl
              << "                         command action, e.g. 'pause'; "
                 "optional "
              << std::endl
              << "                         (some command kinds need no "
                 "parameters)"
              << std::endl;
    std::cout << "    The following is a list of additional arguments that can "
              << std::endl
              << "    be provided to the application:" << std::endl;
    std::cout << "        --domain-id:" << std::endl;
    std::cout << "            Format: --domain-id <uint32>" << std::endl;
    std::cout
            << "            Description: the DDS domain ID to use with the "
            << std::endl
            << "                requester (has to be the same as the service's "
            << std::endl
            << "                administration domain ID). Default: 0."
            << std::endl;
    std::cout << "        --time-tag:" << std::endl;
    std::cout << "            Format: --time-tag <name> [<description>]"
              << std::endl;
    std::cout
            << "            Description: have Recorder create a symbolic "
            << std::endl
            << "                time-stamp with the given name and description "
            << std::endl
            << "                and use the current time at the moment of "
            << std::endl
            << "                sending the command. Description is optional."
            << std::endl;
    std::cout << "        --add-breakpoint:" << std::endl;
    std::cout << "            Format: --add-breakpoint <timestamp> [<label>]"
              << std::endl;
    std::cout << "            Description: the breakpoint to be added in the "
                 "replay."
              << std::endl
              << "                The timestamp have to be provided in "
                 "nanosecond "
              << std::endl
              << "                format. Label is optional." << std::endl;
    std::cout << "        --remove-breakpoint:" << std::endl;
    std::cout << "            Format: --remove-breakpoint <timestamp>|<label>"
              << std::endl;
    std::cout
            << "            Description: the breakpoint to be removed of the "
               "replay."
            << std::endl
            << "                You can specify the breakpoint to be removed by"
            << std::endl
            << "                timestamp or by label." << std::endl;
    std::cout << "        --goto-breakpoint:" << std::endl;
    std::cout << "            Format: --goto-breakpoint <timestamp>|<label>"
              << std::endl;
    std::cout << "            Description: Jump to an existed breakpoint"
              << std::endl
              << "                You can jump by the breakpoint timestamp"
              << std::endl
              << "                or by the breakpoint label." << std::endl;
    std::cout << "        --continue-seconds:" << std::endl;
    std::cout << "            Format: --continue-seconds <seconds>"
              << std::endl;
    std::cout << "            Description: To resume the replay after hit a "
              << std::endl
              << "                breakpoint. You can resume the replay for "
              << std::endl
              << "                a number of seconds. " << std::endl;
    std::cout << "        --continue-slices:" << std::endl;
    std::cout << "            Format: --continue-slices <slices>" << std::endl;
    std::cout << "            Description: To resume the replay after hit a "
              << std::endl
              << "                breakpoint. You can resume the replay for "
              << std::endl
              << "                a number of slices. " << std::endl;
    std::cout << "        --current-timestamp:" << std::endl;
    std::cout << "            Format: --current-timestamp <timestamp_nanos>"
              << std::endl;
    std::cout << "            Description: To jump in time during the replay. "
              << std::endl
              << "                The timestamp have to be provided in "
                 "nanosecond "
              << std::endl
              << "                format. " << std::endl;
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
        : admin_domain_id_(0), octet_kind_(OctetKind::NONE)
{
    const std::string DOMAIN_ID_ARG_NAME("--domain-id");
    const std::string TIME_TAG_ARG_NAME("--time-tag");
    const std::string ADD_BR_ARG_NAME("--add-breakpoint");
    const std::string RM_BR_ARG_NAME("--remove-breakpoint");
    const std::string GOTO_BR_ARG_NAME("--goto-breakpoint");
    const std::string CONTINUE_SEC_ARG_NAME("--continue-seconds");
    const std::string CONTINUE_SLICE_ARG_NAME("--continue-slices");
    const std::string JUMP_TIME_ARG_NAME("--current-timestamp");

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
            && TIME_TAG_ARG_NAME.compare(argv[3]) != 0
            && ADD_BR_ARG_NAME.compare(argv[3]) != 0
            && RM_BR_ARG_NAME.compare(argv[3]) != 0
            && GOTO_BR_ARG_NAME.compare(argv[3]) != 0
            && CONTINUE_SEC_ARG_NAME.compare(argv[3]) != 0
            && CONTINUE_SLICE_ARG_NAME.compare(argv[3]) != 0
            && JUMP_TIME_ARG_NAME.compare(argv[3]) != 0) {
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
                admin_domain_id_ = parse_number(argv[current_arg + 1]);
                current_arg += 2;
            } else if (TIME_TAG_ARG_NAME.compare(argv[current_arg]) == 0) {
                // This parameter may use one or two arguments
                if (current_arg + 1 < argc) {
                    octet_kind_ = OctetKind::DATATAG;
                    data_tag_params_.tag_name(argv[current_arg + 1]);
                    data_tag_params_.timestamp_offset(0);
                    // Check if a description has been provided
                    if (current_arg + 2 < argc) {
                        data_tag_params_.tag_description(argv[current_arg + 2]);
                        current_arg += 3;
                    } else {
                        current_arg += 2;
                    }
                } else {
                    // No name provided for the time tag
                    report_argument_error(
                            argv[0],
                            TIME_TAG_ARG_NAME,
                            "no name provided for ");
                }
            } else if (ADD_BR_ARG_NAME.compare(argv[current_arg]) == 0) {
                if (current_arg + 1 < argc) {
                    octet_kind_ = OctetKind::BREAKPOINT;
                    // This parameter may use one or two arguments
                    br_params_.value().timestamp_nanos(
                            parse_number(argv[current_arg + 1]));
                    // Check if a label has been provided
                    if (current_arg + 2 < argc) {
                        br_params_.label(std::string(argv[current_arg + 2]));
                        current_arg += 3;
                    } else {
                        current_arg += 2;
                    }
                } else {
                    // No timestamp provided for the breakpoint
                    report_argument_error(
                            argv[0],
                            ADD_BR_ARG_NAME,
                            "no timestamp provided for ");
                }
            } else if (RM_BR_ARG_NAME.compare(argv[current_arg]) == 0) {
                octet_kind_ = OctetKind::BREAKPOINT;
                if (current_arg + 2 == argc) {
                    // This parameter may use one or two arguments
                    if (is_number(argv[current_arg + 1])) {
                        br_params_.value().timestamp_nanos(
                                parse_number(argv[current_arg + 1]));
                    } else {
                        br_params_.label(std::string(argv[current_arg + 1]));
                    }
                    current_arg += 2;
                } else if (current_arg + 2 < argc) {
                    br_params_.value().timestamp_nanos(
                            parse_number(argv[current_arg + 1]));
                    br_params_.label(std::string(argv[current_arg + 2]));
                    current_arg += 3;
                } else {
                    // No information for the breakpoint
                    report_argument_error(
                            argv[0],
                            RM_BR_ARG_NAME,
                            "no information provided for ");
                }
            } else if (GOTO_BR_ARG_NAME.compare(argv[current_arg]) == 0) {
                octet_kind_ = OctetKind::BREAKPOINT;
                if (current_arg + 2 == argc) {
                    // This parameter may use one or two arguments
                    if (is_number(argv[current_arg + 1])) {
                        br_params_.value().timestamp_nanos(
                                parse_number(argv[current_arg + 1]));
                    } else {
                        br_params_.label(std::string(argv[current_arg + 1]));
                    }
                    current_arg += 2;
                } else if (current_arg + 2 < argc) {
                    br_params_.value().timestamp_nanos(
                            parse_number(argv[current_arg + 1]));
                    br_params_.label(std::string(argv[current_arg + 2]));
                    current_arg += 3;
                } else {
                    // No information for the breakpoint
                    report_argument_error(
                            argv[0],
                            GOTO_BR_ARG_NAME,
                            "no information provided for ");
                }

            } else if (CONTINUE_SEC_ARG_NAME.compare(argv[current_arg]) == 0) {
                if (current_arg + 1 < argc) {
                    octet_kind_ = OctetKind::CONTINUE;
                    continue_params_.value().offset(
                            parse_number(argv[current_arg + 1]));
                    current_arg += 2;
                } else {
                    // No number of seconds for the continue param
                    report_argument_error(
                            argv[0],
                            CONTINUE_SEC_ARG_NAME,
                            "no number of seconds provided for ");
                }
            } else if (
                    CONTINUE_SLICE_ARG_NAME.compare(argv[current_arg]) == 0) {
                if (current_arg + 1 < argc) {
                    octet_kind_ = OctetKind::CONTINUE;
                    continue_params_.value().slices(
                            parse_number(argv[current_arg + 1]));
                    current_arg += 2;
                } else {
                    // No number of slices for the continue param
                    report_argument_error(
                            argv[0],
                            CONTINUE_SLICE_ARG_NAME,
                            "no number of slices provided for ");
                }
            } else if (JUMP_TIME_ARG_NAME.compare(argv[current_arg]) == 0) {
                if (current_arg + 1 < argc) {
                    octet_kind_ = OctetKind::TIMESTAMPHOLDER;
                    timestamp_holder_.timestamp_nanos(
                            parse_number(argv[current_arg + 1]));
                    current_arg += 2;
                } else {
                    // No timestamp for the jump in time
                    report_argument_error(
                            argv[0],
                            JUMP_TIME_ARG_NAME,
                            "no timestamp provided for ");
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

const std::string &ArgumentsParser::resource_identifier() const
{
    return resource_id_;
}

const std::string &ArgumentsParser::command_params() const
{
    return command_params_;
}

uint32_t ArgumentsParser::admin_domain_id() const
{
    return admin_domain_id_;
}

const DataTagParams &ArgumentsParser::data_tag_params() const
{
    return data_tag_params_;
}

const OctetKind ArgumentsParser::octet_kind() const
{
    return octet_kind_;
}

const BreakpointParams &ArgumentsParser::br_params() const
{
    return br_params_;
}

const ContinueParams &ArgumentsParser::continue_params() const
{
    return continue_params_;
}

const TimestampHolder &ArgumentsParser::timestamp_holder() const
{
    return timestamp_holder_;
}

Application::Application(ArgumentsParser &args_parser)
        : args_parser_(args_parser),
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
                    "ServiceAdministrationProfiles::"
                    "ServiceAdminRequesterProfile"));
    requester_params_.datawriter_qos(
            dds::core::QosProvider::Default().datawriter_qos(
                    "ServiceAdministrationProfiles::"
                    "ServiceAdminRequesterProfile"));

    // configure the requester for compatibility with the RTI routing service
    // request reply.
    requester_params_.require_matching_service_on_send_request(false);

    /*
     * Now that we have set up all the parameters for the requester instance, we
     * can create it.
     */
    command_requester_ = dds::core::external<ServiceAdminRequester>::shared_ptr(
            new ServiceAdminRequester(requester_params_));

    CommandRequest command_request;

    // Check if we have time tag parameters
    OctetKind kind = args_parser.octet_kind();
    switch (kind) {
    case OctetKind::DATATAG:
        dds::topic::topic_type_support<DataTagParams>::to_cdr_buffer(
                reinterpret_cast<std::vector<char> &>(
                        command_request.octet_body()),
                args_parser_.data_tag_params());
        break;
    case OctetKind::BREAKPOINT:
        dds::topic::topic_type_support<BreakpointParams>::to_cdr_buffer(
                reinterpret_cast<std::vector<char> &>(
                        command_request.octet_body()),
                args_parser_.br_params());
        break;
    case OctetKind::CONTINUE:
        dds::topic::topic_type_support<ContinueParams>::to_cdr_buffer(
                reinterpret_cast<std::vector<char> &>(
                        command_request.octet_body()),
                args_parser_.continue_params());
        break;
    case OctetKind::TIMESTAMPHOLDER:
        dds::topic::topic_type_support<TimestampHolder>::to_cdr_buffer(
                reinterpret_cast<std::vector<char> &>(
                        command_request.octet_body()),
                args_parser_.timestamp_holder());
        break;

    case OctetKind::NONE:
    default:
        break;
    }

    command_request.action(args_parser_.command_kind());
    command_request.resource_identifier(args_parser_.resource_identifier());
    command_request.string_body(args_parser_.command_params());

    /*
     * Wait until the command requester's request data writer has matched at
     * least one publication.
     */
    int32_t current_pub_matches = command_requester_->request_datawriter()
                                          .publication_matched_status()
                                          .current_count();
    int32_t wait_cycles = 1200;
    while (current_pub_matches < 1 && wait_cycles > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        current_pub_matches = command_requester_->request_datawriter()
                                      .publication_matched_status()
                                      .current_count();
        wait_cycles--;
    }
    if (wait_cycles == 0) {
        throw std::runtime_error(
                "Error: command requester's request data "
                "writer matched no subscriptions");
    }
    /*
     * Wait until the command requester's reply data reader has matched at least
     * one subscription.
     */
    int32_t current_sub_matches = command_requester_->reply_datareader()
                                          .subscription_matched_status()
                                          .current_count();
    wait_cycles = 1200;
    while (current_sub_matches < 1 && wait_cycles > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        current_sub_matches = command_requester_->reply_datareader()
                                      .subscription_matched_status()
                                      .current_count();
        wait_cycles--;
    }
    if (wait_cycles == 0) {
        throw std::runtime_error(
                "Error: command requester's reply data "
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
        throw std::runtime_error(
                "Error: no reply received for request "
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
    } catch (std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
