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

#include <iostream>
#include <random>

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>
#include <rti/config/Logger.hpp>

#include "application.hpp"
#include "GroupCoherentExample.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::pub::qos;
using namespace GroupCoherentExample;

// Generate patient data
std::default_random_engine generator;
std::uniform_int_distribution<int32_t> distrib(0, 3);

int32_t get_patient_heart_rate()
{
    static int32_t heart_rates[4] = { 35, 56, 85, 110 };
    int32_t index = distrib(generator);
    return heart_rates[index];
}

float get_patient_temperature()
{
    static float temperatures[4] = { 95.0, 98.1, 99.2, 101.7 };
    int32_t index = distrib(generator);
    return temperatures[index];
}

void run_publisher_application(
        unsigned int domain_id,
        unsigned int set_count,
        bool use_xml_qos)
{
    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create three Topics with names and a datatypes
    dds::topic::Topic<Alarm> alarm_topic(participant, "Alarm");
    dds::topic::Topic<HeartRate> heart_rate_topic(participant, "HeartRate");
    dds::topic::Topic<Temperature> temperature_topic(
            participant,
            "Temperature");

    PublisherQos publisher_qos;
    if (use_xml_qos) {
        // Retrieve the Publisher QoS, from USER_QOS_PROFILES.xml.
        publisher_qos = QosProvider::Default().publisher_qos();
    } else {
        // Set the Publisher QoS programatically
        publisher_qos << Presentation::GroupAccessScope(true, true);
    }

    dds::pub::Publisher publisher(participant, publisher_qos);

    DataWriterQos writer_qos;
    if (use_xml_qos) {
        // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml.
        writer_qos = QosProvider::Default().datawriter_qos();
    } else {
        // Set the DataWriter QoS programatically
        writer_qos << Reliability::Reliable() << History::KeepAll();
    }

    // Create a DataWriter for each topic
    dds::pub::DataWriter<Alarm> alarm_writer(
            publisher,
            alarm_topic,
            writer_qos);
    dds::pub::DataWriter<HeartRate> heart_rate_writer(
            publisher,
            heart_rate_topic,
            writer_qos);
    dds::pub::DataWriter<Temperature> temperature_writer(
            publisher,
            temperature_topic,
            writer_qos);

    Alarm alarm_data;
    alarm_data.patient_id(1);
    alarm_data.alarm_code(AlarmCode::PATIENT_OK);
    HeartRate heart_rate_data;
    heart_rate_data.patient_id(1);
    heart_rate_data.beats_per_minute(65);
    Temperature temperature_data;
    temperature_data.patient_id(1);
    temperature_data.temperature(98.6);

    // Below we will write a coherent set any time the patient's vitals are
    // abnormal. Otherwise, we will publish the patient's vitals normally
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < set_count;
         samples_written++) {
        heart_rate_data.beats_per_minute(get_patient_heart_rate());
        temperature_data.temperature(get_patient_temperature());

        if (heart_rate_data.beats_per_minute() >= 100
            || heart_rate_data.beats_per_minute() <= 40
            || temperature_data.temperature() >= 100.0
            || temperature_data.temperature() <= 95.0) {
            // Sound an alarm. In this case, we want all of the patients vitals
            // along with the alarm to be delivered as a single coherent set of
            // data so that we can correlate the alarm with the set of vitals
            // that triggered it
            {
                dds::pub::CoherentSet coherent_set(publisher);  // Start a
                                                                // coherent set
                heart_rate_writer.write(heart_rate_data);
                temperature_writer.write(temperature_data);

                alarm_data.alarm_code(AlarmCode::ABNORMAL_READING);
                alarm_writer.write(alarm_data);
            }  // end coherent set
        } else {
            // No alarm necessary, publish the patient's vitals as normal
            heart_rate_writer.write(heart_rate_data);
            temperature_writer.write(temperature_data);
        }
        rti::util::sleep(dds::core::Duration(2));
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
        run_publisher_application(
                arguments.domain_id,
                arguments.set_count,
                arguments.use_xml_qos);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
