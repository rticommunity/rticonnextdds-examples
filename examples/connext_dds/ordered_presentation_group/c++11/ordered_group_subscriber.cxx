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

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>  // for logging

#include "ordered_group.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

class ordered_groupSubscriberListener
        : public dds::sub::NoOpSubscriberListener {
public:
    void on_data_on_readers(dds::sub::Subscriber &subscriber)
    {
        // Create a coherent group access.
        dds::sub::CoherentAccess coherent_access(subscriber);

        // Get the sequence of DataReaders that specifies the order
        // in wich each sample should be read.
        std::vector<dds::sub::AnyDataReader> readers;
        int num_readers =
                find(subscriber,
                     dds::sub::status::DataState::new_data(),
                     std::back_inserter(readers));
        std::cout << num_readers << std::endl;

        for (int i = 0; i < num_readers; i++) {
            dds::sub::DataReader<ordered_group> reader =
                    readers[i].get<ordered_group>();

            // We need to take only one sample each time, as we want to follow
            // the sequence of DataReaders. This way the samples will be
            // returned in the order in which they were modified.
            dds::sub::LoanedSamples<ordered_group> sample =
                    reader.select().max_samples(1).take();

            if (sample.length() > 0 && sample[0].info().valid()) {
                std::cout << sample[0].data() << std::endl;
            }
        }

        // The destructor will ends the coherent group access
    }
};

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Retrieve the default Subscriber QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::SubscriberQos subscriber_qos =
            dds::core::QosProvider::Default().subscriber_qos();

    // If you want to change the Subscriber's QoS programmatically rather than
    // using the XML file, uncomment the following line.

    // subscriber_qos << Presentation::GroupAccessScope(false, true);

    // Create a shared pointer for the Subscriber Listener
    auto subscriber_listener =
            std::make_shared<ordered_groupSubscriberListener>();

    // Create a Subscriber.
    dds::sub::Subscriber subscriber(
            participant,
            subscriber_qos,
            subscriber_listener);

    // Create three Topic, once for each DataReader.
    dds::topic::Topic<ordered_group> topic1(participant, "Topic1");
    dds::topic::Topic<ordered_group> topic2(participant, "Topic2");
    dds::topic::Topic<ordered_group> topic3(participant, "Topic3");

    // Create one DataReader for each Topic.
    dds::sub::DataReader<ordered_group> reader1(subscriber, topic1);
    dds::sub::DataReader<ordered_group> reader2(subscriber, topic2);
    dds::sub::DataReader<ordered_group> reader3(subscriber, topic3);

    for (unsigned int samples_read = 0;
         !application::shutdown_requested && samples_read < sample_count;
         samples_read++) {
        std::cout << "ordered_group subscriber sleeping for 1 sec..."
                  << std::endl;
        rti::util::sleep(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
    auto arguments = parse_arguments(argc, argv, ApplicationKind::subscriber);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_subscriber_application(arguments.domain_id, arguments.sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory.  Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
