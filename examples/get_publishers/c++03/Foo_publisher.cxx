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
#include <cstdlib>

#include <dds/dds.hpp>
#include "Foo.hpp"

using namespace dds::core;
using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::pub::qos;
using namespace rti::core::policy;

void publisher_main(int domain_id)
{
    // Create a DomainParticipant with default QoS.
    DomainParticipant participant(domain_id);

    // Retrieve publisher QoS to set names.
    PublisherQos publisher_qos = QosProvider::Default().publisher_qos();

    // Create the first publisher.
    publisher_qos << EntityName("Foo");
    Publisher publisher1(participant, publisher_qos);
    std::cout << "The first  publisher name is "
              << publisher1.qos().policy<EntityName>().name().get()
              << std::endl;

    // Create the second publisher.
    publisher_qos << EntityName("Bar");
    Publisher publisher2(participant, publisher_qos);
    std::cout << "The second publisher name is "
              << publisher2.qos().policy<EntityName>().name().get()
              << std::endl;

    // Find the publishers.
    std::cout << std::endl << "Calling to find_publishers()..." << std::endl
              << std::endl;
    std::vector<Publisher> publishers;
    rti::pub::find_publishers(participant, std::back_inserter(publishers));

    // Iterate over the publishers and print the name.
    std::cout << "Found " << publishers.size()
              << " publishers on this participant" << std::endl;
    for (std::vector<int>::size_type i = 0; i < publishers.size(); i++) {
        std::cout << "The " << i << " publisher name is "
                  << publishers[i].qos().policy<EntityName>().name().get()
                  << std::endl;
    }
}

 int main(int argc, char* argv[])
 {
     int domain_id = 0;

     if (argc >= 2) {
         domain_id = atoi(argv[1]);
     }

     // To turn on additional logging, include <rti/config/Logger.hpp> and
     // uncomment the following line:
     // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

     try {
         publisher_main(domain_id);
     } catch (std::exception ex) {
         std::cout << "Exception caught: " << ex.what() << std::endl;
         return -1;
     }

     return 0;
 }
