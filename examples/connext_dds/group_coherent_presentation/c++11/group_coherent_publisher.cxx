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

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      
#include <rti/config/Logger.hpp>

#include "application.hpp"
#include "group_coherent.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::pub::qos;

void run_publisher_application(unsigned int domain_id, unsigned int set_count)
{
    // Start communicating in a domain, usually one participant per application
    dds::domain::DomainParticipant participant(domain_id);

    // Create two Topics with names and a datatypes
    dds::topic::Topic<CheckpointTime> time_topic(
            participant, 
            "Checkpoint Time");
    dds::topic::Topic<CheckpointPlace> place_topic(
            participant, 
            "Checkpoint Place");

    // Retrieve the Publisher QoS, from USER_QOS_PROFILES.xml.
    PublisherQos publisher_qos = QosProvider::Default().publisher_qos();

    // If you want to change the Publisher's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following line.
    // publisher_qos << Presentation::GroupAccessScope(true, true);

    dds::pub::Publisher publisher(participant, publisher_qos);  

    // Retrieve the DataWriter QoS, from USER_QOS_PROFILES.xml.
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following lines.
    // writer_qos << Reliability::Reliable()
    //            << History::KeepAll();

    // Create a DataWriter for each topic
    dds::pub::DataWriter<CheckpointTime> time_writer(
            publisher, time_topic, writer_qos);
    dds::pub::DataWriter<CheckpointPlace> place_writer(
            publisher, place_topic, writer_qos);

    CheckpointTime time_data;
    CheckpointPlace place_data;

    int32_t competitor_id = 1;
    int32_t checkpoint = 1;

    // Publish some initial info for each competitor, before the race starts
    // This data is not published in a coherent set 
    place_data.competitor_id(1);
    place_data.checkpoint(0);
    place_data.place(0);
    place_writer.write(place_data);

    place_data.competitor_id(2);
    place_data.checkpoint(0);
    place_data.place(0);
    place_writer.write(place_data);

    // Below we will write a coherent set with each iteration of the loop. 
    // Think of each iteration as representing a checkpoint in a race where we 
    // want to publish information about each competitor, time and place, at 
    // that checkpoint. Because the time and place are being published on 
    // separate topics, we want to make sure these updates are received as a 
    // set on the receiving end so that we don't report the competitors' 
    // times and places from different checkoints in a race
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < set_count;
         samples_written++, checkpoint++) {

         rti::util::sleep(dds::core::Duration(4));
         {
             dds::pub::CoherentSet coherent_set(publisher); // Start a coherent set
            // Competitor 1
            time_data.competitor_id(1);
            time_data.checkpoint(checkpoint);
            time_data.time().sec(static_cast<int32_t>(samples_written));
            
            place_data.competitor_id(1);
            place_data.checkpoint(checkpoint);
            place_data.place(1);

            time_writer.write(time_data);
            place_writer.write(place_data);

            // Competitor 2
            competitor_id++;
            time_data.competitor_id(2);
            time_data.time().sec(static_cast<int32_t>(samples_written + 1));

            place_data.competitor_id(2);
            place_data.place(2);

            time_writer.write(time_data);
            place_writer.write(place_data);
        } // end coherent set
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
        run_publisher_application(arguments.domain_id, arguments.set_count);
    } catch (const std::exception& ex) {
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
