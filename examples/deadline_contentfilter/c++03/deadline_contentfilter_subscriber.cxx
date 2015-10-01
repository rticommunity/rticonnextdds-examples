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
#include <cstdlib>
#include <iostream>
#include <ctime>

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "deadline_contentfilter.hpp"

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;

clock_t init_time;

class deadline_contentfilterReaderListener :
    public NoOpDataReaderListener<deadline_contentfilter> {
  public:
    void on_data_available(DataReader<deadline_contentfilter>& reader)
    {
        // Take all samples
        LoanedSamples<deadline_contentfilter> samples = reader.take();

        for (LoanedSamples<deadline_contentfilter>::iterator sample_it =
                samples.begin();
            sample_it != samples.end();
            sample_it++) {

            if (sample_it->info().valid()){
                // Print the time we get each sample.
                double elapsed_ticks = clock() - init_time;
                double elapsed_secs = elapsed_ticks / CLOCKS_PER_SEC;

                const deadline_contentfilter& data = sample_it->data();
                std::cout << "@ t=" << elapsed_secs << "s, Instance"
                          << data.code() << ": <" << data.x()
                          << "," << data.y() << ">" << std::endl;
            }
        }
    }

    void on_requested_deadline_missed(
        DataReader<deadline_contentfilter>& reader,
        const RequestedDeadlineMissedStatus& status)
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

void subscriber_main(int domain_id, int sample_count)
{
    // For timekeeping
    init_time = clock();

    // Create a DomainParticipant with default Qos
    DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    Topic<deadline_contentfilter> topic(
        participant, "Example deadline_contentfilter");

    // Set up a Content Filtered Topic to show interaction with deadline.
    std::vector<std::string> parameters(1);
    parameters[0] = "2";

    ContentFilteredTopic<deadline_contentfilter> cft_topic(
        topic,
        "ContentFilteredTopic",
        Filter("code < %0", parameters));

    // Retrieve the default DataReader's QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the DataReader's QoS programmatically rather than
    // using the XML file, uncomment the following lines.
    //
    // In this case, we set the deadline period to 2 seconds to trigger
    // a deadline if the DataWriter does not update often enough, or if
    // the content-filter filters out data so there is no data available
    // with 2 seconds.

    // reader_qos << Deadline(Duration(2));

    // Create a DataReader (Subscriber created in-line)
    DataReader<deadline_contentfilter> reader(
        Subscriber(participant), cft_topic, reader_qos);

    // Create a data reader listener using ListenerBinder, a RAII that
    // will take care of setting it to NULL on destruction.
    rti::core::ListenerBinder<DataReader<deadline_contentfilter> > listener =
        rti::core::bind_and_manage_listener(
            reader,
            new deadline_contentfilterReaderListener,
            StatusMask::all());

    std::cout << std::fixed;
    for (short count=0; (sample_count == 0) || (count < sample_count); ++count){
        // After 10 seconds, change filter to accept only instance 0.
        if (count == 10) {
            std::cout << "Starting to filter out instance1" << std::endl;
            parameters[0] = "1";
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
