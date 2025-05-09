//
// (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
//
//  RTI grants Licensee a license to use, modify, compile, and create derivative
//  works of the Software.  Licensee has the right to distribute object form
//  only for use with RTI products.  The Software is provided "as is", with no
//  warranty of any type, including any warranty for fitness for any purpose.
//  RTI is under no obligation to maintain or support the Software.  RTI shall
//  not be liable for any incidental or consequential damages arising out of the
//  use or inability to use the software.
//

#include <iostream>
#include <sstream>
#include <thread>

#include <rti/rti.hpp>
#include "VehicleModeling.hpp"
#include "common.hpp"

class PublisherSimulation {
public:
    explicit PublisherSimulation(
            dds::pub::DataWriter<VehicleMetrics> metrics_writer,
            dds::pub::DataWriter<VehicleTransit> transit_writer)
            : metrics_writer_(metrics_writer), transit_writer_(transit_writer)
    {
        auto new_vin = utils::new_vin();
        auto new_route = utils::create_new_route();

        metrics_.vehicle_vin(new_vin);
        metrics_.fuel_level(100.0);

        transit_.vehicle_vin(new_vin);
        transit_.current_route(new_route);
        transit_.current_position(new_route.front());
    }

    bool has_ended() const
    {
        return is_out_of_fuel();
    }

    void run();

private:
    dds::pub::DataWriter<VehicleMetrics> metrics_writer_;
    dds::pub::DataWriter<VehicleTransit> transit_writer_;

    VehicleMetrics metrics_;
    VehicleTransit transit_;

    bool is_out_of_fuel() const
    {
        return metrics_.fuel_level() <= 0.0;
    }

    bool is_on_standby() const
    {
        return !transit_.current_route().has_value()
                || transit_.current_route().value().empty();
    }
};

void PublisherSimulation::run()
{
    while (!has_ended()) {
        metrics_writer_.write(metrics_);
        transit_writer_.write(transit_);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (is_on_standby()) {
            std::cout << "Vehicle '" << metrics_.vehicle_vin()
                      << "' has reached its destination, now moving to a "
                         "new location..."
                      << std::endl;

            auto new_route = utils::create_new_route();
            new_route.front() = transit_.current_position();
            transit_.current_route(new_route);
        }

        metrics_.fuel_level() -= 10 * utils::random_stduniform();
        transit_.current_position(transit_.current_route().value().front());
        transit_.current_route().value().erase(
                transit_.current_route().value().begin());

        if (is_out_of_fuel()) {
            metrics_.fuel_level(0.0);

            std::cout << "Vehicle '" << metrics_.vehicle_vin()
                      << "' ran out of fuel!" << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    rti::domain::register_type<VehicleMetrics>();
    rti::domain::register_type<VehicleTransit>();

    dds::core::QosProvider qos_provider("../VehicleModeling.xml");

    auto participant = qos_provider.extensions().create_participant_from_config(
            "ParticipantLibrary::PublisherApp");

    using MetricsWriter = dds::pub::DataWriter<VehicleMetrics>;
    auto metrics_writer = rti::pub::find_datawriter_by_name<MetricsWriter>(
            participant,
            "Publisher::MetricsWriter");

    using TransitWriter = dds::pub::DataWriter<VehicleTransit>;
    auto transit_writer = rti::pub::find_datawriter_by_name<TransitWriter>(
            participant,
            "Publisher::TransitWriter");

    PublisherSimulation simulation(metrics_writer, transit_writer);
    std::cout << "Running simulation:" << std::endl;
    simulation.run();
}
