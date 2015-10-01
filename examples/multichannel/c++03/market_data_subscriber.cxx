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

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "market_data.hpp"

using namespace dds::core;
using namespace rti::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::sub;
using namespace dds::topic;

class MarketDataReaderListener : public NoOpDataReaderListener<market_data> {
  public:
    void on_data_available(DataReader<market_data>& reader)
    {
        // Take all samples
        LoanedSamples<market_data> samples = reader.take();

        for (LoanedSamples<market_data>::iterator sample_it = samples.begin();
            sample_it != samples.end();
            sample_it++) {

            if (sample_it->info().valid()){
                std::cout << sample_it->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<market_data> topic(participant, "Example market_data");

    // Create a ContentFiltered Topic and specify the STRINGMATCH filter name
    // to use a built-in filter for matching multiple strings.
    // More information can be found in the example
    // 'content_filtered_topic_string_filter'.
    Filter filter("Symbol MATCH 'A'", std::vector<std::string>());
    filter->name(rti::topic::stringmatch_filter_name());
    std::cout << "filter is " << filter.expression() << std::endl;

    ContentFilteredTopic<market_data> cft_topic(
        topic,
        "ContentFilteredTopic",
        filter);

    // Create a DataReader with default Qos (Subscriber created in-line)
    DataReader<market_data> reader(Subscriber(participant), cft_topic);

    // Create a data reader listener using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    rti::core::ListenerBinder< DataReader<market_data> > scoped_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new MarketDataReaderListener,
            StatusMask::data_available());

    // Main loop.
    for (int count = 0; sample_count == 0 || count < sample_count; count++) {
        if (count == 3) {
            // On t=3 we add the symbol 'D' to the filter parameter
            // to match 'A' and 'D'.
            cft_topic->append_to_expression_parameter(0, "D");
            std::cout << "changed filter to Symbol MATCH 'AD'" << std::endl;
        } else if (count == 6) {
            // On t=6 we remove the symbol 'A' to the filter paramter
            // to match only 'D'.
            cft_topic->remove_from_expression_parameter(0, "A");
            std::cout << "changed filter to Symbol MATCH 'D'" << std::endl;
        }

        rti::util::sleep(Duration(4));
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
        subscriber_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
