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
#include <memory> // for unique_ptr

#include <signal.h>
#include "CameraImage.hpp"

#include <dds/sub/ddssub.hpp>
#include <dds/pub/ddspub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>

#include <rti/zcopy/sub/ZcopyDataReader.hpp>
#include <rti/zcopy/pub/ZcopyDataWriter.hpp>

#include "Common.hpp"

void subscriber_flat(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    while (1) {
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
                //last sample received, break out of receive loop
                break;
            }
            if (options.display_sample){
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

void subscriber_zero_copy(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    while (1) {
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
            if (options.display_sample){
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


void subscriber_flat_zero_copy(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    while (1) {
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
                    //last sample received, break out of receive loop
                    break;
                }
                if (options.display_sample){
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

void subscriber_plain(const ApplicationOptions &options)
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

    std::cout<< "Waiting for the publisher application\n";
    wait_for_reader(writer);
    wait_for_writer(reader);
    std::cout << "Discovery complete\n";

    int count = 0;
    while (1) {
        // Wait for a ping
        auto conditions = waitset.wait(dds::core::Duration(10));
        if (conditions.empty()) {
            std::cout << "Wait for ping: timeout\n";
        }
        auto ping_samples = reader.take();

        // Write the pong sample
        if (ping_samples.length() && ping_samples[0].info().valid()) {
            if (ping_samples[0].data().timestamp() == 0) {
                //last sample received, break out of receive loop
                break;
            }
            if (options.display_sample){
                display_plain_sample(ping_samples[0].data());
            }
            pong_sample->timestamp(ping_samples[0].data().timestamp());
                writer.write(*pong_sample);
                count++;
        }
    }
    std::cout << "Subscriber shutting down\n";
}

void print_help(const char *program_name) {
    std::cout << "Usage: " << program_name << "[options]\nOptions:\n";
    std::cout << " -domainId    <domain ID>    Domain ID\n";
    std::cout << " -mode        <1,2,3,4>      Subscriber modes\n";
    std::cout << "                                 1. subscriber_flat\n";
    std::cout << "                                 2. subscriber_zero_copy\n";
    std::cout << "                                 3. subscriber_flat_zero_copy\n";
    std::cout << "                                 4. subscriber_plain\n";
    std::cout << " -displaySample              Displays the sample\n";
    std::cout << " -nic         <IP address>   Use the nic specified by <ipaddr> to send\n";
    std::cout << "                             Default: 127.0.0.1\n";
    std::cout << " -help                       Displays this information\n";
}

int main(int argc, char *argv[])
{
    ApplicationOptions options;


    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i],"-h") == argv[i]) {
            print_help(argv[0]);
            return 0;
        } else if (strstr(argv[i],"-di") == argv[i]) {
            options.display_sample = true;
        } else if (strstr(argv[i],"-d") == argv[i]) {
            options.domain_id = atoi(argv[++i]);
        } else if (strstr(argv[i],"-m") == argv[i]) {
            options.mode = atoi(argv[++i]);
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
        switch(options.mode) {
            case 1:
                subscriber_flat(options);
                break;
            case 2:
                subscriber_zero_copy(options);
                break;
            case 3:
                subscriber_flat_zero_copy(options);
                break;
            case 4:
                subscriber_plain(options);
                break;
        }

    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber: " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

