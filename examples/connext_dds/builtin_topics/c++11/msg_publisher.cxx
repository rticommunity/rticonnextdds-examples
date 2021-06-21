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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include <dds/domain/discovery.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "msg.hpp"
#include "application.hpp"

// Authorization string
const std::string expected_password = "password";

// The builtin subscriber sets participant_qos.user_data, so we set up listeners
// for the builtin DataReaders to access these fields.
class BuiltinParticipantListener
        : public dds::sub::NoOpDataReaderListener<
                  dds::topic::ParticipantBuiltinTopicData> {
public:
    // This gets called when a participant has been discovered
    void on_data_available(
            dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>
                    &reader)
    {
        // We only process newly seen participants
        dds::sub::LoanedSamples<dds::topic::ParticipantBuiltinTopicData>
                samples = reader.select()
                                  .state(dds::sub::status::DataState::
                                                 new_instance())
                                  .take();

        for (const auto &sample : samples) {
            if (!sample.info().valid()) {
                continue;
            }

            const dds::core::ByteSeq &user_data =
                    sample.data().user_data().value();
            std::string user_auth(user_data.begin(), user_data.end());

            std::ios::fmtflags default_format(std::cout.flags());
            std::cout << std::hex << std::setw(8) << std::setfill('0');

            const dds::topic::BuiltinTopicKey &key = sample.data().key();
            std::cout << "Built-in Reader: found participant" << std::endl
                      << "\tkey->'" << key.value()[0] << " " << key.value()[1]
                      << " " << key.value()[2] << "'" << std::endl
                      << "\tuser_data->'" << user_auth << "'" << std::endl
                      << "\tinstance_handle: "
                      << sample.info().instance_handle() << std::endl;

            std::cout.flags(default_format);

            // Check if the password match. Otherwise, ignore the participant.
            if (user_auth != expected_password) {
                std::cout << "Bad authorization, ignoring participant"
                          << std::endl;

                // Get the associated participant...
                dds::domain::DomainParticipant participant =
                        reader.subscriber().participant();

                // Ignore the remote participant
                dds::domain::ignore(
                        participant,
                        sample->info().instance_handle());
            }
        }
    }
};

class BuiltinSubscriberListener
        : public dds::sub::NoOpDataReaderListener<
                  dds::topic::SubscriptionBuiltinTopicData> {
public:
    // This gets called when a subscriber has been discovered
    void on_data_available(
            dds::sub::DataReader<dds::topic::SubscriptionBuiltinTopicData>
                    &reader)
    {
        // We only process newly seen subscribers
        dds::sub::LoanedSamples<dds::topic::SubscriptionBuiltinTopicData>
                samples = reader.select()
                                  .state(dds::sub::status::DataState::
                                                 new_instance())
                                  .take();

        for (const auto &sample : samples) {
            if (!sample.info().valid()) {
                continue;
            }

            std::ios::fmtflags default_format(std::cout.flags());
            std::cout << std::hex << std::setw(8) << std::setfill('0');

            const dds::topic::BuiltinTopicKey &partKey =
                    sample.data().participant_key();
            const dds::topic::BuiltinTopicKey &key = sample.data().key();
            std::cout << "Built-in Reader: found subscriber" << std::endl
                      << "\tparticipant_key->'" << partKey.value()[0] << " "
                      << partKey.value()[1] << " " << partKey.value()[2] << "'"
                      << std::endl
                      << "\tkey->'" << key.value()[0] << " " << key.value()[1]
                      << " " << key.value()[2] << "'" << std::endl
                      << "\tinstance_handle: "
                      << sample.info().instance_handle() << std::endl;

            std::cout.flags(default_format);
        }
    }
};

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
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
    dds::domain::DomainParticipant participant(domain_id);

    // Installing listeners for the builtin topics requires several steps
    // First get the builtin subscriber.
    dds::sub::Subscriber builtin_subscriber =
            dds::sub::builtin_subscriber(participant);

    // Create shared pointer to BuiltinParticipantListener class
    auto participant_listener = std::make_shared<BuiltinParticipantListener>();

    // Then get builtin subscriber's datareader for participants.
    std::vector<dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>>
            participant_reader;
    dds::sub::find<
            dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>>(
            builtin_subscriber,
            dds::topic::participant_topic_name(),
            std::back_inserter(participant_reader));

    participant_reader[0].set_listener(participant_listener);

    auto subscriber_listener = std::make_shared<BuiltinSubscriberListener>();

    // Get builtin subscriber's datareader for subscribers.
    std::vector<dds::sub::DataReader<dds::topic::SubscriptionBuiltinTopicData>>
            subscription_reader;
    dds::sub::find<
            dds::sub::DataReader<dds::topic::SubscriptionBuiltinTopicData>>(
            builtin_subscriber,
            dds::topic::subscription_topic_name(),
            std::back_inserter(subscription_reader));

    // Install our listener using the shared pointer.
    subscription_reader[0].set_listener(subscriber_listener);

    // Done! All the listeners are installed, so we can enable the
    // participant now.
    participant.enable();

    // Create a Topic -- and automatically register the type.
    dds::topic::Topic<msg> topic(participant, "Example msg");

    // Create a publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter
    dds::pub::DataWriter<msg> writer(publisher, topic);

    // Create data sample for writing
    msg instance;

    // For data type that has key, if the same instance is going to be
    // written multiple times, initialize the key here
    // and register the keyed instance prior to writing

    dds::core::InstanceHandle instance_handle =
            dds::core::InstanceHandle::nil();
    // instance_handle = writer.register_instance(instance);

    // Main loop
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        std::cout << "Writing msg, count " << samples_written << std::endl;

        // Send count as data
        instance.x(samples_written);

        // Send it, if using instance_handle:
        // writer.write(instance, instance_handle);
        writer.write(instance);

        // Send once every second
        rti::util::sleep(dds::core::Duration(1));
    }

    // writer.unregister_instance(instance);
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, ApplicationKind::publisher);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_publisher_application(arguments.domain_id, arguments.sample_count);
    } catch (std::exception ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
