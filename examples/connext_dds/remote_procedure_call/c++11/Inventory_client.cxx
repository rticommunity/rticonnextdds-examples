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

#include <iostream>
#include "application.hpp"
#include "Inventory.hpp"

void run_client(
        int domain_id,
        bool add,
        const std::string &item_name,
        int quantity)
{
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create a ClientParams to provide a service name and a maximum wait time
    // for remote calls
    dds::rpc::ClientParams client_params(participant);
    client_params.service_name("Inventory");
    client_params.function_call_max_wait(dds::core::Duration(20));

    // Create a client with the ClientParams
    ::InventoryServiceClient client(client_params);

    // Wait until the service is started
    client.wait_for_service();

    std::cout << "Initial inventory: " << client.get_inventory() << std::endl;

    if (add) {
        std::cout << "Add " << quantity << " " << item_name << std::endl;
        client.add_item(::Item(item_name, quantity));
    } else {
        std::cout << "Remove " << quantity << " " << item_name << std::endl;
        try {
            client.remove_item(::Item(item_name, quantity));
        } catch (const UnknownItemError &e) {
            std::cout << "Unknown item: " << e.name() << std::endl;
        }
    }

    std::cout << "Updated inventory: " << client.get_inventory() << std::endl;
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments
    auto arguments = parse_arguments(argc, argv, true);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_client(
                arguments.domain_id,
                arguments.add,
                arguments.item_name,
                arguments.quantity);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_client(): " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Releases the memory used by the participant factory. Optional at
    // application exit
    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}