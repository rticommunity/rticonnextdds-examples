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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>

#include "batch_data.hpp"


int process_data(dds::sub::DataReader<batch_data> reader)
{
    // Take all samples
    int count = 0;
    dds::sub::LoanedSamples<batch_data> samples = reader.take();
    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            count++;
            std::cout << sample.data() << std::endl;
        } else {
            std::cout << "Instance state changed to "
                      << sample.info().state().instance_state() << std::endl;
        }
    }

    return count;
}

void subscriber_main(int domain_id, int sample_count, bool turbo_mode_on)
{
    // We pick the profile name if the turbo_mode is selected or not.
    // If turbo_mode is not selected, the batching profile will be used.
    std::string profile_name = "batching_Library::";
    if (turbo_mode_on) {
        std::cout << "Turbo Mode enable" << std::endl;
        profile_name.append("turbo_mode_profile");
    } else {
        std::cout << "Manual batching enable" << std::endl;
        profile_name.append("batch_profile");
    }

    // To customize entities QoS use the file USER_QOS_PROFILES.xml
    dds::domain::DomainParticipant participant(
            domain_id,
            dds::core::QosProvider::Default().participant_qos(profile_name));

    dds::topic::Topic<batch_data> topic(participant, "Example batch_data");

    dds::sub::Subscriber subscriber(
            participant,
            dds::core::QosProvider::Default().subscriber_qos(profile_name));

    dds::sub::DataReader<batch_data> reader(
            subscriber,
            topic,
            dds::core::QosProvider::Default().datareader_qos(profile_name));

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;

    // Create a ReadCondition for any data on this reader, and add to WaitSet
    unsigned int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [reader, &samples_read]() {
                // If we wake up, process data
                samples_read += process_data(reader);
            });

    waitset += read_condition;

    // Main loop
    for (int count = 0; count < sample_count || sample_count == 0; ++count) {
        std::cout << "batch_data subscriber sleeping for 4 sec...\n";


        // Wait for data and report if it does not arrive in 1 second
        waitset.dispatch(dds::core::Duration(4));
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0;  // Infinite loop
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
        subscriber_main(domain_id, sample_count, turbo_mode_on);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << "\n";
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
