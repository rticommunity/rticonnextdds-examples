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
#include <unordered_map>

#include <rti/rti.hpp>
#include "VehicleModeling.hpp"
#include "common.hpp"

struct DashboardItem {
    std::string vin;
    bool is_historical;
    std::vector<double> fuel_history;
    rti::core::optional<Coord> current_destination;
};

class SubscriberDashboard {
public:
    explicit SubscriberDashboard(
            dds::sub::DataReader<VehicleMetrics> metrics_reader,
            dds::sub::DataReader<VehicleTransit> transit_reader)
            : metrics_reader_(metrics_reader), transit_reader_(transit_reader)
    {
    }

    void run();

    std::string to_string() const;

private:
    dds::sub::DataReader<VehicleMetrics> metrics_reader_;
    dds::sub::DataReader<VehicleTransit> transit_reader_;

    std::string output_string();

    std::unordered_map<dds::core::InstanceHandle, DashboardItem>
    dashboard_data();
};

void SubscriberDashboard::run()
{
    for (;;) {
        std::cout << output_string() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string SubscriberDashboard::output_string()
{
    using ::to_string;
    using std::to_string;
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    ss << "[[ DASHBOARD: " << now.time_since_epoch().count() << " ]]\n";
    auto data = dashboard_data();
    {
        std::vector<DashboardItem> online;
        for (const auto &item : data) {
            if (!item.second.is_historical) {
                online.push_back(item.second);
            }
        }
        ss << "Online vehicles: " << online.size() << "\n";
        for (auto &item : online) {
            ss << "- Vehicle " << item.vin << "\n";
            ss << "  Known fuel updates: " << item.fuel_history.size() << "\n";
            ss << "  Last known destination: "
               << (item.current_destination
                           ? to_string(item.current_destination.value())
                           : "None")
               << "\n";
            ss << "  Last known fuel level: "
               << (item.fuel_history.empty()
                           ? "None"
                           : to_string(item.fuel_history.back()))
               << "\n";
        }
    }
    {
        std::vector<DashboardItem> offline;
        for (const auto &item : data) {
            if (item.second.is_historical) {
                offline.push_back(item.second);
            }
        }
        ss << "Offline vehicles: " << offline.size() << "\n";
        for (auto &item : offline) {
            ss << "- Vehicle " << item.vin << "\n";
        }
    }

    return ss.str();
}

std::unordered_map<dds::core::InstanceHandle, DashboardItem>
SubscriberDashboard::dashboard_data()
{
    {
        std::unordered_map<dds::core::InstanceHandle, DashboardItem> data;
        auto metric_samples = metrics_reader_.read();
        auto transit_samples = transit_reader_.read();

        for (const auto &sample : metric_samples) {
            auto it = data.find(sample.info().instance_handle());
            // If not a tracked vehicle, track it.
            if (it == data.end()) {
                if (!sample.info().valid())
                    continue;

                auto new_handle = sample.info().instance_handle();
                auto new_data = DashboardItem { sample.data().vehicle_vin() };
                it = data.emplace(new_handle, new_data).first;
            }

            auto &item = it->second;
            item.is_historical = sample.info().state().instance_state()
                    != dds::sub::status::InstanceState::alive();

            if (!sample.info().valid() && item.is_historical) {
                continue;
            }

            item.fuel_history.push_back(sample.data().fuel_level());
        }
        for (const auto &sample : transit_samples) {
            auto it = data.find(sample.info().instance_handle());
            // If not a tracked vehicle, track it.
            if (it == data.end()) {
                if (!sample.info().valid())
                    continue;

                auto new_handle = sample.info().instance_handle();
                auto new_data = DashboardItem { sample.data().vehicle_vin() };
                it = data.emplace(new_handle, new_data).first;
            }

            auto &item = it->second;
            item.is_historical = sample.info().state().instance_state()
                    != dds::sub::status::InstanceState::alive();

            if (!sample.info().valid() && item.is_historical) {
                continue;
            }

            auto &current_route = sample.data().current_route();
            if (current_route->size() > 0) {
                item.current_destination = current_route->back();
            } else {
                item.current_destination.reset();
            }
        }

        return data;
    }
}

std::string SubscriberDashboard::to_string() const
{
    std::ostringstream ss;
    ss << "Dashboard()";
    return ss.str();
}

std::string to_string(const SubscriberDashboard &dashboard)
{
    return dashboard.to_string();
}

int main(int argc, char **argv)
{
    rti::domain::register_type<VehicleMetrics>();
    rti::domain::register_type<VehicleTransit>();

    dds::core::QosProvider qos_provider("../VehicleModeling.xml");

    auto participant = qos_provider.extensions().create_participant_from_config(
            "ParticipantLibrary::SubscriberApp");

    using MetricsReader = dds::sub::DataReader<VehicleMetrics>;
    auto metrics_reader = rti::sub::find_datareader_by_name<MetricsReader>(
            participant,
            "Subscriber::MetricsReader");

    using TransitReader = dds::sub::DataReader<VehicleTransit>;
    auto transit_reader = rti::sub::find_datareader_by_name<TransitReader>(
            participant,
            "Subscriber::TransitReader");

    SubscriberDashboard dashboard(metrics_reader, transit_reader);
    std::cout << "Running dashboard " << to_string(dashboard) << std::endl;
    dashboard.run();
}
