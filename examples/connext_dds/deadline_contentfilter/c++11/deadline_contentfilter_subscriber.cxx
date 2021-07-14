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

#include "deadline_contentfilter.hpp"
#include "application.hpp"  // for command line parsing and ctrl-c

clock_t init_time;

class deadline_contentfilterReaderListener
        : public dds::sub::NoOpDataReaderListener<deadline_contentfilter> {
public:
    void on_data_available(dds::sub::DataReader<deadline_contentfilter> &reader)
    {
        // Take all samples
        dds::sub::LoanedSamples<deadline_contentfilter> samples = reader.take();

        for (const auto &sample : samples) {
            if (sample.info().valid()) {
                // Print the time we get each sample.
                double elapsed_ticks = clock() - init_time;
                double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

                const deadline_contentfilter &data = sample.data();
                std::cout << "@ t=" << elapsed_secs << "s, Instance"
                          << data.code() << ": <" << data.x() << "," << data.y()
                          << ">" << std::endl;
            }
        }
    }

    void on_requested_deadline_missed(
            dds::sub::DataReader<deadline_contentfilter> &reader,
            const dds::core::status::RequestedDeadlineMissedStatus &status)
    {
        double elapsed_ticks = clock() - init_time;
        double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

        // Creates a temporary object in order to find out which instance
        // missed its deadline period. The 'key_value' call only fills in the
        // values of the key fields inside the object.
        deadline_contentfilter affected_sample;
        reader.key_value(affected_sample, status.last_instance_handle());

        // Print out which instance missed its deadline.
        std::cout << "Missed deadline @ t=" << elapsed_secs
                  << "s on instance code = " << affected_sample.code()
                  << std::endl;
    }
};

void run_subscriber_application(
        unsigned int domain_id,
        unsigned int sample_count)
{
    // For timekeeping
    init_time = clock();

    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<deadline_contentfilter> topic(
            participant,
            "Example deadline_contentfilter");

    // Set up a Content Filtered Topic to show interaction with deadline.
    dds::topic::ContentFilteredTopic<deadline_contentfilter> cft_topic(
            topic,
            "ContentFilteredTopic",
            dds::topic::Filter("code < %0", { "2" }));

    // Retrieve the default DataReader's QoS, from USER_QOS_PROFILES.xml
    dds::sub::qos::DataReaderQos reader_qos =
            dds::core::QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    //
    // In this case, we set the deadline period to 2 seconds to trigger
    // a deadline if the DataWriter does not update often enough, or if
    // the content-filter filters out data so there is no data available
    // with 2 seconds.

    // reader_qos << Deadline(Duration(2));

    // Create a subscriber
    dds::sub::Subscriber subscriber(participant);

    // Create a shared pointer for deadline_contentfilterReaderListener class
    auto deadline_listener =
            std::make_shared<deadline_contentfilterReaderListener>();

    // Create a DataReader
    dds::sub::DataReader<deadline_contentfilter> reader(
            subscriber,
            cft_topic,
            reader_qos);

    // Set the created listened for the DataReader
    reader.set_listener(deadline_listener);

    std::cout << std::fixed;
    for (unsigned int samples_read = 0;
         !application::shutdown_requested && samples_read < sample_count;
         samples_read++) {
        // After 10 seconds, change filter to accept only instance 0.
        if (samples_read == 10) {
            std::cout << "Starting to filter out instance1" << std::endl;
            std::vector<std::string> parameter { "1" };
            cft_topic.filter_parameters(parameter.begin(), parameter.end());
        }

        rti::util::sleep(dds::core::Duration(1));
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
