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

#include "msg.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

int process_data(
        dds::sub::DataReader<dds::topic::PublicationBuiltinTopicData> reader)
{
    int count = 0;
    // Take all samples
    dds::sub::LoanedSamples<dds::topic::PublicationBuiltinTopicData> samples =
            reader.take();

    for (const auto &sample : samples) {
        if (sample.info().valid()) {
            const dds::topic::PublicationBuiltinTopicData &data = sample.data();
            const dds::topic::BuiltinTopicKey &partKey = data.participant_key();
            const dds::topic::BuiltinTopicKey &key = sample.data().key();

            std::cout << std::hex << std::setw(8) << std::setfill('0');
            std::cout << "-----" << std::endl
                      << "Found topic \"" << data.topic_name() << "\""
                      << std::endl
                      << "participant: " << partKey.value()[0]
                      << partKey.value()[1] << partKey.value()[2] << std::endl
                      << "datawriter:  " << key.value()[0] << key.value()[1]
                      << key.value()[2] << std::endl
                      << "type:" << std::endl;

            if (!data->type().is_set()) {
                std::cout << "No type received, perhaps increase "
                          << "type_code_max_serialized_length?" << std::endl;
            } else {
                // Using the type propagated we print the data type
                // with print_idl()
                rti::core::xtypes::print_idl(data->type().get(), 2);
            }

            count++;
        }
    }

    return count;
}

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Retrieve the Participant QoS, from USER_QOS_PROFILES.xml
    dds::domain::qos::DomainParticipantQos participant_qos =
            dds::core::QosProvider::Default()->participant_qos();

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // participant_qos << DomainParticipantResourceLimits().
    //     type_code_max_serialized_length(3070);

    // Create a DomainParticipant.
    dds::domain::DomainParticipant participant(domain_id, participant_qos);

    // First get the builtin subscriber.
    dds::sub::Subscriber builtin_subscriber =
            dds::sub::builtin_subscriber(participant);

    // Then get builtin subscriber's DataReader for DataWriters.
    dds::sub::DataReader<dds::topic::PublicationBuiltinTopicData>
            publication_reader = rti::sub::find_datareader_by_topic_name<
                    dds::sub::DataReader<
                            dds::topic::PublicationBuiltinTopicData>>(
                    builtin_subscriber,
                    dds::topic::publication_topic_name());

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;

    // Create a ReadCondition for any data on this reader, and add to WaitSet
    unsigned int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            publication_reader,
            dds::sub::status::DataState::any(),
            [publication_reader, &samples_read]() {
                samples_read += process_data(publication_reader);
            });

    waitset += read_condition;

    while (!application::shutdown_requested && samples_read < sample_count) {
        waitset.dispatch(dds::core::Duration(1));
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
