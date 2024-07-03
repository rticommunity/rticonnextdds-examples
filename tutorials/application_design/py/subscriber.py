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

import asyncio
import dataclasses
import datetime
import random
import statistics
from datetime import datetime
from collections import Counter, defaultdict

import rti.asyncio
import rti.connextdds as dds
from VehicleModeling import VehicleMetrics, VehicleTransit, Coord


def new_route(n: int = 5, start: "Coord|None" = None, end: "Coord|None" = None):

    def new_random_coord():
        return Coord(
            (0.5 - random.random()) * 100,
            (0.5 - random.random()) * 100,
        )

    start = start or new_random_coord()
    intermediate = (new_random_coord() for _ in range(n))
    end = end or new_random_coord()

    return [start, *intermediate, end]


@dataclasses.dataclass
class DashboardItem:
    vin: str = None
    is_historical: "bool" = False
    fuel_history: "list[float]" = dataclasses.field(default_factory=lambda: [100.0])
    completed_routes: "int" = 0
    current_destination: Coord = None
    reached_destinations: "list[Coord]" = dataclasses.field(default_factory=list)


class SubscriberDashboard:

    def __init__(self, participant: dds.DomainParticipant):
        self._participant = participant
        self._dashboard_data = defaultdict(DashboardItem)

    def __repr__(self):
        return (
            f"Dashboard("
            f"{self._participant=}, "
            f"{self._dashboard_data=})"
        )

    async def run(self):

        # Create a waitset, attach dispatchers to update state.
        # Display one row per instance

        await asyncio.gather(
            self._display_app(),
            self._metrics_app(),
            self._transit_app(),
        )

    @property
    def online_vehicles(self):
        return {
            handle: data
            for handle, data in self._dashboard_data.items()
            if not data.is_historical
        }

    @property
    def offline_vehicles(self):
        return {
            handle: data
            for handle, data in self._dashboard_data.items()
            if data.is_historical
        }

    async def _display_app(self):
        while True:
            print(f"[[ DASHBOARD: {datetime.now()} ]]")
            print(f"Online vehicles: {len(self.online_vehicles)}")
            for data in self.online_vehicles.values():
                print(f"- Vehicle {data.vin}:")
                print(f"  Fuel updates: {len(data.fuel_history)}")
                print(f"  Last known destination: {data.current_destination}")
                print(f"  Last known fuel level: {data.fuel_history[-1]}")
            print(f"Offline vehicles: {len(self.offline_vehicles.keys())}")
            for data in self.offline_vehicles.values():
                print(f"- Vehicle {data.vin}:")
                print(f"  Mean fuel consumption: {statistics.mean(data.fuel_history) / len(data.fuel_history)}")
                print(f"  Known reached destinations: {len(data.reached_destinations)}")
                for coord in data.reached_destinations:
                    print(f"    {coord}")
            print()
            await asyncio.sleep(0.5)

    async def _metrics_app(self):
        reader = dds.DataReader(self._participant.find_datareader("MetricsReader"))

        async for sample, info in reader.take_async():
            instance_data = self._dashboard_data[info.instance_handle]
            instance_data.is_historical = (
                info.state.instance_state != dds.InstanceState.ALIVE
            )

            if not sample or instance_data.is_historical:
                continue

            instance_data.vin = sample.vehicle_vin
            instance_data.fuel_history.append(sample.fuel_level)

        print("metrics ended")

    async def _transit_app(self):
        reader = dds.DataReader(self._participant.find_datareader("TransitReader"))

        async for sample, info in reader.take_async():
            instance_data = self._dashboard_data[info.instance_handle]
            instance_data.is_historical = (
                info.state.instance_state != dds.InstanceState.ALIVE
            )

            if not sample or instance_data.is_historical:
                continue

            instance_data.vin = sample.vehicle_vin

            if len(sample.current_route):
                # Vehicle is on their way to a destination
                instance_data.current_destination = sample.current_route[-1]
            else:
                # Vehicle has finished its route
                instance_data.current_destination = None
                instance_data.reached_destinations.append(sample.current_position)

        print("transit ended")


def main():
    dds.DomainParticipant.register_idl_type(VehicleMetrics, "VehicleMetrics")
    dds.DomainParticipant.register_idl_type(VehicleTransit, "VehicleTransit")

    qos_provider = dds.QosProvider("../VehicleModeling.xml")

    with qos_provider.create_participant_from_config(
        "ParticipantLibrary::SubscriberApp"
    ) as participant:
        dashboard = SubscriberDashboard(participant)

        print(f"Using simulation parameters: {dashboard=}")

        asyncio.run(dashboard.run())


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
