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

import random
import time
import typing

import rti.connextdds as dds

from VehicleModeling import Coord, VehicleMetrics, VehicleTransit


def new_route(
    n: int = 5,
    start: typing.Optional[Coord] = None,
    end: typing.Optional[Coord] = None,
):
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
    def __init__(
        self,
        metrics_writer: "dds.DataWriter",
        transit_writer: "dds.DataWriter",
    ):
        self._metrics_writer = metrics_writer
        self._transit_writer = transit_writer

        vehicle_vin = "".join(
            random.choices("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", k=17)
        )
        vehicle_route = new_route()
        vehicle_position = vehicle_route.pop(0)

        self._metrics = VehicleMetrics(vehicle_vin, 100.0)
        self._transit = VehicleTransit(vehicle_vin, vehicle_position, vehicle_route)

    def __repr__(self):
        return (
            f"Simulation("
            f"{self._metrics_writer=}, "
            f"{self._transit_writer=}, "
            f"{self._metrics=}, "
            f"{self._transit=})"
        )

    @property
    def has_ended(self):
        return self._is_out_of_fuel

    @property
    def _is_out_of_fuel(self):
        return self._metrics.fuel_level <= 0.0

    @property
    def _is_on_standby(self):
        return not self._transit.current_route

    def run(self):
        while not self.has_ended:
            self._metrics_writer.write(self._metrics)
            self._transit_writer.write(self._transit)

            time.sleep(1)

            if self._is_on_standby:
                print(
                    f"Vehicle '{self._metrics.vehicle_vin}' has reached its destination, now moving to a new location..."
                )
                self._transit.current_route = new_route(
                    start=self._transit.current_position
                )

            self._transit.current_position = self._transit.current_route.pop(0)
            self._metrics.fuel_level -= 10 * random.random()

            if self._is_out_of_fuel:
                self._metrics.fuel_level = 0.0

                print(f"Vehicle '{self._metrics.vehicle_vin}' ran out of fuel!")


def main():
    dds.DomainParticipant.register_idl_type(VehicleMetrics, "VehicleMetrics")
    dds.DomainParticipant.register_idl_type(VehicleTransit, "VehicleTransit")

    qos_provider = dds.QosProvider("../VehicleModeling.xml")

    with qos_provider.create_participant_from_config(
        "ParticipantLibrary::PublisherApp"
    ) as participant:
        metrics_writer = dds.DataWriter(
            participant.find_datawriter("Publisher::MetricsWriter")
        )
        transit_writer = dds.DataWriter(
            participant.find_datawriter("Publisher::TransitWriter")
        )

        simulation = PublisherSimulation(
            metrics_writer=metrics_writer, transit_writer=transit_writer
        )
        print(f"Running simulation: {simulation=}")
        simulation.run()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
