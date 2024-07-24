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
#include <thread>

#include <rti/rti.hpp>
#include "VehicleModeling.hpp"
#include "common.hpp"

struct DashboardItem {
    std::string vin;
    bool is_historical;
    std::vector<double> fuel_history;
    int completed_routes;
    std::unique_ptr<Coord> current_destination; // TODO: Is there a better optional?
    std::vector<Coord> reached_destinations;
};

struct SubscriberDashboard {
    explicit SubscriberDashboard(
            dds::sub::DataReader<VehicleMetrics> metrics_reader,
            dds::sub::DataReader<VehicleTransit> transit_reader)
            : metrics_reader_(metrics_reader), transit_reader_(transit_reader)
    { }

    void run()
    {
        dds::sub::cond::ReadCondition metrics_condition(
                metrics_reader_,
                dds::sub::status::DataState::new_data(),
                [this]() { metrics_app(); });

        dds::sub::cond::ReadCondition transit_condition(
                transit_reader_,
                dds::sub::status::DataState::new_data(),
                [this]() { transit_app(); });

        dds::core::cond::GuardCondition display_condition;
        display_condition.extensions().handler([this, &display_condition]() {
            display_app();
            display_condition.extensions().trigger_value(false);
        });

        std::thread display_thread([&display_condition]() {
            for (;;) {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                display_condition.extensions().trigger_value(true);
            }
        });

        waitset.attach_condition(metrics_condition);
        waitset.attach_condition(transit_condition);
        waitset.attach_condition(display_condition);

        for (;;) {
            waitset.dispatch();
        }
    }

    friend std::string utils::to_string<>(const SubscriberDashboard& dashboard);

private:
    dds::core::cond::WaitSet waitset;
    dds::sub::DataReader<VehicleMetrics> metrics_reader_;
    dds::sub::DataReader<VehicleTransit> transit_reader_;
    std::map<dds::core::InstanceHandle, DashboardItem> dashboard_data_;

    void display_app()
    {
        std::cout << "Displaying!" << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    }

    void metrics_app()
    {
    }

    void transit_app()
    {
    }
};

template <>
std::string utils::to_string(const SubscriberDashboard& dashboard)
{
    std::ostringstream ss;
    ss << "Dashboard:\n";
    return ss.str();
}

int main(int argc, char **argv)
{
    rti::domain::register_type<VehicleMetrics>();
    rti::domain::register_type<VehicleTransit>();

    dds::core::QosProvider qos_provider("../VehicleModeling.xml");

    dds::domain::DomainParticipant participant =
            qos_provider.extensions().create_participant_from_config(
                    "ParticipantLibrary::SubscriberApp");

    using MetricsReader = dds::sub::DataReader<VehicleMetrics>;
    MetricsReader metrics_reader(
            rti::sub::find_datareader_by_name<MetricsReader>(
                    participant,
                    "Subscriber::MetricsReader"));

    using TransitReader = dds::sub::DataReader<VehicleTransit>;
    TransitReader transit_reader(
            rti::sub::find_datareader_by_name<TransitReader>(
                    participant,
                    "Subscriber::TransitReader"));

    SubscriberDashboard dashboard(metrics_reader, transit_reader);
    std::cout << "Running dashboard " << utils::to_string(dashboard)
              << std::endl;
    dashboard.run();
}
