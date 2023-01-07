/*******************************************************************************
 (c) 2005-2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software. Licensee has the right to distribute object form only
 for use with RTI products. The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software. RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

#include <dds/dds.hpp>

#include "hello_world.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(
        int domain_id,
        int sample_count,
        int initial_value,
        int is_persistent,
        int sleep)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<hello_world> topic(participant, "Example hello_world");

    std::string qos_libname = "PersistenceExampleLibrary";
    DataWriterQos writer_qos = is_persistent
            ? QosProvider::Default().datawriter_qos(
                    qos_libname + "::PersistentProfile")
            : QosProvider::Default().datawriter_qos(
                    qos_libname + "::TransientProfile");

    // Create a DataWriter (Publisher created in-line)
    DataWriter<hello_world> writer(Publisher(participant), topic, writer_qos);

    hello_world sample;
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        std::cout << "Writing hello_world, count " << count << std::endl;
        sample.data(initial_value++);
        writer.write(sample);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while (sleep != 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        sleep--;
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0;  // Infinite loop
    int initial_value = 0;
    int is_persistent = 0;
    int sleep = 0;

    for (int i = 1; i < argc;) {
        const std::string &param = argv[i++];

        if (param == "-sleep" && i < argc) {
            sleep = atoi(argv[i++]);
        } else if (param == "-domain_id" && i < argc) {
            domain_id = atoi(argv[i++]);
        } else if (param == "-sample_count" && i < argc) {
            sample_count = atoi(argv[i++]);
        } else if (param == "-initial_value" && i < argc) {
            initial_value = atoi(argv[i++]);
        } else if (param == "-persistent" && i < argc) {
            is_persistent = atoi(argv[i++]);
        } else {
            std::cout << argv[0] << " [options]" << std::endl
                      << "\t-domain_id <domain ID> (default: 0)" << std::endl
                      << "\t-sample_count <number of published samples> "
                      << "(default: 0 (infinite))" << std::endl
                      << "\t-initial_value <first sample value> (default: 0)"
                      << std::endl
                      << "\t-persistent <1 if persistent durability should be used> "
                      << "(default: 0 (transient))" << std::endl
                      << "\t-sleep <sleep time in seconds before finishing> "
                      << "(default: 0)" << std::endl;
            return -1;
        }
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        publisher_main(
                domain_id,
                sample_count,
                initial_value,
                is_persistent,
                sleep);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
