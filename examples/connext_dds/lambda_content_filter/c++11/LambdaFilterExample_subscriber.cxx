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

#include <algorithm>
#include <iostream>

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
// Or simply include <dds/dds.hpp>

#include "LambdaFilterExample.hpp"

#include "LambdaFilter.hpp"

void subscriber_main(int domain_id, int sample_count)
{
    dds::domain::DomainParticipant participant(domain_id);

    dds::topic::Topic<Stock> topic(participant, "Example Stock");

    // Create a ContentFilteredTopic that only selects those Stocks whose
    // symbol is GOOG or IBM.
    auto lambda_cft = create_lambda_cft<Stock>(
        "stock_cft",
        topic,
        [](const Stock& stock)
        {
            return stock.symbol() == "GOOG" || stock.symbol() == "IBM";
        }
    );

    dds::sub::DataReader<Stock> reader(
        dds::sub::Subscriber(participant), lambda_cft);

    // Create a ReadCondition for any data on this reader and associate a handler
    int count = 0;
    dds::sub::cond::ReadCondition read_condition(
        reader,
        dds::sub::status::DataState::any(),
        [&reader, &count]()
        {
            // Take all samples
            dds::sub::LoanedSamples<Stock> samples = reader.take();
            for (auto sample : samples){
                if (sample.info().valid()){
                    count++;
                    std::cout << sample.data() << std::endl;
                }
            }

        } // The LoanedSamples destructor returns the loan
    );

    // Create a WaitSet and attach the ReadCondition
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    while (count < sample_count || sample_count == 0) {
        // Dispatch will call the handlers associated to the WaitSet conditions
        // when they activate
        std::cout << "Stock subscriber sleeping for 4 sec...\n";
        waitset.dispatch(dds::core::Duration(4)); // Wait up to 4s each time
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
        subscriber_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << "\n";
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
