#
# (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import dataclasses
import threading
import typing
from datetime import datetime

import rti.connextdds as dds

from VehicleModeling import Coord, VehicleMetrics, VehicleTransit


@dataclasses.dataclass
class DashboardItem:
    vin: str

    is_historical: bool = False
    fuel_history: typing.List[float] = dataclasses.field(
        default_factory=lambda: [100.0]
    )
    current_destination: typing.Optional[Coord] = None


class SubscriberDashboard:
    def __init__(
        self,
        metrics_reader: dds.DataReader,
        transit_reader: dds.DataReader,
    ) -> None:
        self._metrics_reader = metrics_reader
        self._transit_reader = transit_reader

    def __repr__(self):
        return f"Dashboard({self._metrics_reader=}, {self._transit_reader=})"

    def run(self):

        def new_position_handler(_):
            print(self._new_position_string(new_position_condition))

        new_position_condition = dds.ReadCondition(
            self._transit_reader, dds.DataState.new_data, new_position_handler
        )

        dashboard_condition = dds.GuardCondition()

        def dashboard_handler(_):
            print(self._dashboard_string())

        dashboard_condition.set_handler(dashboard_handler)

        display_handler_sentinel = threading.Event()
        display_handler_condition = threading.Condition()

        def display_handler():
            while not display_handler_sentinel.is_set():
                with display_handler_condition:
                    display_handler_condition.wait(5)
                    dashboard_condition.trigger_value = True

        display_thread = threading.Thread(target=display_handler)

        waitset = dds.WaitSet()
        waitset.attach_condition(new_position_condition)
        waitset.attach_condition(dashboard_condition)

        display_thread.start()
        try:
            while True:
                waitset.dispatch()
                dashboard_condition.trigger_value = False
        except KeyboardInterrupt:
            with display_handler_condition:
                display_handler_sentinel.set()
                display_handler_condition.notify_all()

        display_thread.join()

    def _new_position_string(self, condition):
        string = str()
        for sample, info in self._transit_reader.select().condition(condition).read():
            if not info.valid:
                continue

            string += f"[INFO] Vehicle {sample.vehicle_vin}"
            if sample.current_route and len(sample.current_route):
                string += f" is enroute to {sample.current_route[-1]} from {sample.current_position}"
            else:
                string += (
                    f" has arrived at its destination in {sample.current_position}"
                )
            string += "\n"
        return string

    def _dashboard_string(self):
        dashboard_data = self._dashboard_data()
        online_vehicles = [
            data for data in dashboard_data.values() if not data.is_historical
        ]
        offline_vehicles = [
            data for data in dashboard_data.values() if data.is_historical
        ]

        online_str = "\n".join(
            f"Vehicle {data.vin}:\n"
            f"  Fuel updates: {len(data.fuel_history)}\n"
            f"  Last known destination: {data.current_destination}\n"
            f"  Last known fuel level: {data.fuel_history[-1]}\n"
            for data in online_vehicles
        )
        offline_str = "\n".join(f"Vehicle {data.vin}" for data in offline_vehicles)

        return "\n".join(
            [
                f"[[ DASHBOARD: {datetime.now()} ]]",
                f"Online vehicles: {len(online_vehicles)}",
                online_str,
                f"Offline vehicles: {len(offline_vehicles)}",
                offline_str,
            ]
        )

    def _dashboard_data(self):
        data: typing.Dict[dds.InstanceHandle, DashboardItem] = {}

        metrics = self._metrics_reader.read()
        transit = self._transit_reader.read()

        for sample, info in metrics:
            if info.instance_handle not in data:
                if sample is None:
                    continue
                data[info.instance_handle] = DashboardItem(sample.vehicle_vin)

            instance_data = data[info.instance_handle]
            instance_data.is_historical = (
                info.state.instance_state != dds.InstanceState.ALIVE
            )

            if not sample or instance_data.is_historical:
                continue

            instance_data.fuel_history.append(sample.fuel_level)

        for sample, info in transit:
            if info.instance_handle not in data:
                if sample is None:
                    continue
                data[info.instance_handle] = DashboardItem(sample.vehicle_vin)

            instance_data = data[info.instance_handle]
            instance_data.is_historical = (
                info.state.instance_state != dds.InstanceState.ALIVE
            )

            if not sample or instance_data.is_historical:
                continue

            if len(sample.current_route):
                # Vehicle is on their way to a destination
                instance_data.current_destination = sample.current_route[-1]
            else:
                # Vehicle has finished its route
                instance_data.current_destination = None

        return data


def main():
    dds.DomainParticipant.register_idl_type(VehicleMetrics, "VehicleMetrics")
    dds.DomainParticipant.register_idl_type(VehicleTransit, "VehicleTransit")

    qos_provider = dds.QosProvider("../VehicleModeling.xml")

    with qos_provider.create_participant_from_config(
        "ParticipantLibrary::SubscriberApp"
    ) as participant:
        transit_reader = dds.DataReader(
            participant.find_datareader("Subscriber::TransitReader")
        )
        metrics_reader = dds.DataReader(
            participant.find_datareader("Subscriber::MetricsReader")
        )

        dashboard = SubscriberDashboard(
            transit_reader=transit_reader, metrics_reader=metrics_reader
        )

        print(f"Running dashboard: {dashboard=}")
        dashboard.run()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
