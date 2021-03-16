/*
 * (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

#include <algorithm>
#include <iostream>

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>

#include "GroupCoherentExample.hpp"
#include "application.hpp"  

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::sub;
using namespace dds::sub::qos;
using namespace GroupCoherentExample;

template<typename T>
class PatientDRListener : public dds::sub::NoOpDataReaderListener<T>
{
public:
    void on_sample_lost(
            dds::sub::DataReader<T> &,
            const dds::core::status::SampleLostStatus &status)
    {
        if (status.extensions().last_reason() 
            == rti::core::status::SampleLostState::
                    lost_by_incomplete_coherent_set()) {
            std::cout << "Lost " << status.total_count_change() 
                      << " samples in an incomplete coherent set.\n";
        }
    }
};

void print_coherent_set_info(const dds::sub::SampleInfo& info)
{
    // The coherent_set_info in the SampleInfo is only present if the sample is
    // part of a coherent set. We therefore need to check if it is present
    // before accessing any of the members
    if (info.extensions().coherent_set_info().has_value()) {
        const rti::core::CoherentSetInfo& set_info =
                *info.extensions().coherent_set_info();
        std::cout << "Sample is part of "
                  << (set_info.incomplete_coherent_set() ? "an incomplete "
                                                         : "a complete ")
                  << "group coherent set with SN ("
                  << set_info.group_coherent_set_sequence_number() << ");\n" 
                  << std::endl;
    } else {
        std::cout << "Sample is not part of a coherent set.\n" << std::endl;
    }
}

void process_data(dds::sub::Subscriber subscriber)
{
    {
        CoherentAccess coherent_access(subscriber); // Begin coherent access
        std::vector<AnyDataReader> readers;

        // Get the list of readers with samples that have not been read yet
        find(subscriber, 
             dds::sub::status::SampleState::not_read(), 
             std::back_inserter(readers));

         // Iterate through the returned readers list and take their samples
         for (AnyDataReader reader : readers) {
             if (reader.topic_name() == "Alarm") {
                 dds::sub::LoanedSamples<Alarm> samples = 
                         reader.get<Alarm>().take();
                 for (auto& sample : samples) {
                     std::cout << sample << std::endl;
                     print_coherent_set_info(sample.info());
                 }
             } else if (reader.topic_name() == "HeartRate") {
                 dds::sub::LoanedSamples<HeartRate> samples = 
                         reader.get<HeartRate>().take();
                 for (auto& sample : samples) {
                     std::cout << sample << std::endl;
                     print_coherent_set_info(sample.info());
                 }
             } else if (reader.topic_name() == "Temperature") {
                 dds::sub::LoanedSamples<Temperature> samples = 
                         reader.get<Temperature>().take();
                 for (auto& sample : samples) {
                     std::cout << sample << std::endl;
                     print_coherent_set_info(sample.info());
                }
             }
        }
    } // end coherent access
} 

void run_subscriber_application(unsigned int domain_id, bool use_xml_qos)
{
    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create three Topics with names and a datatypes
    dds::topic::Topic<Alarm> alarm_topic(participant, "Alarm");
    dds::topic::Topic<HeartRate> heart_rate_topic(participant, "HeartRate");
    dds::topic::Topic<Temperature> temperature_topic(participant, "Temperature");

    SubscriberQos subscriber_qos;
    if (use_xml_qos) {
        // Retrieve the Subscriber QoS, from USER_QOS_PROFILES.xml.
        subscriber_qos = QosProvider::Default().subscriber_qos();
    } else {
        // Set the Subscriber QoS programatically
        subscriber_qos << Presentation::GroupAccessScope(true, true);
        // Uncomment this line to keep and deliver incomplete coherent sets to 
        // the application
        // subscriber_qos.policy<Presentation>().extensions().drop_incomplete_coherent_set(false);
    }

    dds::sub::Subscriber subscriber(participant, subscriber_qos);

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos;
    if (use_xml_qos) {
        // Retrieve the DataReader QoS, from USER_QOS_PROFILES.xml.
        reader_qos = QosProvider::Default().datareader_qos();
    } else {
        // Set the DataReader QoS programatically
        reader_qos << Reliability::Reliable() << History::KeepAll();
    }

    auto alarm_listener = std::make_shared<PatientDRListener<Alarm>>();
    auto heart_rate_listener = std::make_shared<PatientDRListener<HeartRate>>();
    auto temperature_listener = std::make_shared<PatientDRListener<Temperature>>();

    // We are installing a listener for the sample lost status in case an
    // incomplete coherent set is received and dropped (assuming the
    // PresentationQosPolicy::drop_incomplete_coherent_set is
    // true (the default))
    dds::sub::DataReader<Alarm> alarm_reader(
            subscriber,
            alarm_topic,
            reader_qos,
            alarm_listener,
            dds::core::status::StatusMask::sample_lost());
    dds::sub::DataReader<HeartRate> heart_rate_reader(
            subscriber,
            heart_rate_topic,
            reader_qos,
            heart_rate_listener,
            dds::core::status::StatusMask::sample_lost());
    dds::sub::DataReader<Temperature> temperature_reader(
            subscriber,
            temperature_topic,
            reader_qos,
            temperature_listener,
            dds::core::status::StatusMask::sample_lost());

    dds::core::cond::WaitSet waitset;

    dds::core::cond::StatusCondition cond(subscriber);
    dds::core::cond::Condition scond_from_handler = dds::core::null;
    cond.enabled_statuses(dds::core::status::StatusMask::data_on_readers());
    cond.extensions().handler([subscriber]() { process_data(subscriber); });

    waitset += cond;

    while (!application::shutdown_requested) {
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

    if (arguments.set_count != (std::numeric_limits<unsigned int>::max)()) {
        std::cerr << "The -s, --set_count argument is only applicable to the"
                     " publisher application. It will be ignored." 
                  << std::endl;
    }
    
    try {
        run_subscriber_application(arguments.domain_id, arguments.use_xml_qos);
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
