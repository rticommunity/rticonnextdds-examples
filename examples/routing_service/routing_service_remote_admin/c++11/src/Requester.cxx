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

#include "ServiceAdmin.hpp"
#include "ServiceCommon.hpp"
#include "rti/request/RequesterParams.hpp"
#include "rti/request/Requester.hpp"

#include "ArgumentsParser.hpp"

// using namespace rti::request;
using namespace dds::core;
using namespace RTI::Service;
using namespace RTI::Service::Admin;

const unsigned int WAIT_TIMEOUT_SEC_MAX = 10;
const unsigned int ADMIN_DOMAIN_ID = 55;

void send(
        CommandRequest &request,
        rti::request::Requester<
                RTI::Service::Admin::CommandRequest,
                RTI::Service::Admin::CommandReply> &requester)
{
    dds::topic::topic_type_support<EntityState>::to_cdr_buffer(
            reinterpret_cast<std::vector<char> &>(request.octet_body()),
            EntityState(EntityStateKind::DISABLED));
    /*
     * Send disable
     */
    requester.send_request(request);
    CommandReply reply =
            requester.receive_replies(Duration(WAIT_TIMEOUT_SEC_MAX))[0];
    if (reply.retcode() == CommandReplyRetcode::OK_RETCODE) {
        std::cout << "Command returned: " << reply.string_body() << std::endl;
    } else {
        std::cout << "Unsuccessful command returned value " << reply.retcode()
                  << "." << std::endl;
        throw dds::core::Error("Error in replier");
    }
}

int main(int argc, char *argv[])
{
    try {
        ArgumentsParser args_parser(argc, argv);


        dds::domain::DomainParticipant participant(ADMIN_DOMAIN_ID);

        // create requester params
        rti::request::RequesterParams requester_params(participant);
        requester_params.request_topic_name(COMMAND_REQUEST_TOPIC_NAME);
        requester_params.reply_topic_name(COMMAND_REPLY_TOPIC_NAME);

        rti::request::Requester<
                RTI::Service::Admin::CommandRequest,
                RTI::Service::Admin::CommandReply>
                requester(requester_params);
        // Wait for Routing Service Discovery
        dds::core::status::PublicationMatchedStatus matched_status;
        unsigned int wait_count = 0;

        std::cout << "Waiting for a matching replier..." << std::endl;
        while (matched_status.current_count() < 1
               && wait_count < WAIT_TIMEOUT_SEC_MAX) {
            matched_status =
                    requester.request_datawriter().publication_matched_status();
            wait_count++;
            rti::util::sleep(Duration(1));
        }

        if (matched_status.current_count() < 1) {
            throw dds::core::Error("No matching replier found.");
        }

        /*
         * Setup command
         */
        CommandRequest request;
        request.action(args_parser.command_kind());
        request.resource_identifier(args_parser.resource_identifier());
        if (args_parser.body_str() != "")
            request.string_body(args_parser.body_str());
        send(request, requester);

    } catch (const std::exception &ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}