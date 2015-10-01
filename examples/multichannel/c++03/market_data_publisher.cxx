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
#include "market_data.hpp"

using namespace dds::core;
using namespace rti::core;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

MultiChannel create_multichannel_qos()
{
    // We set the publish as multichannel using the different
    // channels to send different symbols. Every channel have a IP to send the
    // data.

    // Create 8 channels based on Symbol.
    std::vector<ChannelSettings> channels;
    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(), // All the transports available
                "239.255.0.2",
                0)),                        // Port determined automatically
        "Symbol MATCH '[A-C]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(),
                "239.255.0.3",
                0)),
        "Symbol MATCH '[D-F]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(),
                "239.255.0.4",
                0)),
        "Symbol MATCH '[G-I]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(),
                "239.255.0.5",
                0)),
        "Symbol MATCH '[J-L]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(),
                "239.255.0.6",
                0)),
        "Symbol MATCH '[M-O]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(),
                "239.255.0.7",
                0)),
        "Symbol MATCH '[P-S]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(),
                "239.255.0.8",
                0)),
        "Symbol MATCH '[T-V]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(ChannelSettings(
        std::vector<TransportMulticastSettings>(
            1,
            TransportMulticastSettings(
                std::vector<std::string>(),
                "239.255.0.9",
                0)),
        "Symbol MATCH '[W-Z]*'",
        PUBLICATION_PRIORITY_UNDEFINED));

    // Set the MultiChannel QoS.
    return MultiChannel(channels, rti::topic::stringmatch_filter_name());
}

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    DomainParticipant participant (domain_id);

    // Create a Topic -- and automatically register the type
    Topic<market_data> topic (participant, "Example market_data");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following line.
    // writer_qos << create_multichannel_qos();

    // Create a DataWriter (Publisher created in-line)
    DataWriter<market_data> writer(Publisher(participant), topic, writer_qos);

    // Create a data sample for writing.
    market_data sample;

    // Main loop.
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        // Update the sample.
        char symbol = (char)('A' + (count % 26));
        sample.Symbol(std::string(1, symbol));
        sample.Price(count);

        // Send and wait.
        writer.write(sample);
        rti::util::sleep(Duration::from_millisecs(100));
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
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
