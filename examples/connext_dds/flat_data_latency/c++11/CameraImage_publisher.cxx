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


#include <iostream>
#include <memory>  // for shared_ptr and unique_ptr
#include <signal.h>

#include <dds/core/ddscore.hpp>
#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>

#include <rti/config/Logger.hpp>
#include <rti/util/util.hpp>  // for sleep()

#include "CameraImage.hpp"

#include <rti/zcopy/pub/ZcopyDataWriter.hpp>
#include <rti/zcopy/sub/ZcopyDataReader.hpp>

#include "Common.hpp"

void publisher_flat(const application::ApplicationArguments &options)
{
    using namespace flat_types;
    using namespace dds::core::policy;

    std::cout << "Running publisher_flat\n";

    auto participant_qos = dds::core::QosProvider::Default().participant_qos();
    configure_nic(participant_qos, options.nic);
    dds::domain::DomainParticipant participant(
            options.domain_id,
            participant_qos);

    // Create the ping DataWriter
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");

    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            ping_topic);

    // Create the pong DataReader
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");
    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            pong_topic);

    // Create a ReadCondition for any data on the pong reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::cout << "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    int count = 0;
    uint64_t total_latency = 0;
    uint64_t latency_interval_start_time = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while (!application::shutdown_requested && count < options.sample_count) {
        // Get and populate the sample
        auto ping_sample = writer.extensions().get_loan();
        populate_flat_sample(*ping_sample, count);

        // Write the ping sample:
        auto ping = ping_sample->root();
        uint64_t send_ts = participant->current_time().to_microsecs();
        if ((count == options.sample_count)
            || (send_ts - start_ts >= options.execution_time)) {
            // notify reader about last sample
            ping.timestamp(0);
            writer.write(*ping_sample);
            break;
        }
        ping.timestamp(send_ts);

        // Write the ping sample.
        //
        // The DataWriter now owns the buffer and the application should not
        // reuse the sample.
        writer.write(*ping_sample);
        if (latency_interval_start_time == 0) {
            latency_interval_start_time = send_ts;
        }

        // Wait for the pong
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for pong: timeout\n";
        }

        auto pong_samples = reader.take();
        if (pong_samples.length() > 0 && pong_samples[0].info().valid()) {
            count++;
            auto pong = pong_samples[0].data().root();
            uint64_t receive_ts = participant->current_time().to_microsecs();
            uint64_t latency = receive_ts - pong.timestamp();
            total_latency += latency;
            if (receive_ts - latency_interval_start_time > 4000000) {
                print_latency(total_latency, count);
                latency_interval_start_time = 0;
            }
        }
    }

    // Wait for unmatch
    wait_for_reader(writer, false);
    std::cout << "Publisher shutting down\n";
}

void publisher_zero_copy(const application::ApplicationArguments &options)
{
    using namespace zero_copy_types;
    using namespace rti::core::policy;

    std::cout << "Running publisher_zero_copy\n";

    dds::domain::DomainParticipant participant(options.domain_id);

    // Create the ping DataWriter.
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");
    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            ping_topic);

    // Create the pong DataReader
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");

    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            pong_topic);

    // Create a ReadCondition for any data on the pong reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::cout << "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    // In this loop we write pings and wait for pongs, measuring the end-to-end
    // latency
    uint64_t total_latency = 0;
    uint64_t latency_interval_start_time = 0;
    int count = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while (!application::shutdown_requested && count < options.sample_count) {
        // Create and write the ping sample:
        // The DataWriter gets a sample from its shared memory sample pool. This
        // operation does not allocate memory. Samples are returned to the
        // sample pool when they are returned from the DataWriter queue.
        CameraImage *ping_sample = writer.extensions().get_loan();

        // Populate the sample
        populate_plain_sample(*ping_sample, count);

        uint64_t send_ts = participant->current_time().to_microsecs();
        if ((count == options.sample_count)
            || (send_ts - start_ts >= options.execution_time)) {
            // notify reader about last sample
            ping_sample->timestamp(0);
            writer.write(*ping_sample);
            break;
        }
        ping_sample->timestamp(send_ts);
        writer.write(*ping_sample);

        if (latency_interval_start_time == 0) {
            latency_interval_start_time = send_ts;
        }
        // Wait for the pong
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for pong: timeout\n";
        }

        auto pong_samples = reader.take();
        if (pong_samples.length() > 0 && pong_samples[0].info().valid()) {
            count++;
            uint64_t recv_ts = participant->current_time().to_microsecs();
            uint64_t latency = recv_ts - pong_samples[0].data().timestamp();
            total_latency += latency;
            if (recv_ts - latency_interval_start_time > 4000000) {
                print_latency(total_latency, count);
                latency_interval_start_time = 0;
            }
        }
    }

    // Wait for unmatch
    wait_for_reader(writer, false);
    std::cout << "Publisher shutting down\n";
}

void publisher_flat_zero_copy(const application::ApplicationArguments &options)
{
    using namespace flat_zero_copy_types;
    using namespace rti::core::policy;
    using namespace dds::core::policy;

    std::cout << "Running publisher_flat_zero_copy\n";

    auto participant_qos = dds::core::QosProvider::Default().participant_qos();
    configure_nic(participant_qos, options.nic);
    dds::domain::DomainParticipant participant(
            options.domain_id,
            participant_qos);

    // Create the ping DataWriter. We configure the pool of shared-memory
    // samples to contain 10 at all times.
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");

    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            ping_topic);

    // Create the pong DataReader
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");

    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            pong_topic);

    // Create a ReadCondition for any data on the pong reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::cout << "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    // In this loop we write pings and wait for pongs, measuring the end-to-end
    // latency
    uint64_t total_latency = 0;
    int count = 0;
    uint64_t latency_interval_start_time = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while (!application::shutdown_requested && count < options.sample_count) {
        // Create and write the ping sample:
        CameraImage *ping_sample = writer.extensions().get_loan();

        // Populate the sample
        populate_flat_sample(*ping_sample, count);

        auto ping = ping_sample->root();
        uint64_t send_ts = participant->current_time().to_microsecs();
        if ((count == options.sample_count)
            || (send_ts - start_ts >= options.execution_time)) {
            // notify reader about last sample
            ping.timestamp(0);
            writer.write(*ping_sample);
            break;
        }

        ping.timestamp(send_ts);
        writer.write(*ping_sample);
        if (latency_interval_start_time == 0) {
            latency_interval_start_time = send_ts;
        }
        // Wait for the pong
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for pong: timeout\n";
        }

        auto pong_samples = reader.take();
        if (pong_samples.length() > 0 && pong_samples[0].info().valid()) {
            count++;
            auto pong = pong_samples[0].data().root();
            uint64_t recv_ts = participant->current_time().to_microsecs();
            uint64_t latency = recv_ts - pong.timestamp();
            total_latency += latency;
            if (recv_ts - latency_interval_start_time > 4000000) {
                print_latency(total_latency, count);
                latency_interval_start_time = 0;
            }
        }
    }
    print_latency(total_latency, count);

    // Wait for unmatch
    wait_for_reader(writer, false);
    std::cout << "Publisher shutting down\n";
}

void publisher_plain(const application::ApplicationArguments &options)
{
    using namespace plain_types;
    using namespace dds::core::policy;

    std::cout << "Running publisher_plain\n";

    auto participant_qos = dds::core::QosProvider::Default().participant_qos();
    configure_nic(participant_qos, options.nic);
    dds::domain::DomainParticipant participant(
            options.domain_id,
            participant_qos);

    // Create the ping DataWriter
    dds::topic::Topic<CameraImage> ping_topic(participant, "CameraImagePing");

    // Create the pong DataWriter with a profile from USER_QOS_PROFILES.xml
    dds::pub::DataWriter<CameraImage> writer(
            dds::pub::Publisher(participant),
            ping_topic);

    // Create the pong DataReader
    dds::topic::Topic<CameraImage> pong_topic(participant, "CameraImagePong");
    dds::sub::DataReader<CameraImage> reader(
            dds::sub::Subscriber(participant),
            pong_topic);

    // Create a ReadCondition for any data on the pong reader, and attach it to
    // a Waitset
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any());
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::unique_ptr<CameraImage> ping_sample(new CameraImage);

    std::cout << "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    int count = 0;
    uint64_t total_latency = 0;
    uint64_t latency_interval_start_time = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while (!application::shutdown_requested && count < options.sample_count) {
        uint64_t send_ts = participant->current_time().to_microsecs();
        if ((count == options.sample_count)
            || (send_ts - start_ts >= options.execution_time)) {
            ping_sample->timestamp(0);
            writer.write(*ping_sample);
            break;
        }

        // Write the ping sample:
        ping_sample->timestamp(send_ts);
        writer.write(*ping_sample);
        if (latency_interval_start_time == 0) {
            latency_interval_start_time = send_ts;
        }

        // Wait for the pong
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for pong: timeout\n";
        }

        auto pong_samples = reader.take();
        if (pong_samples.length() > 0 && pong_samples[0].info().valid()) {
            count++;
            uint64_t recv_ts = participant->current_time().to_microsecs();
            uint64_t latency = recv_ts - pong_samples[0].data().timestamp();
            total_latency += latency;
            if (recv_ts - latency_interval_start_time > 4000000) {
                print_latency(total_latency, count);
                latency_interval_start_time = 0;
            }
        }
    }
    print_latency(total_latency, count);

    // Wait for unmatch
    wait_for_reader(writer, false);
    std::cout << "Publisher shutting down\n";
}

void publisher_copy_sample(unsigned int domain_id, unsigned int sample_count)
{
    using namespace plain_types;

    std::cout << "Running publisher_copy_sample\n";

    dds::domain::DomainParticipant participant(domain_id);

    std::unique_ptr<CameraImage> ping_sample(new CameraImage);
    std::unique_ptr<CameraImage> copy(new CameraImage);

    int count = 0;
    uint64_t total_latency = 0;
    while (!application::shutdown_requested && count < sample_count) {
        ping_sample->data()[45345] = count + sample_count + 100;
        ping_sample->timestamp(participant->current_time().to_microsecs());
        *copy = *ping_sample;
        if (copy->data()[45345] == 3) {
            return;
        }

        count++;

        uint64_t latency = participant->current_time().to_microsecs()
                - ping_sample->timestamp();
        total_latency += latency;
        if (count % 10 == 0) {
            std::cout << "Average end-to-end latency: "
                      << total_latency / (count * 1) << " microseconds\n";
        }
    }
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
        switch (arguments.mode) {
        case 1:
            publisher_flat(arguments);
            break;
        case 2:
            publisher_zero_copy(arguments);
            break;
        case 3:
            publisher_flat_zero_copy(arguments);
            break;
        case 4:
            publisher_plain(arguments);
            break;
        case 5:
            publisher_copy_sample(arguments.domain_id, arguments.sample_count);
            break;
        }

    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
