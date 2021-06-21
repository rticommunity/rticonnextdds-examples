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

#include <algorithm>
#include <iostream>
#include <memory>  // for unique_ptr

#include "CameraImage.hpp"
#include <signal.h>

#include <dds/core/ddscore.hpp>
#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include <rti/config/Logger.hpp>

#include <rti/zcopy/pub/ZcopyDataWriter.hpp>
#include <rti/zcopy/sub/ZcopyDataReader.hpp>

#include "Common.hpp"

void subscriber_flat(const application::ApplicationArguments &options)
{
    using namespace flat_types;
    using namespace dds::core::policy;

    std::cout << "Running subscriber_flat\n";

    auto participant_qos = dds::core::QosProvider::Default().participant_qos();
    configure_nic(participant_qos, options.nic);
    dds::domain::DomainParticipant participant(
            options.domain_id,
            participant_qos);

    // Create the ping DataReader
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");
    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            ping_topic);

    // Create the pong DataWriter
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");
    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            pong_topic);

    // Create a ReadCondition for any data on the ping reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::cout << "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    while (!application::shutdown_requested) {
        // Wait for a ping
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for ping: timeout\n";
            continue;
        }

        auto ping_samples = reader.take();
        if ((ping_samples.length() > 0) && (ping_samples[0].info().valid())) {
            auto ping = ping_samples[0].data().root();
            if (ping.timestamp() == 0) {
                // last sample received, break out of receive loop
                break;
            }
            if (options.display_sample) {
                display_flat_sample(ping_samples[0].data());
            }

            auto pong_sample = writer.extensions().get_loan();
            auto pong = pong_sample->root();
            pong.timestamp(ping.timestamp());
            writer.write(*pong_sample);
        }
    }
    std::cout << "Subscriber shutting down\n";
}

void subscriber_zero_copy(const application::ApplicationArguments &options)
{
    using namespace zero_copy_types;
    using namespace rti::core::policy;

    std::cout << "Running subscriber_zero_copy\n";

    dds::domain::DomainParticipant participant(options.domain_id);

    // Create the ping DataReader
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");
    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            ping_topic);

    // Create the pong DataWriter
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");

    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            pong_topic);

    // Create a ReadCondition for any data on the ping reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::cout << "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    while (!application::shutdown_requested) {
        // Wait for a ping
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for ping: timeout\n";
        }

        auto ping_samples = reader.take();
        if (ping_samples.length() > 0 && ping_samples[0].info().valid()) {
            if (ping_samples[0].data().timestamp() == 0) {
                // last sample received, break out of receive loop
                break;
            }
            if (options.display_sample) {
                display_plain_sample(ping_samples[0].data());
            }

            // Write the pong sample:
            CameraImage *pong_sample = writer.extensions().get_loan();
            pong_sample->timestamp(ping_samples[0].data().timestamp());
            if (reader->is_data_consistent(ping_samples[0])) {
                writer.write(*pong_sample);
            }
        }
    }
    std::cout << "Subscriber shutting down\n";
}


void subscriber_flat_zero_copy(const application::ApplicationArguments &options)
{
    using namespace flat_zero_copy_types;
    using namespace rti::core::policy;

    std::cout << "Running subscriber_flat_zero_copy\n";

    auto participant_qos = dds::core::QosProvider::Default().participant_qos();
    configure_nic(participant_qos, options.nic);
    dds::domain::DomainParticipant participant(
            options.domain_id,
            participant_qos);

    // Create the ping DataReader
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");
    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            ping_topic);

    // Create the pong DataWriter
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");
    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            pong_topic);

    // Create a ReadCondition for any data on the ping reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::cout << "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    while (!application::shutdown_requested) {
        // Wait for a ping
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for ping: timeout\n";
            continue;
        }

        auto ping_samples = reader.take();
        if (ping_samples.length() > 0 && ping_samples[0].info().valid()) {
            if (ping_samples[0].info().valid()) {
                auto ping = ping_samples[0].data().root();
                if (ping.timestamp() == 0) {
                    // last sample received, break out of receive loop
                    break;
                }
                if (options.display_sample) {
                    display_flat_sample(ping_samples[0].data());
                }

                // Write the pong sample:
                CameraImage *pong_sample = writer.extensions().get_loan();
                auto pong = pong_sample->root();
                pong.timestamp(ping.timestamp());
                if (reader->is_data_consistent(ping_samples[0])) {
                    writer.write(*pong_sample);
                }
            }
        }
    }
    std::cout << "Subscriber shutting down\n";
}

void subscriber_plain(const application::ApplicationArguments &options)
{
    using namespace plain_types;
    using namespace dds::core::policy;

    std::cout << "Running subscriber_plain\n";

    auto participant_qos = dds::core::QosProvider::Default().participant_qos();
    configure_nic(participant_qos, options.nic);
    dds::domain::DomainParticipant participant(
            options.domain_id,
            participant_qos);

    // Create the ping DataReader
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");

    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            ping_topic);

    // Create the pong DataWriter
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");
    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            pong_topic);

    // Create a ReadCondition for any data on the ping reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    // We create the sample in the heap because is to large to be in the stack
    std::unique_ptr<CameraImage> pong_sample(new CameraImage);

    std::cout << "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    int count = 0;
    while (!application::shutdown_requested) {
        // Wait for a ping
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for ping: timeout\n";
        }
        auto ping_samples = reader.take();

        // Write the pong sample
        if (ping_samples.length() && ping_samples[0].info().valid()) {
            if (ping_samples[0].data().timestamp() == 0) {
                // last sample received, break out of receive loop
                break;
            }
            if (options.display_sample) {
                display_plain_sample(ping_samples[0].data());
            }
            pong_sample->timestamp(ping_samples[0].data().timestamp());
            writer.write(*pong_sample);
            count++;
        }
    }
    std::cout << "Subscriber shutting down\n";
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, ApplicationKind::subscriber);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        switch (arguments.mode) {
        case 1:
            subscriber_flat(arguments);
            break;
        case 2:
            subscriber_zero_copy(arguments);
            break;
        case 3:
            subscriber_flat_zero_copy(arguments);
            break;
        case 4:
            subscriber_plain(arguments);
            break;
        }

    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
