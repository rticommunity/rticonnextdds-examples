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
import rti.connextdds as dds
import rti.rpc as rpc
import rti.types as idl
from robot import Coordinates, RobotControl


class RobotControlExample(RobotControl):
    def __init__(self):
        self.position = Coordinates(0, 0)
        self.speed = 0.0

    async def walk_to(
        self, destination: Coordinates, speed: idl.float32
    ) -> Coordinates:
        if speed <= 0.0:
            return self.position  # return previously known position

        self.speed = speed
        print(f"Robot walking at {self.speed}mph to {destination}")
        await asyncio.sleep(10.0 - 0.1 * self.speed)

        self.speed = 0.0
        self.position = destination

        print(f"Robot arrived at {destination}")
        return destination

    def get_speed(self) -> idl.float32:
        return self.speed


async def main():
    participant = dds.DomainParticipant(domain_id=0)
    robot_control = RobotControlExample()
    service = rpc.Service(robot_control, participant, "MyRobotControl")
    await service.run()  # runs forever


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
