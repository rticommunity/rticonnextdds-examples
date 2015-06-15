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
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

class BatchDataReaderListener : public NoOpDataReaderListener<batch_data> {
public:
    void on_data_available(DataReader<batch_data>& reader)
    {
        // Take all samples
        LoanedSamples<batch_data> samples = reader.take();
        for (LoanedSamples<batch_data>::iterator sample_it = samples.begin();
                sample_it != samples.end();
                sample_it++) {

            if (sample_it->info().valid()) {
                 std::cout << sample_it->data() << std::endl;
            }
        }
    }
};

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
    DomainParticipant participant (
        domain_id,
        QosProvider::Default().participant_qos(profile_name));

    Topic<batch_data> topic (participant, "Example batch_data");

    Subscriber subscriber (
        participant,
        QosProvider::Default().subscriber_qos(profile_name));

    DataReader<batch_data> reader(
        subscriber,
        topic,
        QosProvider::Default().datareader_qos(profile_name));

    // Set the listener using a RAII so it's exception-safe
    rti::core::ListenerBinder< DataReader<batch_data> > scoped_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new BatchDataReaderListener,
            StatusMask::data_available());

    // Main loop
    for (int count = 0; count < sample_count || sample_count == 0; ++count) {
        std::cout << "batch_data subscriber sleeping for 4 sec...\n";
        rti::util::sleep(Duration(4));
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop
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
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << "\n";
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
