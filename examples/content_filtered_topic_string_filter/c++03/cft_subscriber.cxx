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
#include <algorithm>
#include <iostream>

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "cft.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;
using namespace rti::core;

class cftReaderListener : public NoOpDataReaderListener<cft> {
  public:
    void on_data_available(dds::sub::DataReader<cft>& reader)
    {
        // Take all samples
        LoanedSamples<cft> samples = reader.take();

        for (LoanedSamples<cft>::iterator sample_it = samples.begin();
        sample_it != samples.end(); sample_it++) {

            if (sample_it->info().valid()) {
                std::cout << sample_it->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count, bool is_cft)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<cft> topic(participant, "Example cft");

    // Define the default parameter of the filter.
    std::vector<std::string> parameters(1);
    parameters[0] = "SOME_STRING";

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // reader_qos << Reliability::Reliable()
    //            << Durability::TransientLocal()
    //            << History::KeepLast(20);

    // Create the ContentFilteredTopic and DataReader.
    ContentFilteredTopic<cft> cft_topic = dds::core::null;
    DataReader<cft> reader = dds::core::null;

    if (is_cft) {
        std::cout << "Using ContentFiltered Topic" << std::endl;
        Filter filter("name MATCH %0", parameters);

        // If there is no filter name, the regular SQL filter will be used.
        filter->name(rti::topic::stringmatch_filter_name());

        cft_topic = ContentFilteredTopic<cft>(
            topic,
            "ContentFilteredTopic",
            filter);

        reader = DataReader<cft>(Subscriber(participant), cft_topic,reader_qos);
    } else {
        std::cout << "Using Normal Topic" << std::endl;
        reader = DataReader<cft>(Subscriber(participant), topic, reader_qos);
    }

    // Create a DataReader listener using ListenerBinder, a RAII utility that
    // will take care of reseting it from the reader and deleting it.
    ListenerBinder< DataReader<cft> > scoped_listener =
        bind_and_manage_listener(
            reader,
            new cftReaderListener,
            StatusMask::data_available());

    // Change the filter
    if (is_cft) {
        std::cout << "Now setting a new filter: 'name MATCH \"EVEN\"'"
                  << std::endl;
        cft_topic->append_to_expression_parameter(0, "EVEN");
    }

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        rti::util::sleep(dds::core::Duration(1));
        if (!is_cft) {
            continue;
        }

        if (count == 10) {
            std::cout << std::endl << "===========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Append 'ODD' filter" << std::endl
                      << "===========================" << std::endl;
            cft_topic->append_to_expression_parameter(0, "ODD");
        } else if (count == 20) {
            std::cout << std::endl << "===========================" << std::endl
                      << "Changing filter parameters" << std::endl
                      << "Removing 'EVEN' filter" << std::endl
                      << "===========================" << std::endl;
            cft_topic->remove_from_expression_parameter(0, "EVEN");
        }
    }
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; // Infinite loop
    bool is_cft = true;

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
        std::cerr << "Exception in subscriber_main(): " << ex.what()
                  << std::endl;
        return -1;
    }

    return 0;
}
