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

#include <cstdlib>
#include <iostream>

#include <dds/dds.hpp>
#include "waitset_query_cond.hpp"

using namespace dds::core;
using namespace dds::core::cond;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::cond;
using namespace dds::sub::qos;
using namespace dds::sub::status;

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant.
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<waitset_query_cond> topic(participant, "Example waitset_query_cond");

    // Retrieve the DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather
    // than using the XML file, you will need to uncomment the following lines.

    // reader_qos << History::KeepLast(1);

    // Create a DataReader.
    DataReader<waitset_query_cond> reader(
        Subscriber(participant),
        topic,
        reader_qos);

    // Create the parameters of the condition.
    // The initial value of the parameter is EVEN string to get even numbers.
    std::vector<std::string> query_parameters = { "'EVEN'" };

    // Create a query condition with a functor handler.
    QueryCondition query_condition(
        Query(reader, "name MATCH %0", query_parameters),
        DataState::any_data(),
        [&reader]()
        {
            // Using only 'take()' method will read all received samples.
            // To read only samples that triggered the QueryCondition
            // use 'select().condition().read()' as it's done in
            // the "polling_querycondition" example.
            LoanedSamples<waitset_query_cond> samples = reader.take();
            for (auto sample : samples) {
                if (!sample.info().valid()) {
                    std::cout << "Got metadata" << std::endl;
                } else {
                    std::cout << sample.data() << std::endl;
                }
            }
        }
    );

    // Create the waitset and attach the condition.
    WaitSet waitset;
    waitset += query_condition;

    Duration wait_timeout(1);
    std::cout << std::endl << ">>> Timeout: " << wait_timeout.sec() << " sec"
              << std::endl
              << ">>> Query conditions: " << query_condition.expression()
              << std::endl << "\t %%0 = " << query_parameters[0] << std::endl
              << "----------------------------------" << std::endl << std::endl;

    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        // We change the parameter in the Query Condition after 7 secs.
        if (count == 7) {
            query_parameters[0] = "'ODD'";
            query_condition.parameters(
                query_parameters.begin(), query_parameters.end());

            std::cout << std::endl<< "CHANGING THE QUERY CONDITION" << std::endl
                      << ">>> Query conditions: "<< query_condition.expression()
                      << std::endl << "\t %%0 = " << query_parameters[0]
                      << std::endl << ">>> We keep one sample in the history"
                      << std::endl << "-------------------------------------"
                      << std::endl << std::endl;
        }

        // 'dispatch()' blocks execution of the thread until one or more
        // attached Conditions become true, or until a user-specified timeout
        // expires and then dispatches the events.
        // Another way would be to use 'wait()' and check if the QueryCondition
        // has been triggered as the "waitsets" example does.
        waitset.dispatch(wait_timeout);
    }
}

int main(int argc, char *argv[])
{
    int domain_id    = 0;
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
        subscriber_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
