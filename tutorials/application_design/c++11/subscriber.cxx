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

    std::string create_new_position_string(
            dds::sub::cond::ReadCondition &condition);
    std::string create_dashboard_string();

    std::unordered_map<dds::core::InstanceHandle, DashboardItem>
    build_dashboard_data();
};

void SubscriberDashboard::run()
{
    dds::sub::cond::ReadCondition new_position_condition(
            transit_reader_,
            dds::sub::status::DataState::new_data(),
            [this, &new_position_condition]() {
                std::cout << create_new_position_string(new_position_condition)
                          << std::endl;
            });

    dds::core::cond::GuardCondition dashboard_condition;
    dashboard_condition.extensions().handler(
            [this]() { std::cout << create_dashboard_string() << std::endl; });

    std::mutex mutex;
    std::thread display_thread([&dashboard_condition, &mutex]() {
        for (;;) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::lock_guard<std::mutex> lock(mutex);
            dashboard_condition.trigger_value(true);
        }
    });

    dds::core::cond::WaitSet waitset;
    waitset += new_position_condition;
    waitset += dashboard_condition;

    for (;;) {
        waitset.dispatch();
        std::lock_guard<std::mutex> lock(mutex);
        dashboard_condition.trigger_value(false);
    }
}

std::string SubscriberDashboard::create_new_position_string(
        dds::sub::cond::ReadCondition &condition)
{
    using ::to_string;
    using std::to_string;
    std::stringstream ss;
    auto transit_samples = transit_reader_.select().condition(condition).read();
    for (const auto &sample : transit_samples) {
        if (!sample.info().valid()) {
            continue;
        }

        ss << "[INFO] Vehicle " << sample.data().vehicle_vin;
        auto &current_route = sample.data().current_route;
        if (current_route.has_value() && !current_route->empty()) {
            ss << " is en route to " << to_string(current_route->back())
               << " from " << to_string(sample.data().current_position);
        } else {
            ss << " has arrived at its destination in "
               << to_string(sample.data().current_position);
        }
        ss << "\n";
    }
    return ss.str();
}

std::string SubscriberDashboard::create_dashboard_string()
{
    using ::to_string;
    using std::to_string;
    std::stringstream ss;
    auto data = build_dashboard_data();

    std::vector<DashboardItem> online_vehicles;
    std::vector<DashboardItem> offline_vehicles;
    for (const auto &item : data) {
        if (!item.second.is_historical) {
            online_vehicles.push_back(item.second);
        } else {
            offline_vehicles.push_back(item.second);
        }
    }

    ss << "[[ DASHBOARD: "
       << std::chrono::system_clock::now().time_since_epoch().count()
       << " ]]\n";

    ss << "Online vehicles: " << online_vehicles.size() << "\n";
    for (auto &item : online_vehicles) {
        ss << "- Vehicle " << item.vin << "\n";
        ss << "  Known fuel updates: " << item.fuel_history.size() << "\n";
        ss << "  Last known destination: "
           << (item.current_destination
                       ? to_string(item.current_destination.value())
                       : "None")
           << "\n";
        ss << "  Last known fuel level: "
           << (item.fuel_history.empty() ? "None"
                                         : to_string(item.fuel_history.back()))
           << "\n";
    }

    ss << "Offline vehicles: " << offline_vehicles.size() << "\n";
    for (auto &item : offline_vehicles) {
        ss << "- Vehicle " << item.vin << "\n";
    }

    return ss.str();
}

std::unordered_map<dds::core::InstanceHandle, DashboardItem>
SubscriberDashboard::build_dashboard_data()
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
                auto new_data = DashboardItem { sample.data().vehicle_vin };
                it = data.emplace(new_handle, new_data).first;
            }

            auto &item = it->second;
            item.is_historical = sample.info().state().instance_state()
                    != dds::sub::status::InstanceState::alive();

            if (!sample.info().valid() && item.is_historical) {
                continue;
            }

            item.fuel_history.push_back(sample.data().fuel_level);
        }
        for (const auto &sample : transit_samples) {
            auto it = data.find(sample.info().instance_handle());
            // If not a tracked vehicle, track it.
            if (it == data.end()) {
                if (!sample.info().valid())
                    continue;

                auto new_handle = sample.info().instance_handle();
                auto new_data = DashboardItem { sample.data().vehicle_vin };
                it = data.emplace(new_handle, new_data).first;
            }

            auto &item = it->second;
            item.is_historical = sample.info().state().instance_state()
                    != dds::sub::status::InstanceState::alive();

            if (!sample.info().valid() && item.is_historical) {
                continue;
            }

            auto &current_route = sample.data().current_route;
            if (current_route->size() > 0) {
                item.current_destination = current_route->back();
            } else {
                item.current_destination.reset();
            }
        }

        return data;
    }
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

    std::cout << "Running dashboard:" << std::endl;
    dashboard.run();
}
