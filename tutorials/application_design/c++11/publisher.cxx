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

struct PublisherSimulation {
    explicit PublisherSimulation(
            dds::pub::DataWriter<VehicleMetrics> metrics_writer,
            dds::pub::DataWriter<VehicleTransit> transit_writer)
            : metrics_writer_(metrics_writer), transit_writer_(transit_writer)
    {
        vehicle_vin_ = utils::new_vin();
        vehicle_fuel_ = 100.0;
        vehicle_route_ = utils::new_route();
        vehicle_position_ = vehicle_route_[0];
    }

    bool has_ended() const
    {
        return is_out_of_fuel();
    }

    void run()
    {
        while (!has_ended()) {
            metrics_writer_.write(
                    VehicleMetrics { vehicle_vin_, vehicle_fuel_ });

            transit_writer_.write(VehicleTransit { vehicle_vin_,
                                                   vehicle_position_,
                                                   vehicle_route_ });

            std::this_thread::sleep_for(std::chrono::seconds(1));

            if (is_on_standby()) {
                std::cout << "Vehicle '" << vehicle_vin_
                          << "' has reached its destination, now moving to a "
                             "new location..."
                          << std::endl;
                vehicle_route_ = utils::new_route();
                vehicle_route_[0] = vehicle_position_;
            }

            vehicle_fuel_ -= 10 * utils::random_stduniform();
            vehicle_position_ = vehicle_route_.front();
            vehicle_route_.erase(vehicle_route_.begin());

            if (is_out_of_fuel()) {
                vehicle_fuel_ = 0.0;
                std::cout << "Vehicle '" << vehicle_vin_ << "' ran out of fuel!"
                          << std::endl;
            }
        }
    }

    friend std::string utils::to_string<>(PublisherSimulation const &sim);

private:
    dds::pub::DataWriter<VehicleMetrics> metrics_writer_;
    dds::pub::DataWriter<VehicleTransit> transit_writer_;

    std::string vehicle_vin_;
    double vehicle_fuel_;
    utils::CoordSequence vehicle_route_;
    Coord vehicle_position_;

    bool is_out_of_fuel() const
    {
        return vehicle_fuel_ <= 0.0;
    }

    bool is_on_standby() const
    {
        return vehicle_route_.empty();
    }
};

template<>
std::string utils::to_string(PublisherSimulation const &sim)
{
    std::ostringstream ss;
    ss << "PublisherSimulation(vehicle_vin: " << sim.vehicle_vin_;
    ss << ", vehicle_fuel: " << sim.vehicle_fuel_;
    ss << ", vehicle_route: " << to_string(sim.vehicle_route_);
    ss << ", vehicle_position: " << to_string(sim.vehicle_position_) << ")";
    return ss.str();
}

int main(int argc, char **argv)
{
    rti::domain::register_type<VehicleMetrics>();
    rti::domain::register_type<VehicleTransit>();

    dds::core::QosProvider qos_provider("../VehicleModeling.xml");

    dds::domain::DomainParticipant participant =
            qos_provider.extensions().create_participant_from_config(
                    "ParticipantLibrary::PublisherApp");

    using MetricsWriter = dds::pub::DataWriter<VehicleMetrics>;
    MetricsWriter metrics_writer(
            rti::pub::find_datawriter_by_name<MetricsWriter>(
                    participant,
                    "Publisher::MetricsWriter"));

    using TransitWriter = dds::pub::DataWriter<VehicleTransit>;
    TransitWriter transit_writer(
            rti::pub::find_datawriter_by_name<TransitWriter>(
                    participant,
                    "Publisher::TransitWriter"));

    PublisherSimulation simulation(metrics_writer, transit_writer);
    std::cout << "Running simulation " << utils::to_string(simulation)
              << std::endl;
    simulation.run();
}
