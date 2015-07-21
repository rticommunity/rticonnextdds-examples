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
using namespace dds::sub;
using namespace dds::sub::cond;
using namespace dds::sub::qos;
using namespace dds::sub::status;

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos.
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<Flight> topic(participant, "Example Flight");

     // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    // reader_qos << Reliability::Reliable();

    // Create a DataReader.
    DataReader<Flight> reader(Subscriber(participant), topic, reader_qos);

    // Query for company named 'CompanyA' and for flights in cruise
    // (about 30,000ft). The company parameter will be changed in run-time.
    // NOTE: There must be single-quotes in the query parameters around-any
    // strings! The single-quote do NOT go in the query condition itself.
    std::vector<std::string> query_parameters(2);
    query_parameters[0] = "'CompanyA'";
    query_parameters[1] = "30000";
    std::cout << "Setting parameters to company: " << query_parameters[0]
              << " and altitude bigger or equals to " << query_parameters[1]
              << std::endl;

    // Create the query condition with an expession to MATCH the id field in
    // the structure and a numeric comparison.
    QueryCondition query_condition(
        Query(reader, "company MATCH %0 AND altitude >= %1", query_parameters),
        DataState::any_data());

    // Main loop
    bool update = false;
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        // Poll for new samples every second.
        rti::util::sleep(Duration(1));

        // Change the filter parameter after 5 seconds.
        if ((count + 1) % 10 == 5) {
            query_parameters[0] = "'CompanyB'";
            update = true;
        } else if ((count + 1) % 10 == 0) {
            query_parameters[0] = "'CompanyA'";
            update = true;
        }

        // Set new parameters.
        if (update) {
            std::cout << "Changing parameter to " << query_parameters[0]
                      << std::endl;
            query_condition.parameters(
                query_parameters.begin(),
                query_parameters.end());
            update = false;
        }

        // Get new samples selecting them with a condition.
        LoanedSamples<Flight> samples = reader.select()
            .condition(query_condition)
            .read();

        for (LoanedSamples<Flight>::iterator sampleIt = samples.begin();
                sampleIt != samples.end();
                ++sampleIt) {

            if (!sampleIt->info().valid())
                continue;

            std::cout << "\t" << sampleIt->data() << std::endl;
        }
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
    // rti::config::Logger::instance()l.verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
