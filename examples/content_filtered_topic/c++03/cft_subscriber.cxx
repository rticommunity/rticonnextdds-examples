/*******************************************************************************
 (c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/
#include "cft.hpp"
#include <dds/sub/ddssub.hpp>

using namespace dds::core;
using namespace dds::core::status;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

class cftListener : public NoOpDataReaderListener<cft> {
public:
    virtual void on_data_available(DataReader<cft> &reader);
};

void cftListener::on_data_available(DataReader<cft> &reader)
{
    // Take the available data
    LoanedSamples<cft> samples = reader.take();

    // Print samples by copying to std::cout
    std::copy(
        samples.begin(),
        samples.end(),
        LoanedSamples<cft>::ostream_iterator(std::cout, "\n"));
}

void subscriber_main(int domainId, int sample_count, int sel_cft)
{
    // To customize any of the entities QoS, use
    // the configuration file USER_QOS_PROFILES.xml

    DomainParticipant participant (domainId);

    Topic<cft> topic (participant, "Example cft");

    // Sequence of parameters for the content filter expression
    std::vector<std::string> parameters(2);
    // The default parameter list that we will include in the
    // sequence of parameters will be "1", "4" (i.e., 1 <= x <= 4).
    parameters[0] = "1";
    parameters[1] = "4";

    // Create a data reader listener
    cftListener reader_listener;

    // Create the content filtered topic in the case sel_cft is true
    // The Content Filter Expresion has two parameters:
    // - %0 -- x must be greater or equal than %0.
    // - %1 -- x must be less or equal than %1.
    ContentFilteredTopic<cft>* cft_topic;
    DataReader<cft>* reader;

    if (sel_cft) {
        std::cout << "Using ContentFiltered Topic" << std::endl;
        cft_topic = new ContentFilteredTopic<cft>(
            topic,
            "ContentFilteredTopic",
            Filter("x >= %0 AND x <= %1", parameters));
        
        reader = new DataReader<cft>(
            Subscriber(participant),
            *cft_topic,
            DataReaderQos(),
            &reader_listener,
            dds::core::status::StatusMask::data_available());
    } else {
        std::cout << "Using Normal Topic" << std::endl;

        reader = new DataReader<cft>(
            Subscriber(participant),
            topic,
            DataReaderQos(),
            &reader_listener,
            dds::core::status::StatusMask::data_available());
    }

    // If you want to set the reliability and history QoS settings
    // programmatically rather than using the XML, you will need to add
    // the following lines to your code and comment out the datareader
    // constructor call above.
    
    /*
    DataReaderQos datareader_qos;
    datareader_qos << Reliability::Reliable();
    datareader_qos << Durability::TransientLocal();
    datareader_qos << History::KeepLast(20);
    
    if (sel_cft) {
        std::cout << "Using ContentFiltered Topic" << std::endl;
        cft_topic = new ContentFilteredTopic<cft>(
            topic,
            "ContentFilteredTopic",
            Filter("(x >= %0 and x <= %1)", parameters));

        reader = new DataReader<cft>(
            Subscriber(participant),
            *cft_topic,
            datareader_qos,
            &reader_listener,
            dds::core::status::StatusMask::data_available());
    } else {
        std::cout << "Using Normal Topic" << std::endl;

        reader = new DataReader<cft>(
            Subscriber(participant),
            topic,
            datareader_qos,
            &reader_listener,
            dds::core::status::StatusMask::data_available());
    }
    */

    if (sel_cft) {
        std::cout << std::endl
                  << "==========================" << std::endl
                  << "Using CFT"                  << std::endl
                  << "Filter: 1 <= x <= 4"        << std::endl
                  << "==========================" << std::endl;
    }

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        // Receive period of 1 second.
        rti::util::sleep(Duration(1));

        // If we are not using CFT, we do not need to change the filter
        // parameters
        if (!sel_cft) {
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
            cft_topic->filter_parameters(parameters.begin(), parameters.end());
        } else if (count == 20) {
            std::cout << std::endl
                      << "==========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Filter: 3 <= x <= 9"        << std::endl
                      << "==========================" << std::endl;

            parameters[0] = "3";
            cft_topic->filter_parameters(parameters.begin(), parameters.end());
        }
    }
}

void main(int argc, char *argv[])
{
    int domainId = 0;
    int sample_count = 0; // Infinite loop
    int sel_cft = 1;      // Use content filtered topic?

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }

    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    if (argc >= 4) {
        sel_cft = atoi(argv[3]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    return subscriber_main(domainId, sample_count, sel_cft);
}