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

#include <cstdlib>
#include <iostream>
#include <ctime>

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "cfc.hpp"

using namespace dds::core;
using namespace rti::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::sub;
using namespace dds::topic;

clock_t Init_time;

class cfcReaderListener : public NoOpDataReaderListener<cfc> {
  public:
    void on_data_available(DataReader<cfc>& reader)
    {
        // Take all samples
        LoanedSamples<cfc> samples = reader.take();

        for (LoanedSamples<cfc>::iterator sample_it = samples.begin();
            sample_it != samples.end();
            sample_it++) {

            if (sample_it->info().valid()){
                // Print the time we get each sample.
                double elapsed_ticks = clock() - Init_time;
                double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

                std::cout << "@ t=" << elapsed_secs << "s, got x = "
                          << sample_it->data().x() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // For timekeeping.
    Init_time = clock();

    // Retrieve the default Participant QoS, from USER_QOS_PROFILES.xml
    DomainParticipantQos participant_qos = QosProvider::Default()
        .participant_qos();

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    //
    // By default, data will be sent via shared memory "and" UDPv4.
    // Because the flowcontroller limits writes across all interfaces,
    // this halves the effective send rate. To avoid this, we enable only the
    // UDPv4 transport.

    // participant_qos << TransportBuiltin::UDPv4();

    // Create a DomainParticipant.
    DomainParticipant participant(domain_id, participant_qos);

    // Create a Topic -- and automatically register the type
    Topic<cfc> topic(participant, "Example cfc");

    // Create a DataReader with default Qos (Subscriber created in-line)
    DataReader<cfc> reader(Subscriber(participant), topic);

    // Associate a listener to the DataReader using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    ListenerBinder< DataReader<cfc> > reader_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new cfcReaderListener,
            StatusMask::all());

    std::cout << std::fixed;
    for (int count = 0; sample_count == 0 || count < sample_count; ++count) {
        rti::util::sleep(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{

    int domain_id = 0;
    int sample_count = 0; // infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
