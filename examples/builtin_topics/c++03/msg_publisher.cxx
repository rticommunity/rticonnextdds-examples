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
#include <iostream>
#include <iomanip>

#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include "msg.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;

// Authorization string
const std::string Auth = "password";

// Set up a list of authorized participant keys. Datareaders associated
// with an authorized participant do not need to suplly their own password.
std::list<BuiltinTopicKey> AuthList;

bool isAuthParticipant(const BuiltinTopicKey& participantKey)
{
    auto item = std::find(AuthList.begin(), AuthList.end(), participantKey);
    return item != AuthList.end();
}

// The builtin subscriber sets participant_qos.user_data and
// reader_qos.user_data, so we set up listeners for the builtin
// DataReaders to access these fields.
class BuiltinParticipantLister : 
    NoOpDataReaderListener<ParticipantBuiltinTopicData> {
public:
    // This gets called when a participant has been discovered
    void on_data_available(DataReader<ParticipantBuiltinTopicData>& reader)
    {
        // We only process newly seen participants
        LoanedSamples<ParticipantBuiltinTopicData> samples = reader.select()
            .state(dds::sub::status::DataState::new_instance())
            .take();

        for (auto sampleIt = samples.begin();
            sampleIt != samples.end();
            ++sampleIt) {

            if (!sampleIt->info().valid()) {
                continue;
            }

            const ByteSeq& userData = sampleIt->data().user_data().value();
            std::string userAuth (userData.begin(), userData.end());
            const BuiltinTopicKey& key = sampleIt->data().key();
            bool isAuth = false;

            // Check if the password match.
            if (Auth.compare(userAuth)) {
                AuthList.push_back(key);
                isAuth = true;
            }

            std::ios::fmtflags defaultFormat(std::cout.flags());
            std::cout << std::hex << std::setw(8) << std::setfill('0');

            std::cout << "Built-in Reader: found participant" << std::endl
                      << "\tkey->'" << key.value()[0] << " "  << key.value()[1]
                      << " " << key.value()[2] << "'"         << std::endl
                      << "\tuser_data->'" << userAuth << "'"  << std::endl
                      << "instance_handle: " 
                      << sampleIt->info().instance_handle()   << std::endl;

            std::cout.flags(defaultFormat);

            if (!isAuth) {
                std::cout << "Bad authorization, ignoring participant"
                          << std::endl;
                const DomainParticipant& participant = 
                    reader.subscriber().participant();
                ignore(
                    const_cast<DomainParticipant&>(participant),
                    sampleIt->info().instance_handle());
            }
        }
    }
};

void publisherMain(int domainId, int sampleCount)
{
    
}

void main(int argc, char* argv[])
{
    int domainId = 0;
    int sampleCount = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sampleCount = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);
    
    publisherMain(domainId, sampleCount);
}