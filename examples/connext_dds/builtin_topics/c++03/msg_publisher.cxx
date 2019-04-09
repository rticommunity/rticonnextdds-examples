/*******************************************************************************
 (c) 2005-2015 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "msg.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::pub;

// Authorization string
const std::string expected_password = "password";

// The builtin subscriber sets participant_qos.user_data, so we set up listeners
// for the builtin DataReaders to access these fields.
class BuiltinParticipantListener :
    public NoOpDataReaderListener<ParticipantBuiltinTopicData> {
public:
    // This gets called when a participant has been discovered
    void on_data_available(DataReader<ParticipantBuiltinTopicData>& reader)
    {
        // We only process newly seen participants
        LoanedSamples<ParticipantBuiltinTopicData> samples = reader.select()
            .state(dds::sub::status::DataState::new_instance())
            .take();

        for (LoanedSamples<ParticipantBuiltinTopicData>::iterator sampleIt = samples.begin();
                sampleIt != samples.end();
                ++sampleIt) {

            if (!sampleIt->info().valid()) {
                continue;
            }

            const ByteSeq& user_data = sampleIt->data().user_data().value();
            std::string user_auth (user_data.begin(), user_data.end());

            std::ios::fmtflags default_format(std::cout.flags());
            std::cout << std::hex << std::setw(8) << std::setfill('0');

            const BuiltinTopicKey& key = sampleIt->data().key();
            std::cout << "Built-in Reader: found participant" << std::endl
                      << "\tkey->'" << key.value()[0] << " "  << key.value()[1]
                      << " " << key.value()[2] << "'"         << std::endl
                      << "\tuser_data->'" << user_auth << "'" << std::endl
                      << "\tinstance_handle: "
                      << sampleIt->info().instance_handle()   << std::endl;

            std::cout.flags(default_format);

            // Check if the password match. Otherwise, ignore the participant.
            if (user_auth != expected_password) {
                std::cout << "Bad authorization, ignoring participant"
                          << std::endl;

                // Get the associated participant...
                DomainParticipant participant =
                    reader.subscriber().participant();

                // Ignore the remote participant
                dds::domain::ignore(
                    participant,
                    sampleIt->info().instance_handle());
            }
        }
    }
};

class BuiltinSubscriberListener :
    public NoOpDataReaderListener<SubscriptionBuiltinTopicData> {
public:
    // This gets called when a subscriber has been discovered
    void on_data_available(DataReader<SubscriptionBuiltinTopicData>& reader)
    {
         // We only process newly seen subscribers
        LoanedSamples<SubscriptionBuiltinTopicData> samples = reader.select()
            .state(dds::sub::status::DataState::new_instance())
            .take();

        for (LoanedSamples<SubscriptionBuiltinTopicData>::iterator sampleIt = samples.begin();
            sampleIt != samples.end();
            ++sampleIt) {

            if (!sampleIt->info().valid()) {
                continue;
            }

            std::ios::fmtflags default_format(std::cout.flags());
            std::cout << std::hex << std::setw(8) << std::setfill('0');

            const BuiltinTopicKey& partKey = sampleIt->data().participant_key();
            const BuiltinTopicKey& key = sampleIt->data().key();
            std::cout << "Built-in Reader: found subscriber"  << std::endl
                      << "\tparticipant_key->'"
                      << partKey.value()[0] << " " << partKey.value()[1] << " "
                      << partKey.value()[2] << "'" << std::endl
                      << "\tkey->'" << key.value()[0] << " "  << key.value()[1]
                      << " " << key.value()[2] << "'" << std::endl
                      << "\tinstance_handle: "
                      << sampleIt->info().instance_handle() << std::endl;

            std::cout.flags(default_format);
        }
    }
};

void publisher_main(int domain_id, int sample_count)
{
    // By default, the participant is enabled upon construction.
    // At that time our listeners for the builtin topics have not
    // been installed, so we disable the participant until we
    // set up the listeners. This is done by default in the
    // USER_QOS_PROFILES.xml file. If you want to do it programmatically,
    // just uncomment the following code.

    // DomainParticipantFactoryQos factoryQos =
    //     DomainParticipant::participant_factory_qos();
    // factoryQos << EntityFactory::ManuallyEnable();
    // DomainParticipant::participant_factory_qos(factoryQos);

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, you will need to comment out these lines and pass
    // the variable participant_qos as second argument in the constructor call.

    // DomainParticipantQos participant_qos = QosProvider::Default()
    //    .participant_qos();
    // DomainParticipantResourceLimits resource_limits_qos;
    // resource_limits_qos.participant_user_data_max_length(1024);
    // participant_qos << resource_limits_qos;

    // Create a DomainParticipant with default Qos.
    DomainParticipant participant(domain_id);

    // Installing listeners for the builtin topics requires several steps
    // First get the builtin subscriber.
    Subscriber builtin_subscriber = dds::sub::builtin_subscriber(participant);

    // Then get builtin subscriber's datareader for participants.
    std::vector< DataReader<ParticipantBuiltinTopicData> > participant_reader;
    find< DataReader<ParticipantBuiltinTopicData> >(
        builtin_subscriber,
        dds::topic::participant_topic_name(),
        std::back_inserter(participant_reader));

    // Install our listener using ListenerBinder, a RAII that will take care
    // of setting it to NULL and deleting it.
    rti::core::ListenerBinder< DataReader<ParticipantBuiltinTopicData> >
        participant_listener = rti::core::bind_and_manage_listener(
            participant_reader[0],
            new BuiltinParticipantListener,
            dds::core::status::StatusMask::data_available());

    // Get builtin subscriber's datareader for subscribers.
    std::vector< DataReader<SubscriptionBuiltinTopicData> > subscription_reader;
    find< DataReader<SubscriptionBuiltinTopicData> >(
        builtin_subscriber,
        dds::topic::subscription_topic_name(),
        std::back_inserter(subscription_reader));

    // Install our listener using ListenerBinder.
    rti::core::ListenerBinder< DataReader<SubscriptionBuiltinTopicData> >
        subscriber_listener = rti::core::bind_and_manage_listener(
            subscription_reader[0],
            new BuiltinSubscriberListener,
            dds::core::status::StatusMask::data_available());

    // Done! All the listeners are installed, so we can enable the
    // participant now.
    participant.enable();

    // Create a Topic -- and automatically register the type.
    Topic<msg> topic(participant, "Example msg");

    // Create a DataWriter
    DataWriter<msg> writer(Publisher(participant), topic);

    // Create data sample for writing
    msg instance;

    // For data type that has key, if the same instance is going to be
    // written multiple times, initialize the key here
    // and register the keyed instance prior to writing

    InstanceHandle instance_handle = InstanceHandle::nil();
    // instance_handle = writer.register_instance(instance);

    // Main loop
    for (short count=0; (sample_count == 0) || (count < sample_count); ++count){
        rti::util::sleep(Duration(1));
        std::cout << "Writing msg, count " << count << std::endl;

        // Modify the data to be sent here
        instance.x(count);

        writer.write(instance, instance_handle);
    }

    // writer.unregister_instance(instance);
}

int main(int argc, char* argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        publisher_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
