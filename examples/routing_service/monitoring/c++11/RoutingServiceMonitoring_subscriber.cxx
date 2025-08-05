/*
 * (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

/*
 * RoutingServiceMonitoring_subscriber.cxx
 *
 * A subscription example to the Routing Service Monitoring topics
 *
 * */
#include <algorithm>
#include <dds/dds.hpp>
#include <exception>
#include <iostream>
#include <map>
#include <sstream>

#include "RoutingServiceMonitoring.hpp"

/*
 * Get the Configuration sample (rti/service/monitoring/config topic) from its
 * Instance Handle. The reader for that topic is passed as a parameter. Also the
 * Instance Handle.
 * */
RTI::RoutingService::Monitoring::Config getConfig(
        dds::sub::DataReader<RTI::RoutingService::Monitoring::Config>
                &configReader,
        const dds::core::InstanceHandle &handle)
{
    // Get the configuration for an instance handle to use this information to
    // complement what is received in the event or periodic topic
    auto cSamples = configReader.select()
                            .instance(handle)
                            .state(dds::sub::status::DataState::any_data())
                            .read();
    if (cSamples.length() == 0) {
        std::stringstream errorMsg;
        errorMsg << "The configuration for the instance handle " << handle
                 << " has not being received";
        throw std::runtime_error((errorMsg.str()));
    }
    // We are interested only in the last config sample received
    return cSamples[cSamples.length() - 1].data();
}

/* Process the samples from the "rti/service/monitoring/periodic" topic. It
 * shows in the console information related to the Domain Route, and Routing
 * Service process.
 * */
int processPeriodicData(
        dds::sub::DataReader<RTI::RoutingService::Monitoring::Periodic>
                &periodicReader,
        dds::sub::DataReader<RTI::RoutingService::Monitoring::Config>
                &configReader)
{
    int count = 0;
    try {
        // Take all samples
        dds::sub::LoanedSamples<RTI::RoutingService::Monitoring::Periodic>
                samples = periodicReader.take();
        for (const auto &sample : samples) {
            if (sample.info().valid()) {
                count++;
                switch (sample.data().value._d()) {
                case (RTI::Service::Monitoring::ResourceKind::
                              ROUTING_DOMAIN_ROUTE): {
                    auto config = getConfig(
                            configReader,
                            sample.info().instance_handle());
                    std::cout << "Periodic data:\n\tDomain Route: ";
                    if (config.value
                                .routing_domain_route()
                                .connections
                                .value()
                                .size()
                        > 0) {
                        std::cout << " { ";
                        for (const auto &connection :
                             config.value
                                     .routing_domain_route()
                                     .connections
                                     .value()) {
                            std::cout << connection.name << ", ";
                        }
                        std::cout << " } " << std::endl;
                    }

                    std::cout << "\n\t\t in samples/s (mean): "
                              << sample.data()
                                         .value
                                         .routing_domain_route()
                                         .in_samples_per_sec
                                         .value()
                                         .publication_period_metrics
                                         .mean
                              << ", out samples/s (mean): "
                              << sample.data()
                                         .value
                                         .routing_domain_route()
                                         .out_samples_per_sec
                                         .value()
                                         .publication_period_metrics
                                         .mean
                              << std::endl;
                    break;
                }
                case (RTI::Service::Monitoring::ResourceKind::
                              ROUTING_SERVICE): {
                    auto config = getConfig(
                            configReader,
                            sample.info().instance_handle());
                    std::cout << "Periodic data:\n\t > Routing Service "
                              << config.value
                                         .routing_service()
                                         .application_name
                              << "\n\t\t cpu usage (mean): "
                              << sample.data()
                                         .value
                                         .routing_service()
                                         .process
                                         .value()
                                         .cpu_usage_percentage
                                         .value()
                                         .publication_period_metrics
                                         .mean
                              << " %" << std::endl;
                    break;
                }
                default:
                    // Nothing to show
                    break;
                }
            }
        }
    } catch (const dds::core::Exception &e) {
        std::cerr << "A Connext exception has been thrown: " << e.what()
                  << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "An std exception has been thrown: " << e.what()
                  << std::endl;
    }

    return count;
    // The LoanedSamples destructor returns the loan
}

/*
 * Process the samples from the "rti/service/monitoring/event" topic. It shows
 * in the console information related to the Domain Route, Inputs and Outputs
 * (Routing Service).
 * */
int processEventData(
        dds::sub::DataReader<RTI::RoutingService::Monitoring::Event>
                &eventReader,
        dds::sub::DataReader<RTI::RoutingService::Monitoring::Config>
                &configReader)
{
    int count = 0;
    try {
        // Take all samples
        dds::sub::LoanedSamples<RTI::RoutingService::Monitoring::Event>
                samples = eventReader.take();
        for (const auto &sample : samples) {
            if (sample.info().valid()) {
                count++;
                switch (sample.data().value._d()) {
                case (RTI::Service::Monitoring::ResourceKind::
                              ROUTING_DOMAIN_ROUTE): {
                    std::cout << "\t > The Domain Route status is "
                              << sample.data()
                                         .value
                                         .routing_domain_route()
                                         .state
                              << std::endl;
                    if (sample.data()
                                .value
                                .routing_domain_route()
                                .connections
                                .value()
                                .size()
                        > 0) {
                        std::cout << "\t\t Connections available: { ";
                        for (const auto &connection :
                             sample.data()
                                     .value
                                     .routing_domain_route()
                                     .connections
                                     .value()) {
                            std::cout << " Participant name: "
                                      << connection.name << ", ";
                        }
                        std::cout << " } " << std::endl;
                    }
                    break;
                }
                case (RTI::Service::Monitoring::ResourceKind::ROUTING_INPUT): {
                    auto config = getConfig(
                            configReader,
                            sample.info().instance_handle());
                    if (config.value._d()
                        == RTI::Service::Monitoring::ResourceKind::
                                ROUTING_INPUT) {
                        std::cout
                                << "\t > The Input "
                                << config.value.routing_input().resource_id
                                << std::endl;
                        std::cout
                                << "\t\t status is "
                                << sample.data().value.routing_input().state
                                << std::endl;
                        std::cout
                                << "\t\t Topic: "
                                << config.value.routing_input().stream_name
                                << ", Type: "
                                << config.value
                                           .routing_input()
                                           .registered_type_name
                                << ", Participant name: "
                                << config.value
                                           .routing_input()
                                           .connection_name
                                << std::endl;
                    }
                    break;
                }
                case (RTI::Service::Monitoring::ResourceKind::ROUTING_OUTPUT): {
                    auto config = getConfig(
                            configReader,
                            sample.info().instance_handle());
                    if (config.value._d()
                        == RTI::Service::Monitoring::ResourceKind::
                                ROUTING_OUTPUT) {
                        std::cout
                                << "\t > The Output "
                                << config.value.routing_output().resource_id
                                << std::endl;
                        std::cout << "\t > status is "
                                  << sample.data()
                                             .value
                                             .routing_output()
                                             .state
                                  << std::endl;
                        std::cout
                                << "\t\t Topic: "
                                << config.value.routing_output().stream_name
                                << ", Type: "
                                << config.value
                                           .routing_output()
                                           .registered_type_name
                                << ", Participant name: "
                                << config.value
                                           .routing_output()
                                           .connection_name
                                << std::endl;
                    }
                    break;
                }
                default:
                    // Nothing to show
                    break;
                }
            }
        }
    }

    catch (const dds::core::Exception &e) {
        std::cerr << "A Connext exception has been thrown: " << e.what()
                  << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "An std exception has been thrown: " << e.what()
                  << std::endl;
    }

    return count;
    // The LoanedSamples destructor returns the loan
}

int subscriberMain(int domain_id, int sample_count)
{
    auto qosProvider = dds::core::QosProvider::Default();
    // Create a DomainParticipant with default Qos
    dds::domain::DomainParticipant participant(domain_id);

    // Create the Topics -- and automatically register the type
    dds::topic::Topic<RTI::RoutingService::Monitoring::Event> eventTopic(
            participant,
            "rti/service/monitoring/event");
    dds::topic::Topic<RTI::RoutingService::Monitoring::Config> configTopic(
            participant,
            "rti/service/monitoring/config");
    dds::topic::Topic<RTI::RoutingService::Monitoring::Periodic> periodicTopic(
            participant,
            "rti/service/monitoring/periodic");

    // Create a DataReaders
    dds::sub::Subscriber subscriber(
            participant,
            qosProvider.subscriber_qos("monitoring_Library::event_Profile"));
    dds::sub::DataReader<RTI::RoutingService::Monitoring::Event> eventReader(
            subscriber,
            eventTopic,
            qosProvider.datareader_qos("monitoring_Library::event_Profile"));
    dds::sub::DataReader<RTI::RoutingService::Monitoring::Config> configReader(
            subscriber,
            configTopic,
            qosProvider.datareader_qos("monitoring_Library::config_Profile"));
    dds::sub::DataReader<RTI::RoutingService::Monitoring::Periodic>
            periodicReader(
                    subscriber,
                    periodicTopic,
                    qosProvider.datareader_qos(
                            "monitoring_Library::periodic_Profile"));
    int count = 0;
    // Create a ReadCondition for any data on this reader
    dds::sub::cond::ReadCondition eventReadCondition(
            eventReader,
            dds::sub::status::DataState::any(),
            [&eventReader, &configReader, &count]() {
                count += processEventData(eventReader, configReader);
            });
    dds::sub::cond::ReadCondition periodicReadCondition(
            periodicReader,
            dds::sub::status::DataState::any(),
            [&periodicReader, &configReader, &count]() {
                count += processPeriodicData(periodicReader, configReader);
            });

    // Create a WaitSet and attach the ReadCondition
    dds::core::cond::WaitSet waitset;
    waitset += eventReadCondition;
    waitset += periodicReadCondition;

    std::cout << "RTI::RoutingService::Monitoring::Event subscriber is running"
              << std::endl;
    while (count < sample_count || sample_count == 0) {
        // Dispatch calls the conditions WaitSet handlers when they activate
        waitset.dispatch(dds::core::Duration(4));
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    // infinite loop
    int sample_count = 0;

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /* To turn on additional logging, include <rti/config/Logger.hpp> and
     uncomment the following line: */
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        subscriberMain(domain_id, sample_count);
    } catch (const std::exception &ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in subscriberMain(): " << ex.what()
                  << std::endl;
        return -1;
    }

    /* RTI Connext provides a finalize_participant_factory() method
       if you want to release memory used by the participant factory singleton.
       Uncomment the following line to release the singleton:*/
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}
