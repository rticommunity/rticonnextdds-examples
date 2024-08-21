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
    int completed_routes;
    rti::core::optional<Coord> current_destination;
    std::vector<Coord> reached_destinations;
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

    friend std::string to_string(const SubscriberDashboard &dashboard);

private:
    dds::sub::DataReader<VehicleMetrics> metrics_reader_;
    dds::sub::DataReader<VehicleTransit> transit_reader_;
    dds::core::cond::WaitSet waitset;
    std::unordered_map<dds::core::InstanceHandle, DashboardItem>
            dashboard_data_;

    void display_app();
    void metrics_app();
    void transit_app();

    std::vector<DashboardItem> online_vehicles() const
    {
        std::vector<DashboardItem> online;
        for (const auto &item : dashboard_data_) {
            if (!item.second.is_historical) {
                online.push_back(item.second);
            }
        }
        return online;
    }

    std::vector<DashboardItem> offline_vehicles() const
    {
        std::vector<DashboardItem> offline;
        for (const auto &item : dashboard_data_) {
            if (item.second.is_historical) {
                offline.push_back(item.second);
            }
        }
        return offline;
    }
};

void SubscriberDashboard::run()
{
    std::mutex mutex;

    dds::sub::cond::ReadCondition metrics_condition(
            metrics_reader_,
            dds::sub::status::DataState::any(),
            [this]() { metrics_app(); });

    dds::sub::cond::ReadCondition transit_condition(
            transit_reader_,
            dds::sub::status::DataState::any(),
            [this]() { transit_app(); });

    dds::core::cond::GuardCondition display_condition;
    display_condition.extensions().handler([this]() { display_app(); });

    std::thread display_thread([&display_condition, &mutex]() {
        for (;;) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::lock_guard<std::mutex> lock(mutex);
            display_condition.trigger_value(true);
        }
    });

    waitset.attach_condition(metrics_condition);
    waitset.attach_condition(transit_condition);
    waitset.attach_condition(display_condition);

    for (;;) {
        waitset.dispatch();
        std::lock_guard<std::mutex> lock(mutex);
        display_condition.trigger_value(false);
    }
}

void SubscriberDashboard::display_app()
{
    using ::to_string;
    using std::to_string;

    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    ss << "[[ DASHBOARD: " << now.time_since_epoch().count() << " ]]\n";
    {
        auto online = online_vehicles();
        ss << "Online vehicles: " << online.size() << "\n";
        for (auto &item : online) {
            ss << "- Vehicle " << item.vin << ":\n";
            ss << "  Fuel updates: " << item.fuel_history.size() << "\n";
            ss << "  Last known destination: "
               << (item.current_destination
                           ? to_string(*item.current_destination)
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
        auto offline = offline_vehicles();
        ss << "Offline vehicles: " << offline.size() << "\n";
        for (auto &item : offline) {
            ss << "- Vehicle " << item.vin << ":\n";
            ss << "  Mean fuel consumption: "
               << std::accumulate(
                          item.fuel_history.begin(),
                          item.fuel_history.end(),
                          0.0)
                            / item.fuel_history.size()
               << "\n";
            ss << "  Known reached destinations: "
               << item.reached_destinations.size() << "\n";
            for (auto &destination : item.reached_destinations) {
                ss << "    - " << to_string(destination) << "\n";
            }
        }
    }

    std::cout << ss.str() << std::endl;
}

void SubscriberDashboard::metrics_app()
{
    for (const auto &sample : metrics_reader_.take()) {
        auto it = dashboard_data_.find(sample.info().instance_handle());
        // If not a tracked vehicle, track it.
        if (it == dashboard_data_.end()) {
            if (!sample.info().valid())
                continue;

            auto new_handle = sample.info().instance_handle();
            auto new_data = DashboardItem { sample.data().vehicle_vin() };
            it = dashboard_data_.emplace(new_handle, new_data).first;
        }

        auto &item = it->second;
        item.is_historical = sample.info().state().instance_state()
                != dds::sub::status::InstanceState::alive();

        if (!sample.info().valid() && item.is_historical) {
            continue;
        }

        item.fuel_history.push_back(sample.data().fuel_level());
    }
}

void SubscriberDashboard::transit_app()
{
    for (const auto &sample : transit_reader_.take()) {
        auto it = dashboard_data_.find(sample.info().instance_handle());
        // If not a tracked vehicle, track it.
        if (it == dashboard_data_.end()) {
            if (!sample.info().valid())
                continue;

            auto new_handle = sample.info().instance_handle();
            auto new_data = DashboardItem { sample.data().vehicle_vin() };
            it = dashboard_data_.emplace(new_handle, new_data).first;
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
            item.reached_destinations.push_back(*item.current_destination);
            item.current_destination.reset();
            item.completed_routes++;
        }
    }
}

std::string to_string(const SubscriberDashboard &dashboard)
{
    std::ostringstream ss;
    ss << "Dashboard()";
    return ss.str();
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
