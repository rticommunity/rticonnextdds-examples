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

#include "partitions.hpp"
#include <dds/dds.hpp>

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;

void publisher_main(int domain_id, int sample_count)
{
    // Create a DomainParticipant with default Qos.
    DomainParticipant participant(domain_id);

    // Retrieve the default Publisher QoS, from USER_QOS_PROFILES.xml
    PublisherQos publisher_qos = QosProvider::Default().publisher_qos();
    Partition partition = publisher_qos.policy<Partition>();
    std::vector<std::string> partition_names = partition.name();

    // If you want to change the Publisher QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // partition_names[0] = "ABC";
    // partition_names[1] = "foo";
    // partition.name(partition_names);
    // publisher_qos << partition;

    std::cout << "Setting partition to";
    for (int i = 0; i < partition_names.size(); i++) {
        std::cout << " '" << partition_names[i] << "'";
    }
    std::cout << std::endl;

    // Create a Publisher.
    Publisher publisher(participant, publisher_qos);

    // Create a Topic -- and automatically register the type.
    Topic<partitions> topic(participant, "Example partitions");

    // Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    DataWriterQos writer_qos = QosProvider::Default().datawriter_qos();

    // If you want to change the DataWriter QoS programmatically rather
    // than using the XML file, you will need to comment out these lines.

    // writer_qos << Reliability::Reliable()
    //            << History::KeepLast(3)
    //            << Durability::TransientLocal();

    // Create a Datawriter.
    DataWriter<partitions> writer(publisher, topic, writer_qos);

    // Create a data sample for writing.
    partitions instance;

    // Main loop
    bool update_qos = false;
    for (int count = 0; (sample_count == 0) || (count < sample_count); count++){
        std::cout << "Writing partitions, count " << count << std::endl;

        // Modify and send the sample.
        instance.x(count);
        writer.write(instance);

        // Every 5 samples we will change the partition name.
        // These are the partition expressions we are going to try:
        // "bar", "A*", "A?C", "X*Z", "zzz" and "A*C".
        if ((count + 1) % 25 == 0) {
            // Matches "ABC", name[1] here can match name[0] there,
            // as long as there is some overlapping name.
            partition_names.resize(2);
            partition_names[0] = "zzz";
            partition_names[1] = "A*C";
            update_qos = true;
        } else if ((count + 1) % 25 == 20) {
            // Strings that are regular expressions aren't tested for
            // literal matches, so this won't match "X*Z".
            partition_names[0] = "X*Z";
            update_qos = true;
        } else if ((count + 1) % 25 == 15) {
            // Matches "ABC".
            partition_names[0] = "A?C";
            update_qos = true;
        } else if ((count + 1) % 25 == 10) {
            // Matches "ABC".
            partition_names[0] = "A*";
            update_qos = true;
        } else if ((count + 1) % 25 == 5) {
            // No literal match for "bar".
            // For the next iterations we are using only one partition.
            partition_names.resize(1);
            partition_names[0] = "bar";
            update_qos = true;
        }

        // Set the new partition names to the publisher QoS.
        if (update_qos) {
            std::cout << "Setting partition to";
            for (int i = 0; i < partition_names.size(); i++) {
                std::cout << " '" << partition_names[i] << "'";
            }
            std::cout << std::endl;

            partition.name(partition_names);
            publisher.qos(publisher_qos << partition);
            update_qos = false;
        }

        rti::util::sleep(Duration(1));
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
        publisher_main(domain_id, sample_count);
    } catch (std::exception ex) {
        std::cout << "Exception caught: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
