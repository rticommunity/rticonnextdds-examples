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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "keys.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

void new_instance_found(
        const keys &msg,
        std::map<int, dds::sub::status::InstanceState> &sampleState)
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
    } else if (sampleState[code] == dds::sub::status::InstanceState::alive()) {
        std::cout << "Error, 'new' already-active instance found; code = "
                  << code << std::endl;
    } else if (
            sampleState[code]
            == dds::sub::status::InstanceState::not_alive_no_writers()) {
        std::cout << "Found writer for instance; code = " << code << std::endl;
    } else if (
            sampleState[code]
            == dds::sub::status::InstanceState::not_alive_disposed()) {
        std::cout << "Found reborn instance; code = " << code << std::endl;
    }

    sampleState[code] = dds::sub::status::InstanceState::alive();
}

void instance_lost_writers(
        const keys &msg,
        std::map<int, dds::sub::status::InstanceState> &sampleState)
{
    std::cout << "Instance has no writers; code = " << msg.code() << std::endl;
    sampleState[msg.code()] =
            dds::sub::status::InstanceState::not_alive_no_writers();
}

void instance_disposed(
        const keys &msg,
        std::map<int, dds::sub::status::InstanceState> &sampleState)
{
    std::cout << "Instance disposed; code = " << msg.code() << std::endl;
    sampleState[msg.code()] =
            dds::sub::status::InstanceState::not_alive_disposed();
}

void handle_data(const keys &msg)
{
    std::cout << "code: " << msg.code() << ", x: " << msg.x()
              << ", y: " << msg.y() << std::endl;
}

int process_data(dds::sub::DataReader<keys> reader)
{
    int count = 0;
    // To read the first instance the handle must be InstanceHandle::nil()
    dds::core::InstanceHandle previous_handle =
            dds::core::InstanceHandle::nil();
    std::map<int, dds::sub::status::InstanceState> sampleState;
    dds::sub::LoanedSamples<keys> samples;
    do {
        samples = reader.select().next_instance(previous_handle).take();

        // Update the previous handle to read the next instance.
        if (samples.length() > 0) {
            previous_handle = samples[0].info().instance_handle();
        }

        for (const auto &sample : samples) {
            const dds::sub::SampleInfo &info = sample.info();
            if (info.valid()) {
                if (info.state().view_state()
                    == dds::sub::status::ViewState::new_view()) {
                    new_instance_found(sample.data(), sampleState);
                }

                handle_data(sample.data());
            } else {
                // Since there is not valid data, it may include metadata.
                keys key_sample;
                reader.key_value(key_sample, info.instance_handle());

                // Here we print a message and change the instance state
                // if the instance state is
                dds::sub::status::InstanceState inst_state =
                        info.state().instance_state();
                if (inst_state
                    == dds::sub::status::InstanceState::
                            not_alive_no_writers()) {
                    instance_lost_writers(key_sample, sampleState);
                } else if (
                        inst_state
                        == dds::sub::status::InstanceState::
                                not_alive_disposed()) {
                    instance_disposed(key_sample, sampleState);
                }
            }
            count++;
        }
    } while (samples.length() > 0);

    return count;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<keys> topic(participant, "Example keys");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // reader_qos << Ownership::Exclusive();

    // Create a subscriber with default QoS
    dds::sub::Subscriber subscriber(participant);

    // Create a DataReader with default Qos
    dds::sub::DataReader<keys> reader(subscriber, topic, reader_qos);

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;

    // Create a ReadCondition for any data on this reader, and add to WaitSet
    unsigned int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [reader, &samples_read]() {
                samples_read += process_data(reader);
            });

    waitset += read_condition;

    // Main loop.
    while (!application::shutdown_requested && samples_read < sample_count) {
        waitset.dispatch(dds::core::Duration(2));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
