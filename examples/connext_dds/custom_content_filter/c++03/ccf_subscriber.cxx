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
#include <rti/core/ListenerBinder.hpp>
#include "ccf.hpp"
#include "filter.hpp"

using namespace dds::core;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;

class CcfListener : public NoOpDataReaderListener<Foo> {
public:
    void on_data_available(DataReader<Foo> &reader)
    {
        // Take the available data
        LoanedSamples<Foo> samples = reader.take();

        for (LoanedSamples<Foo>::iterator sampleIt = samples.begin();
            sampleIt != samples.end();
            ++sampleIt) {

            if (sampleIt->info().valid()) {
                std::cout << sampleIt->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant.
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type.
    Topic<Foo> topic(participant, "Example ccf");

    // Register the custom filter type. It must be registered in both sides.
    participant->register_contentfilter(
        CustomFilter<CustomFilterType>(new CustomFilterType()),
        "CustomFilter");

    // The default filter parameters will filter values that are divisible by 2.
    std::vector<std::string> parameters(2);
    parameters[0] = "2";
    parameters[1] = "divides";

    // Create the filter with the expression and the type registered.
    Filter filter("%0 %1 x", parameters);
    filter->name("CustomFilter");

    // Create the content filtered topic.
    ContentFilteredTopic<Foo> cft_topic(
        topic,
        "ContentFilteredTopic",
        filter);

    std::cout << "Filter: 2 divides x" << std::endl;

    // Create a DataReader. Note that we are using the content filtered topic.
    DataReader<Foo> reader(Subscriber(participant), cft_topic);

    // Create a data reader listener using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    rti::core::ListenerBinder< DataReader<Foo> > scoped_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new CcfListener,
            StatusMask::data_available());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        if (count == 10) {
            std::cout << "Changing filter parameters" << std::endl
                      << "Filter: 15 greater-than x"  << std::endl;
            parameters[0] = "15";
            parameters[1] = "greater-than";
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
        } else if (count == 20) {
            std::cout << "Changing filter parameters" << std::endl
                      << "Filter: 3 divides x"  << std::endl;
            parameters[0] = "3";
            parameters[1] = "divides";
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
        }

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
    // rti::config::Logger::instance()l.verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
