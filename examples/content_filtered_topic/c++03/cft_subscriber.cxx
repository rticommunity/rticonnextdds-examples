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

#include "cft.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::core::status;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

class CftListener : public NoOpDataReaderListener<cft> {
public:
    void on_data_available(DataReader<cft> &reader)
    {
        // Take the available data
        LoanedSamples<cft> samples = reader.take();

        // Print samples by copying to std::cout
        for (LoanedSamples<cft>::iterator sampleIt = samples.begin();
            sampleIt != samples.end();
            ++sampleIt) {

            if (sampleIt->info().valid()) {
                std::cout << sampleIt->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count, bool is_cft)
{
    // To customize any of the entities QoS, use
    // the configuration file USER_QOS_PROFILES.xml

    DomainParticipant participant (domain_id);

    Topic<cft> topic (participant, "Example cft");

    // Sequence of parameters for the content filter expression
    std::vector<std::string> parameters(2);

    // The default parameter list that we will include in the
    // sequence of parameters will be "1", "4" (i.e., 1 <= x <= 4).
    parameters[0] = "1";
    parameters[1] = "4";
    if (is_cft) {
        std::cout << std::endl
                  << "==========================" << std::endl
                  << "Using CFT"                  << std::endl
                  << "Filter: 1 <= x <= 4"        << std::endl
                  << "==========================" << std::endl;
    }

    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to set the reliability and history QoS settings
    // programmatically rather than using the XML, you will need to comment out
    // the following lines of code.
    // reader_qos << Reliability::Reliable()
    //            << Durability::TransientLocal()
    //            << History::KeepLast(20);

    // Create the content filtered topic in the case sel_cft is true
    // The Content Filter Expresion has two parameters:
    // - %0 -- x must be greater or equal than %0.
    // - %1 -- x must be less or equal than %1.
    ContentFilteredTopic<cft> cft_topic = dds::core::null;
    DataReader<cft> reader = dds::core::null;

    if (is_cft) {
        std::cout << "Using ContentFiltered Topic" << std::endl;
        cft_topic = ContentFilteredTopic<cft>(
            topic,
            "ContentFilteredTopic",
            Filter("x >= %0 AND x <= %1", parameters));
        reader = DataReader<cft>(
            Subscriber(participant),
            cft_topic,
            reader_qos);
    } else {
        std::cout << "Using Normal Topic" << std::endl;
        reader = DataReader<cft>(
            Subscriber(participant),
            topic,
            reader_qos);
    }

    // Create a data reader listener using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    rti::core::ListenerBinder< DataReader<cft> > scoped_listener =
        rti::core::bind_and_manage_listener(
            reader,
            new CftListener,
            StatusMask::data_available());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        // Receive period of 1 second.
        rti::util::sleep(Duration(1));

        // If we are not using CFT, we do not need to change the filter
        // parameters
        if (!is_cft) {
            continue;
        }

        if (count == 10) {
            std::cout << std::endl
                      << "==========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Filter: 5 <= x <= 9"        << std::endl
                      << "==========================" << std::endl;

            parameters[0] = "5";
            parameters[1] = "9";
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
        } else if (count == 20) {
            std::cout << std::endl
                      << "==========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Filter: 3 <= x <= 9"        << std::endl
                      << "==========================" << std::endl;

            parameters[0] = "3";
            cft_topic.filter_parameters(parameters.begin(), parameters.end());
        }
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop
    bool is_cft = true;   // Use content filtered topic?

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    if (argc >= 4) {
        is_cft = (argv[3][0] == '1');
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriber_main(domain_id, sample_count, is_cft);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriber_main(): " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
