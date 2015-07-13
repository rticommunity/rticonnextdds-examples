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
#include "flights.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos.
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<Flight> topic (participant, "Example Flight");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    // writer_qos << Reliability::Reliable();

    // Create a DataWriter.
    DataWriter<Flight> writer(Publisher(participant), topic, writer_qos);

    // Create the flight info samples.
    std::vector<Flight> flights_info(4);
    flights_info[0] = Flight(1111, "CompanyA", 15000);
    flights_info[1] = Flight(2222, "CompanyB", 20000);
    flights_info[2] = Flight(3333, "CompanyA", 30000);
    flights_info[3] = Flight(4444, "CompanyB", 25000);

    // Main loop
    for (int count = 0;
        (sample_count == 0) || (count < sample_count);
        count += flights_info.size()){

        // Update flight info latitude
        std::cout << "Updating and sending values" << std::endl;

        for (std::vector<int>::size_type f = 0; f < flights_info.size(); f++) {
            // Set the plane altitude lineally (usually the max is at 41,000ft).
            int altitude = flights_info[f].altitude() + count * 100;
            flights_info[f].altitude(altitude >= 41000 ? 41000 : altitude);
            std::cout << "\t" << flights_info[f] << std::endl;
        }

        writer.write(flights_info.begin(), flights_info.end());
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
        publisher_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
