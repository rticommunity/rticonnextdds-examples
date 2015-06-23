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

#include "listeners.hpp"
#include <dds/dds.hpp>
#include <rti/core/ListenerBinder.hpp>

using namespace dds::core;
using namespace dds::domain;
using namespace dds::pub;
using namespace dds::topic;

class MyDataWriterListener : public NoOpDataWriterListener<listeners> {
public:
    virtual void on_offered_deadline_missed(
        dds::pub::DataWriter<listeners>& writer,
        const dds::core::status::OfferedDeadlineMissedStatus& status)
    {
        std::cout << "DataWriterListener: on_offered_deadline_missed()"
                  << std::endl;
    }

    virtual void on_liveliness_lost(
        dds::pub::DataWriter<listeners>& writer,
        const dds::core::status::LivelinessLostStatus& status)
    {
        std::cout << "DataWriterListener: on_liveliness_lost()"
                  << std::endl;
    }

    virtual void on_offered_incompatible_qos(
        dds::pub::DataWriter<listeners>& writer,
        const dds::core::status::OfferedIncompatibleQosStatus& status)
    {
        std::cout << "DataWriterListener: on_offered_incompatible_qos()"
                  << std::endl;
    }

    virtual void on_publication_matched(
        dds::pub::DataWriter<listeners>& writer,
        const dds::core::status::PublicationMatchedStatus& status)
    {
        std::cout << "DataWriterListener: on_publication_matched()"
                  << std::endl;
        if (status.current_count_change() < 0) {
            std::cout << "lost a subscription" << std::endl;
        } else {
            std::cout << "found a subscription" << std::endl;
        }
    }

    virtual void on_reliable_writer_cache_changed(
        dds::pub::DataWriter<listeners>& writer,
        const rti::core::status::ReliableWriterCacheChangedStatus& status)
    {
        std::cout << "DataWriterListener: on_reliable_writer_cache_changed()"
                  << std::endl;
    }

    virtual void on_reliable_reader_activity_changed(
        dds::pub::DataWriter<listeners>& writer,
        const rti::core::status::ReliableReaderActivityChangedStatus& status)
    {
        std::cout << "DataWriterListener: on_reliable_reader_activity_changed()"
                  << std::endl;
    }
};

void publisher_main(int domain_id, int sample_count)
{
    // Create the participant.
    // To customize QoS, use the configuration file USER_QOS_PROFILES.xml
    DomainParticipant participant (domain_id);

    // Create the publisher
    // To customize QoS, use the configuration file USER_QOS_PROFILES.xml
    Publisher publisher (participant);

    // Create ande Delete Inconsistent Topic
    // ---------------------------------------------------------------
    // Here we create an inconsistent topic to trigger the subscriber
    // application's callback.
    // The inconsistent topic is created with the topic name used in
    // the Subscriber application, but with a different data type --
    // the msg data type defined in listener.idl.
    // Once it is created, we sleep to ensure the applications discover
    // each other and delete the Data Writer and Topic.
    std::cout << "Creating Inconsistent Topic..." << std::endl;
    Topic<msg> inconsistent_topic (participant, "Example listeners");

    // We have to associate a writer to the topic, as Topic information is not
    // actually propagated until the creation of an associated writer.
    DataWriter<msg> inconsistent_writer (publisher, inconsistent_topic);

    // Sleep to leave time for applications to discover each other.
    rti::util::sleep(Duration(2));

    inconsistent_writer.close();
    inconsistent_topic.close();
    std::cout << "... Deleted Incosistent Topic" << std::endl << std::endl;

    // Create Consistent Topic
    // -----------------------------------------------------------------
    // Once we have created the inconsistent topic with the wrong type,
    // we create a topic with the right type name -- listeners -- that we
    // will use to publish data.
    Topic<listeners> topic (participant, "Example listeners");

    // We will use the Data Writer Listener defined above to print
    // a message when some of events are triggered in the DataWriter.
    // By using ListenerBinder (a RAII) it will take care of setting the
    // listener to NULL on destruction.
    DataWriter<listeners> writer (publisher, topic);
    rti::core::ListenerBinder< DataWriter<listeners> > writer_listener =
        rti::core::bind_and_manage_listener(
            writer,
            new MyDataWriterListener,
            dds::core::status::StatusMask::all());

    // Create data sample for writing
    listeners instance;

    // Main loop
    for (int count = 0; (sample_count == 0) || (count < sample_count); ++count){
        std::cout << "Writing listeners, count " << count << std::endl;

        // Modify data and send it.
        instance.x(count);
        writer.write(instance);

        rti::util::sleep(Duration(2));
    }
}

 int main(int argc, char* argv[])
 {
     int domain_id = 0;
     int sample_count = 0;   // Infinite loop

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
     } catch (std::exception ex) {
         std::cout << "Exception caught: " << ex.what() << std::endl;
         return -1;
     }

     return 0;
 }
