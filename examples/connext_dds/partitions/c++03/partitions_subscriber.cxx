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

#include "partitions.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::core::status;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;
using namespace dds::sub::status;
using namespace rti::core;

class PartitionsListener : public NoOpDataReaderListener<partitions> {
public:
    void on_data_available(DataReader<partitions>& reader)
    {
        // Take all samples
        LoanedSamples<partitions> samples = reader.take();

        for (LoanedSamples<partitions>::iterator sample_it = samples.begin();
            sample_it != samples.end(); sample_it++) {

            if (sample_it->info().valid()) {
                // After partition mismatch unpair,
                // it detects the instance as new.
                ViewState view_state = sample_it->info().state().view_state();
                if (view_state == ViewState::new_view()) {
                    std::cout << "Found new instance" << std::endl;
                }

                std::cout << sample_it->data() << std::endl;
            }
        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a participant with default QoS.
    DomainParticipant participant(domain_id);

    // Retrieve the default Subscriber QoS, from USER_QOS_PROFILES.xml
    SubscriberQos subscriber_qos = QosProvider::Default().subscriber_qos();

    Partition partition = subscriber_qos.policy<Partition>();
    std::vector<std::string> partition_names = partition.name();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // partition_names[0] = "ABC";
    // partition_names[1] = "X*Z";
    // partition.name(partition_names);
    // subscriber_qos << partition;

    std::cout << "Setting partition to "
              << "'" << partition_names[0] << "', "
              << "'" << partition_names[1] << "'..." << std::endl;

    // Create a subscriber.
    Subscriber subscriber(participant, subscriber_qos);

    // Create a Topic -- and automatically register the type.
    Topic<partitions> topic(participant, "Example partitions");

    // Retrieve the default DataReader QoS, from USER_QOS_PROFILES.xml
    DataReaderQos reader_qos = QosProvider::Default().datareader_qos();

    // If you want to change the Subscriber QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // reader_qos << Reliability::Reliable()
    //            << History::KeepAll()
    //            << Durability::TransientLocal();

    // Create a DataReader.
    DataReader<partitions> reader(subscriber, topic, reader_qos);

    // Create a DataReader listener using ListenerBinder, a RAII utility that
    // will take care of reseting it from the reader and deleting it.
    ListenerBinder<DataReader<partitions> > scoped_listener =
        bind_and_manage_listener(
            reader,
            new PartitionsListener,
            StatusMask::data_available());

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        rti::util::sleep(Duration(4));
    }
}

int main(int argc, char* argv[])
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
        std::cerr << "Exception in subscriber_main(): " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
