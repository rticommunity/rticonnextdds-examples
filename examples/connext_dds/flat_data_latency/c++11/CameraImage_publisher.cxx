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
#include <memory> // for shared_ptr and unique_ptr
#include <signal.h>

#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>

#include <rti/config/Logger.hpp>
#include <rti/util/util.hpp> // for sleep()
#include <rti/core/ListenerBinder.hpp>

#include "CameraImage.hpp"

#include <rti/zcopy/sub/ZcopyDataReader.hpp>
#include <rti/zcopy/pub/ZcopyDataWriter.hpp>

#include "Common.hpp"

void publisher_flat(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    int count = 0;
    uint64_t total_latency = 0;
    uint64_t latency_interval_start_time = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while(count <= options.sample_count || options.sample_count == -1) {
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
    print_latency(total_latency, count);

    // Wait for unmatch
    wait_for_reader(writer, false);
    std::cout << "Publisher shutting down\n";
}

void publisher_zero_copy(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    // In this loop we write pings and wait for pongs, measuring the end-to-end
    // latency
    uint64_t total_latency = 0;
    uint64_t latency_interval_start_time = 0;
    int count = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while (count <= options.sample_count || options.sample_count == -1) {

        // Create and write the ping sample:
        // The DataWriter gets a sample from its shared memory sample pool. This
        // operation does not allocate memory. Samples are returned to the sample
        // pool when they are returned from the DataWriter queue.
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
    print_latency(total_latency, count);

    // Wait for unmatch
    wait_for_reader(writer, false);
    std::cout << "Publisher shutting down\n";
}

void publisher_flat_zero_copy(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    // In this loop we write pings and wait for pongs, measuring the end-to-end
    // latency
    uint64_t total_latency = 0;
    int count = 0;
    uint64_t latency_interval_start_time = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while (count <= options.sample_count || options.sample_count == -1) {
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

void publisher_plain(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the subscriber application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    int count = 0;
    uint64_t total_latency = 0;
    uint64_t latency_interval_start_time = 0;
    uint64_t start_ts = participant->current_time().to_microsecs();
    while(count <= options.sample_count || options.sample_count == -1) {
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

void publisher_copy_sample(int domain_id, int sample_count)
{
    using namespace plain_types;

    std::cout << "Running publisher_copy_sample\n";

    dds::domain::DomainParticipant participant (domain_id);

    std::unique_ptr<CameraImage> ping_sample(new CameraImage);
    std::unique_ptr<CameraImage> copy(new CameraImage);

    int count = 0;
    uint64_t total_latency = 0;
    while(count < sample_count || sample_count == 0) {
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

void print_help(const char *program_name) {
    std::cout << "Usage: " << program_name << "[options]\nOptions:\n";
    std::cout << " -domainId    <domain ID>    Domain ID\n";
    std::cout << " -mode        <1,2,3,4>      Publisher modes\n";
    std::cout << "                                 1. publisher_flat\n";
    std::cout << "                                 2. publisher_zero_copy\n";
    std::cout << "                                 3. publisher_flat_zero_copy\n";
    std::cout << "                                 4. publisher_plain\n";
    std::cout << " -sampleCount <sample count> Sample count\n";
    std::cout << "                             Default: -1 (infinite)\n";
    std::cout << " -executionTime <sec>        Execution time in seconds\n";
    std::cout << "                             Default: 30\n";
    std::cout << " -nic         <IP address>   Use the nic specified by <ipaddr> to send\n";
    std::cout << "                             Default: automatic\n";
    std::cout << " -help                       Displays this information\n";
}

int main(int argc, char *argv[])
{
    ApplicationOptions options;

    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i],"-h") == argv[i]) {
            print_help(argv[0]);
            return 0;
        } else if (strstr(argv[i],"-d") == argv[i]) {
            options.domain_id = atoi(argv[++i]);
        } else if (strstr(argv[i],"-m") == argv[i]) {
            options.mode = atoi(argv[++i]);
        } else if (strstr(argv[i],"-s") == argv[i]) {
            options.sample_count = atoi(argv[++i]);
        } else if (strstr(argv[i],"-e") == argv[i]) {
            options.execution_time = atoi(argv[++i])*1000000;
        } else if (strstr(argv[i],"-n") == argv[i]) {
            options.nic = (argv[++i]);
        } else {
            std::cout << "unexpected option: " << argv[i] << std::endl;
            return -1;
        }
    }


    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::WARNING);

    try {
        switch (options.mode) {
            case 1:
                publisher_flat(options);
                break;
            case 2:
                publisher_zero_copy(options);
                break;
            case 3:
                publisher_flat_zero_copy(options);
                break;
            case 4:
                publisher_plain(options);
                break;
            case 5:
                publisher_copy_sample(0, 0);
                break;
        }

    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher: " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

