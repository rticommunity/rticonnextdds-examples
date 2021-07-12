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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "profiles.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

class ProfilesListener : public dds::sub::NoOpDataReaderListener<profiles> {
public:
    ProfilesListener(std::string name) : listener_name_(name)
    {
    }

    void on_data_available(dds::sub::DataReader<profiles> &reader)
    {
        // Take all samples
        dds::sub::LoanedSamples<profiles> samples = reader.take();

        std::cout << "============================================="
                  << std::endl
                  << listener_name() << " listener received" << std::endl;

        for (const auto &sample : samples) {
            if (sample.info().valid()) {
                std::cout << sample.data() << std::endl;
            }
        }

        std::cout << "============================================="
                  << std::endl
                  << std::endl;
    }

    std::string listener_name() const
    {
        return listener_name_;
    }

private:
    const std::string listener_name_;
};

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Retrieve QoS from custom profile XML and USER_QOS_PROFILES.xml
    dds::core::QosProvider qos_provider("my_custom_qos_profiles.xml");

    // Create a DomainParticipant with the default QoS of the provider.
    dds::domain::DomainParticipant participant(
            domain_id,
            qos_provider.participant_qos());

    // Create a Subscriber with default QoS.
    dds::sub::Subscriber subscriber(participant, qos_provider.subscriber_qos());

    // Create a Topic with default QoS.
    dds::topic::Topic<profiles> topic(
            participant,
            "Example profiles",
            qos_provider.topic_qos());

    // Create a shared pointer for the ProfilesListener Class
    auto transient_listener =
            std::make_shared<ProfilesListener>("transient_local_profile");

    // Create a DataWriter with the QoS profile "transient_local_profile" that
    // it is inside the QoS library "profiles_Library".
    dds::sub::DataReader<profiles> reader_transient_local(
            subscriber,
            topic,
            qos_provider.datareader_qos(
                    "profiles_Library::transient_local_profile"),
            transient_listener);

    // Create a shared pointer for the ProfilesListener Class
    auto volatile_listener =
            std::make_shared<ProfilesListener>("volatile_profile");

    // Create a DataReader with the QoS profile "volatile_profile" that it is
    // inside the QoS library "profiles_Library".
    dds::sub::DataReader<profiles> reader_volatile(
            subscriber,
            topic,
            qos_provider.datareader_qos("profiles_Library::volatile_profile"),
            volatile_listener);

    // Main loop.
    for (unsigned int samples_read = 0;
         !application::shutdown_requested && samples_read < sample_count;
         samples_read++) {
        rti::util::sleep(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, Entity::Subscriber);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
