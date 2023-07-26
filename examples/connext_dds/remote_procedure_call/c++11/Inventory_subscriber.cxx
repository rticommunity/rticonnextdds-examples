/*
* (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

/* Inventory_service.cxx

*/

#include <iostream>
#include <mutex>
#include <map>
#include <rti/config/Logger.hpp>

#include "application.hpp"

#include "Inventory.hpp"


class InventoryImpl : public InventoryService {
public:
    InventoryImpl()
    {
        inventory["apples"] = 100;
        inventory["oranges"] = 50;
    }

    ::InventoryContents get_inventory() override
    {
        ::InventoryContents contents;

        std::lock_guard<std::mutex> lock(mutex);
        for (const auto &item : inventory) {
            contents.items().push_back(::Item(item.first, item.second));
        }

        return contents;
    }

    void add_item(const ::Item &item) override
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (inventory.find(item.name()) == inventory.end()) {
            inventory[item.name()] = item.quantity();
        } else {
            inventory[item.name()] += item.quantity();
        }
    }

    void remove_item(const ::Item &item) override
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (inventory.find(item.name()) == inventory.end()) {
            throw UnknownItemError(item.name());
        }

        inventory[item.name()] -= item.quantity();
        if (inventory[item.name()] <= 0) {
            inventory.erase(item.name());
        }
    }
private:
    std::map<std::string, int> inventory;

    // The mutex protects the map from concurrent access, since the server
    // can process concurrent remote calls.
    std::mutex mutex;
};

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments
    auto arguments = parse_arguments(argc, argv, false);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    // Create a DomainParticipant with default Qos. The Service will communicate
    // only with Clients that join the same domain_id
    dds::domain::DomainParticipant participant(arguments.domain_id);

    // Create an instance of the service interface
    auto service_impl = std::make_shared< InventoryImpl>();

    // A server provides the execution environment (a thread pool) for one or
    // more services
    dds::rpc::ServerParams server_params;

    // Use a thread_pool_size > 1 to dispatch incoming function calls in
    // parallel
    server_params.extensions().thread_pool_size(4);

    // Create the server with the server_params configuration
    dds::rpc::Server server(server_params);

    // Create a service with an instance of the service interface, and attach
    // it to a server. The service will wait for remote calls, call the
    // service interface to compute the results, and send them back.
    dds::rpc::ServiceParams params(participant);
    params.service_name("Inventory");
    ::InventoryServiceService service(service_impl, server, params);

    std::cout << "InventoryService running... " << std::endl;
    server.run();

    // Releases the memory used by the participant factory. Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

