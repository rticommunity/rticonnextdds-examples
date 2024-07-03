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
import random

import rti.connextdds as dds

from VehicleModeling import Coord, VehicleMetrics, VehicleTransit


def new_route(n: int = 5, start: Coord = None, end: Coord = None):

    def new_random_coord():
        return Coord(
            (0.5 - random.random()) * 100,
            (0.5 - random.random()) * 100,
        )

    start = start or new_random_coord()
    intermediate = (new_random_coord() for _ in range(n))
    end = end or new_random_coord()

    return [start, *intermediate, end]


class PublisherSimulation:

    def __init__(self, participant: dds.DomainParticipant):
        self._participant = participant
        self._vehicle_vin: str = "".join(
            random.choices("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", k=17)
        )
        self._vehicle_fuel = 100.0
        self._vehicle_route = new_route()
        self._vehicle_position = self._vehicle_route.pop(0)

    def __repr__(self):
        return (
            f"Simulation("
            f"{self._participant=}, "
            f"{self._vehicle_vin=}, "
            f"{self._vehicle_fuel=}, "
            f"{self._vehicle_route=}, "
            f"{self._vehicle_position=})"
        )

    @property
    def has_ended(self):
        return self._is_out_of_fuel

    @property
    def _is_out_of_fuel(self):
        return self._vehicle_fuel <= 0.0

    @property
    def _is_on_standby(self):
        return not self._vehicle_route

    async def run(self):
        await asyncio.gather(
            self._metrics_app(),
            self._transit_app(),
        )

    async def _metrics_app(self):
        writer = dds.DataWriter(self._participant.find_datawriter("MetricsWriter"))

        print(f"Starting metrics app: {writer=}")
        await asyncio.sleep(1)

        while not self.has_ended:
            writer.write(
                VehicleMetrics(
                    self._vehicle_vin,
                    self._vehicle_fuel,
                )
            )

            await asyncio.sleep(1)

            self._vehicle_fuel -= 10 * random.random()

            if self._is_out_of_fuel:
                self._vehicle_fuel = 0.0

                print(f"Vehicle '{self._vehicle_vin}' ran out of fuel!")

    async def _transit_app(self):
        writer = dds.DataWriter(self._participant.find_datawriter("TransitWriter"))

        print(f"Starting transit app: {writer=}")
        await asyncio.sleep(1)

        while not self.has_ended:
            writer.write(
                VehicleTransit(
                    self._vehicle_vin,
                    current_route=self._vehicle_route,
                    current_position=self._vehicle_position,
                )
            )

            await asyncio.sleep(1)

            if self._is_on_standby:
                print(
                    f"Vehicle '{self._vehicle_vin}' has reached its destination, now moving to a new location..."
                )
                self._vehicle_route = new_route()

            self._vehicle_position = self._vehicle_route.pop(0)


def main():
    dds.DomainParticipant.register_idl_type(VehicleMetrics, "VehicleMetrics")
    dds.DomainParticipant.register_idl_type(VehicleTransit, "VehicleTransit")

    qos_provider = dds.QosProvider("VehicleModeling.xml")

    with qos_provider.create_participant_from_config(
        "ParticipantLibrary::PublisherApp"
    ) as participant:
        simulation = PublisherSimulation(participant)

        print(f"Using simulation parameters: {simulation=}")

        asyncio.run(simulation.run())


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
