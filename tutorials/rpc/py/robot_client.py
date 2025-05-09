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
from robot import Coordinates, RobotControl
from time import sleep


class RobotControlClient(RobotControl, rpc.ClientBase):
    pass


async def main():
    participant = dds.DomainParticipant(domain_id=0)
    client = RobotControlClient(participant, "MyRobotControl")

    await client.wait_for_service_async(dds.Duration(20)) # Available in Connext 7.5.0+

    print("Calling walk_to...")
    result = await client.walk_to(
        destination=Coordinates(x=10, y=10), speed=30.0
    )
    print(result)


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
