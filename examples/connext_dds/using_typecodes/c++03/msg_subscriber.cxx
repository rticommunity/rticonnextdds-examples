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
#include <iomanip>

#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>
#include "msg.hpp"

using namespace dds::core;
using namespace rti::core::policy;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::sub;

class BuiltinPublicationListener :
    public NoOpDataReaderListener<PublicationBuiltinTopicData> {
  public:
    void on_data_available(DataReader<PublicationBuiltinTopicData>& reader)
    {
        // Take all samples
        LoanedSamples<PublicationBuiltinTopicData> samples = reader.take();

        typedef LoanedSamples<PublicationBuiltinTopicData>::iterator SampleIter;
        for (SampleIter sample_it = samples.begin();
            sample_it != samples.end();
            sample_it++) {

            if (sample_it->info().valid()){
                const PublicationBuiltinTopicData& data = sample_it->data();
                const BuiltinTopicKey& partKey = data.participant_key();
                const BuiltinTopicKey& key = sample_it->data().key();

                std::cout << std::hex << std::setw(8) << std::setfill('0');
                std::cout << "-----" << std::endl
                          << "Found topic \"" << data.topic_name() << "\""
                          << std::endl
                          << "participant: "    << partKey.value()[0]
                          << partKey.value()[1] << partKey.value()[2]
                          << std::endl
                          << "datawriter:  " << key.value()[0]
                          << key.value()[1]  << key.value()[2] << std::endl
                          << "type:" << std::endl;

                if (!data->type().is_set()) {
                    std::cout << "No type received, perhaps increase "
                              << "type_code_max_serialized_length?"
                              << std::endl;
                } else {
                    // Using the type propagated we print the data type
                    // with print_idl()
                    rti::core::xtypes::print_idl(data->type().get(), 2);
                }
            }

        }
    }
};

void subscriber_main(int domain_id, int sample_count)
{
    // Retrieve the Participant QoS, from USER_QOS_PROFILES.xml
    DomainParticipantQos participant_qos = QosProvider::Default()->
        participant_qos();

    // If you want to change the Participant's QoS programmatically rather than
    // using the XML file, uncomment the following lines.

    // participant_qos << DomainParticipantResourceLimits().
    //     type_code_max_serialized_length(3070);

    // Create a DomainParticipant.
    DomainParticipant participant(domain_id, participant_qos);

    // First get the builtin subscriber.
    Subscriber builtin_subscriber = dds::sub::builtin_subscriber(participant);

    // Then get builtin subscriber's DataReader for DataWriters.
    DataReader<PublicationBuiltinTopicData> publication_reader =
        rti::sub::find_datareader_by_topic_name< DataReader<PublicationBuiltinTopicData> >(
            builtin_subscriber, 
            dds::topic::publication_topic_name());

    // Install our listener using ListenerBinder, a RAII that will take care
    // of setting it to NULL and deleting it.
    rti::core::ListenerBinder< DataReader<PublicationBuiltinTopicData> >
        publication_listener = rti::core::bind_and_manage_listener(
            publication_reader,
            new BuiltinPublicationListener,
            dds::core::status::StatusMask::data_available());

    for (int count = 0; sample_count == 0 || count < sample_count; ++count) {
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
