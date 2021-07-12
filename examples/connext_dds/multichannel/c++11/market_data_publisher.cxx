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

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>      // for sleep()
#include <rti/config/Logger.hpp>  // for logging

#include "application.hpp"  // for command line parsing and ctrl-c
#include "market_data.hpp"

rti::core::policy::MultiChannel create_multichannel_qos()
{
    // We set the publish as multichannel using the different
    // channels to send different symbols. Every channel have a IP to send the
    // data.

    // Create 8 channels based on Symbol.
    std::vector<rti::core::ChannelSettings> channels;
    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings(
                    {},  // All the transports available
                    "239.255.0.2",
                    0) },  // Port determined automatically
            "Symbol MATCH '[A-C]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings({}, "239.255.0.3", 0) },
            "Symbol MATCH '[D-F]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings({}, "239.255.0.4", 0) },
            "Symbol MATCH '[G-I]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings({}, "239.255.0.5", 0) },
            "Symbol MATCH '[J-L]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings({}, "239.255.0.6", 0) },
            "Symbol MATCH '[M-O]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings({}, "239.255.0.7", 0) },
            "Symbol MATCH '[P-S]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings({}, "239.255.0.8", 0) },
            "Symbol MATCH '[T-V]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    channels.push_back(rti::core::ChannelSettings(
            { rti::core::TransportMulticastSettings({}, "239.255.0.9", 0) },
            "Symbol MATCH '[W-Z]*'",
            rti::core::PUBLICATION_PRIORITY_UNDEFINED));

    // Set the MultiChannel QoS.
    return rti::core::policy::MultiChannel(
            channels,
            rti::topic::stringmatch_filter_name());
}

void run_publisher_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<market_data> topic(participant, "Example market_data");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    dds::pub::qos::DataWriterQos writer_qos =
            dds::core::QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter's QoS programmatically rather than
    // using the XML file, uncomment the following line.
    // writer_qos << create_multichannel_qos();

    // Create a publisher
    dds::pub::Publisher publisher(participant);

    // Create a DataWriter
    dds::pub::DataWriter<market_data> writer(publisher, topic, writer_qos);

    // Create a data sample for writing.
    market_data sample;

    // Main loop.
    for (unsigned int samples_written = 0;
         !application::shutdown_requested && samples_written < sample_count;
         samples_written++) {
        // Update the sample.
        char symbol = (char) ('A' + (samples_written % 26));
        sample.Symbol(std::string(1, symbol));
        sample.Price(samples_written);

        // Send and wait.
        writer.write(sample);
        rti::util::sleep(dds::core::Duration::from_millisecs(100));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_publisher_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
