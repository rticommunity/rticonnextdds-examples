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

#include <iostream>
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "keys.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace rti::sub;
using namespace dds::sub::cond;
using namespace dds::sub::qos;
using namespace dds::sub::status;

class KeysReaderListener : public NoOpDataReaderListener<keys> {
  public:
    void on_data_available(DataReader<keys>& reader)
    {
        // To read the first instance the handle must be InstanceHandle::nil()
        InstanceHandle previous_handle = InstanceHandle::nil();
        LoanedSamples<keys> samples;
        do {
            samples = reader.select()
                .next_instance(previous_handle)
                .take();

            // Update the previous handle to read the next instance.
            if (samples.length() > 0) {
                previous_handle = samples[0].info().instance_handle();
            }

            for (LoanedSamples<keys>::iterator sampleIt = samples.begin();
                sampleIt != samples.end();
                ++sampleIt) {

                const SampleInfo& info = sampleIt->info();
                if (info.valid()) {
                    if (info.state().view_state() == ViewState::new_view()) {
                        new_instance_found(sampleIt->data());
                    }

                    handle_data(sampleIt->data());
                } else {
                    // Since there is not valid data, it may include metadata.
                    keys key_sample;
                    reader.key_value(key_sample, info.instance_handle());

                    // Here we print a message and change the instance state
                    // if the instance state is
                    InstanceState inst_state = info.state().instance_state();
                    if (inst_state == InstanceState::not_alive_no_writers()) {
                        instance_lost_writers(key_sample);
                    } else if (inst_state==InstanceState::not_alive_disposed()){
                        instance_disposed(key_sample);
                    }
                }
            }
        } while (samples.length() > 0);
    }

    void new_instance_found(const keys& msg)
    {
        // There are three cases here:
        // 1.) truly new instance.
        // 2.) instance lost all writers, but now we're getting data again.
        // 3.) instance was disposed, but a new one has been created.
        //
        // We distinguish these cases by examining generation counts, BUT
        // note that if the instance resources have been reclaimed, the
        // generation counts may be reset to 0.
        //
        // Instances are eligible for resource cleanup if there are no
        // active writers and all samples have been taken.  To reliably
        // determine which case a 'new' instance falls into, the application
        // must store state information on a per-instance basis.
        //
        // Note that this example assumes that state changes only occur via
        // explicit register_instance(), unregister_instance() and dispose()
        // calls from the DataWriter. In reality, these changes could also
        // occur due to lost liveliness or missed deadlines, so those
        // listeners would also need to update the instance state.

        int code = msg.code();

        // If we don't have any information about it, it's a new instance.
        if (sampleState.count(code) == 0) {
            std::cout << "New instance found; code = " << code << std::endl;
        } else if (sampleState[code] == InstanceState::alive()) {
            std::cout << "Error, 'new' already-active instance found; code = "
                      << code << std::endl;
        } else if (sampleState[code] == InstanceState::not_alive_no_writers()) {
            std::cout << "Found writer for instance; code = " << code
                      << std::endl;
        } else if (sampleState[code] == InstanceState::not_alive_disposed()) {
            std::cout << "Found reborn instance; code = " << code << std::endl;
        }

        sampleState[code] = InstanceState::alive();
    }

    void instance_lost_writers(const keys& msg)
    {
        std::cout << "Instance has no writers; code = " << msg.code()
                  << std::endl;
        sampleState[msg.code()] = InstanceState::not_alive_no_writers();

    }

    void instance_disposed(const keys& msg)
    {
        std::cout << "Instance disposed; code = " << msg.code() << std::endl;
        sampleState[msg.code()] = InstanceState::not_alive_disposed();
    }

    void handle_data(const keys& msg)
    {
        std::cout << "code: " << msg.code() << ", x: " << msg.x()
                  << ", y: " << msg.y() << std::endl;
    }

private:
    std::map<int, InstanceState> sampleState;
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<keys> topic(participant, "Example keys");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // reader_qos << Ownership::Exclusive();

    // Create a DataReader with default Qos (Subscriber created in-line)
    DataReader<keys> reader(Subscriber(participant), topic, reader_qos);

    // Associate a listener using ListenerBinder, a RAII that will take care of
    // setting it to NULL on destruction.
    rti::core::ListenerBinder< DataReader<keys> > reader_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new KeysReaderListener,
            dds::core::status::StatusMask::all());

    // Main loop.
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        rti::util::sleep(Duration(2));
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
        std::cerr << "Exception in subscriber_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
