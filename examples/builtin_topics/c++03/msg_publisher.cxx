/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/
// http://en.cppreference.com/w/cpp/container/list

#include <string>
#include <list>
#include <algorithm>
#include "msg.hpp"
#include <dds/pub/ddspub.hpp>

using namespace dds::domain;
using namespace dds::topic;

// Authorization string
const std::string auth = "password";

// Set up a list of authorized participant keys. Datareaders associated
// with an authorized participant do not need to suplly their own password.
std::list<BuiltinTopicKey> Auth_list;

void add_auth_participant(const BuiltinTopicKey& participant_key)
{
    Auth_list.push_back(participant_key);
}

bool is_auth_participant(const BuiltinTopicKey& participant_key)
{
    return (std::find(Auth_list.begin(), Auth_list.end(), participant_key) !=
        Auth_list.end());
}

void publisher_main(int domainId, int sample_count)
{
    
}

void main(int argc, char* argv[])
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);
    
    return publisher_main(domainId, sample_count);
}