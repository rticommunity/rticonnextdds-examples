/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <algorithm>
#include <iostream>

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
// Or simply include <dds/dds.hpp>

#include "HelloMsg.hpp"

class HelloMsgReaderListener : public dds::sub::NoOpDataReaderListener<HelloMsg> {
  public:

    HelloMsgReaderListener() : count_ (0)
    {
    }

    void on_data_available(dds::sub::DataReader<HelloMsg>& reader)
    {
        // Take all samples
        dds::sub::LoanedSamples<HelloMsg> samples = reader.take();

        for ( dds::sub::LoanedSamples<HelloMsg>::iterator sample_it = samples.begin();
        sample_it != samples.end(); sample_it++) {

            if (sample_it->info().valid()){
                count_++;
                std::cout << sample_it->data() << std::endl;
            }
        }
    }

    int count() const
    {
        return count_;
    }

  private:
    int count_;
};

void subscriber_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<HelloMsg> topic(participant, "Example_Cpp_Storage");

    // Create a DataReader with default Qos (Subscriber created in-line)
    HelloMsgReaderListener listener;
    dds::sub::DataReader<HelloMsg> reader(
        dds::sub::Subscriber(participant),
        topic,
        dds::core::QosProvider::Default().datareader_qos(),
        &listener,
        dds::core::status::StatusMask::data_available());

    while (listener.count() < sample_count || sample_count == 0) {
        std::cout << "HelloMsg subscriber sleeping for 4 sec..." << std::endl;

        rti::util::sleep(dds::core::Duration(4));
    }

    // Unset the listener to allow the reader destruction
    // (rti::core::ListenerBinder can do this automatically)
    reader.listener(NULL, dds::core::status::StatusMask::none());
}

int main(int argc, char *argv[])
{

    int domain_id = 0;
    int sample_count = 0; // infinite loop

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

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

