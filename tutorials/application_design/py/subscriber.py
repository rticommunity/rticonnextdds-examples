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
import time
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
        metrics_reader: "dds.DataReader",
        transit_reader: "dds.DataReader",
    ):
        self._metrics_reader = metrics_reader
        self._transit_reader = transit_reader

    def __repr__(self):
        return f"Dashboard({self._metrics_reader=}, {self._transit_reader=})"

    def run(self):
        while True:
            print(f"[[ DASHBOARD: {datetime.now()} ]]")

            dashboard_data = self._dashboard_data()
            online_vehicles = [
                data for data in dashboard_data.values() if not data.is_historical
            ]
            offline_vehicles = [
                data for data in dashboard_data.values() if data.is_historical
            ]
            print(f"Online vehicles: {len(online_vehicles)}")
            for data in online_vehicles:
                print(f"- Vehicle {data.vin}")
                print(f"  Fuel updates: {len(data.fuel_history)}")
                print(f"  Last known destination: {data.current_destination}")
                print(f"  Last known fuel level: {data.fuel_history[-1]}")
            print(f"Offline vehicles: {len(offline_vehicles)}")
            for data in offline_vehicles:
                print(f"- Vehicle {data.vin}")
            print()
            time.sleep(0.5)

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
