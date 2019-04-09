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

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "keys.hpp"

using namespace dds::core;
using namespace rti::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::status;

class KeysReaderListener : public NoOpDataReaderListener<keys> {
  public:
    void on_data_available(DataReader<keys>& reader)
    {
        // Take all samples
        LoanedSamples<keys> samples = reader.take();

        for (LoanedSamples<keys>::iterator sample_it = samples.begin();
             sample_it != samples.end();
             sample_it++) {

            const SampleInfo& info = sample_it->info();
            if (info.valid()) {
                ViewState view_state = info.state().view_state();
                if (view_state == ViewState::new_view()) {
                    std::cout << "Found new instance; code = "
                              << sample_it->data().code() << std::endl;
                }

                std::cout << "Instance " << sample_it->data().code()
                          << ", x: " << sample_it->data().x()
                          << ", y: " << sample_it->data().y() << std::endl;
            } else {
                // Since there is not valid data, it may include metadata.
                keys sample;
                reader.key_value(sample, info.instance_handle());

                // Here we print a message if the instance state is
                // 'not_alive_no_writers' or 'not_alive_disposed'.
                const InstanceState& state = info.state().instance_state();
                if (state == InstanceState::not_alive_no_writers()) {
                    std::cout << "Instance " << sample.code()
                              << " has no writers" << std::endl;
                } else if (state == InstanceState::not_alive_disposed()){
                    std::cout << "Instance " << sample.code() << " disposed"
                              << std::endl;
                }
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<keys> topic(participant, "Example keys");

    // Create a DataReader with default Qos (Subscriber created in-line)
    DataReader<keys> reader(Subscriber(participant), topic);

    // Associate a listener to the datareader using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    ListenerBinder< DataReader<keys> > reader_listener =
        bind_and_manage_listener(
            reader,
            new KeysReaderListener,
            dds::core::status::StatusMask::all());

    // Main loop
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        rti::util::sleep(Duration(1));
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop

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

    return 0;
}
