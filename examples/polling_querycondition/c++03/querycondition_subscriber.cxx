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

#include "querycondition.hpp"
#include <dds/dds.hpp>

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::cond;
using namespace dds::sub::status;

void subscriber_main(int domain_id, int sample_count)
{
    DomainParticipant participant(domain_id);

    Topic<querycondition> topic(participant, "Example querycondition");

    DataReader<querycondition> reader(Subscriber(participant), topic);

    // Query for 'GUID2'. This query parameter can be changed at runtime
    // allowing an application to selectively look at subsets of data
    // at different times.
    // NOTE: There must be single-quotes in the query parameters around-any
    // strings! The single-quote do NOT go in the query condition itself.
    std::vector<std::string> query_parameters(1);
    query_parameters[0] = "'GUID2'";

    // Create the query condition with an expession to MATCH the id field in
    // the structure.
    QueryCondition query_condition(
        Query(reader, "id MATCH %0", query_parameters),
        DataState::any_data());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        // Poll for new samples every 5 seconds
        rti::util::sleep(Duration(5));

        // Get new samples selecting them with a condition.
        LoanedSamples<querycondition> samples = reader.select()
            .condition(query_condition)
            .read();

        int len = 0;
        double sum = 0;

        // Iterate through the samples read accessing only the samples of GUID2.
        // Then, show the number of samples received and, adding the value
        // on each of them to calculate the average afterwards.
        for (LoanedSamples<querycondition>::iterator sampleIt = samples.begin();
                sampleIt != samples.end();
                ++sampleIt) {

            if (!sampleIt->info().valid())
                continue;

            len++;
            sum += sampleIt->data().value();
            std::cout << "Guid = " << sampleIt->data().id() << std::endl;
        }

        if (len > 0)
            std::cout << "Got " << len << " samples. Avg = "
                      << sum / len << std::endl;
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
