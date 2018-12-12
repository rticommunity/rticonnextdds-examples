/*
 * (c) Copyright, Real-Time Innovations, 2019.
 * All rights reserved.
 *
 * No duplications, whole or partial, manual or electronic, may be made
 * without express written permission.  Any such copies, or
 * revisions thereof, must display this notice unaltered.
 * This code contains trade secrets of Real-Time Innovations, Inc.
 */
#include <iostream>

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp> // for sleep()

#include "SensorData.hpp"

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<SensorAttributesCollection> topic(
            participant,
            "Example SensorAttributesCollection");

    // Create a DataWriter with default Qos (Publisher created in-line)
    dds::pub::DataWriter<SensorAttributesCollection> writer(dds::pub::Publisher(participant), topic);

    SensorAttributesCollection data;
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        for (size_t i = 0; i < data.sensor_array().size(); i++) {
            data.sensor_array().at(i).id(count);
            data.sensor_array().at(i).value((float) i / count);
            data.sensor_array().at(i).is_active(true);
        }

        std::cout << "Writing SensorAttributesCollection, count " << count << std::endl;
        writer.write(data);
        std::cout << data << std::endl;
        rti::util::sleep(dds::core::Duration(4));
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
        publisher_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

