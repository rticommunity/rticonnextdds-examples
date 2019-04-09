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

#include "batch_data.hpp"
#include <dds/dds.hpp>

using namespace dds::core;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::pub;

void publisher_main(int domain_id, int sample_count, bool turbo_mode_on)
{
    // Seconds to wait between samples published.
    Duration send_period (1);

    // We pick the profile name if the turbo_mode is selected or not.
    // If turbo_mode is not selected, the batching profile will be used.
    std::string profile_name = "batching_Library::";
    if (turbo_mode_on) {
        std::cout << "Turbo Mode enable" << std::endl;
        profile_name.append("turbo_mode_profile");

        // If turbo_mode, we do not want to wait to send samples.
        send_period.sec(0);
    } else {
        std::cout << "Manual batching enable" << std::endl;
        profile_name.append("batch_profile");
    }

    // To customize entities QoS use the file USER_QOS_PROFILES.xml
    DomainParticipant participant (
        domain_id,
        QosProvider::Default().participant_qos(profile_name));

    Topic<batch_data> topic (participant, "Example batch_data");

    Publisher publisher (
        participant,
        QosProvider::Default().publisher_qos(profile_name));

    DataWriter<batch_data> writer (
        publisher,
        topic,
        QosProvider::Default().datawriter_qos(profile_name));

    // Create data sample for writing.
    batch_data sample;

    // For a data type that has a key, if the same instance is going to be
    // written multiple times, initialize the key here and register the keyed
    // instance prior to writing.
    InstanceHandle instance_handle = InstanceHandle::nil();
    //instance_handle = writer.register_instance(sample);

    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        // Modify the data to be written here
        sample.x(count);

        std::cout << "Writing batch_data, count " << count << std::endl;
        writer.write(sample);

        rti::util::sleep(send_period);
    }

    //writer.unregister_instance(sample);
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // infinite loop
    bool turbo_mode_on = false;

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        turbo_mode_on = (argv[2][0] == '1');
    }

    if (argc >= 4) {
        sample_count = atoi(argv[3]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        publisher_main(domain_id, sample_count, turbo_mode_on);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
