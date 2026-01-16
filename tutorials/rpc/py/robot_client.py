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

# The client class inherits from both the service interface and rpc.ClientBase.
# ClientBase provides the implementation for all @rpc.operation methods, enabling
# remote calls to matching services.


class RobotControlClient(RobotControl, rpc.ClientBase):
    pass


async def main():
    # Create a DomainParticipant on the same domain ID as the service.
    participant = dds.DomainParticipant(domain_id=0)
    
    # Create a client instance with the same service name as the server. Connext
    # automatically discovers matching services on the network.
    client = RobotControlClient(participant, "MyRobotControl")

    # Wait for the service to be discovered before making calls.
    await client.wait_for_service_async(dds.Duration(20)) # Available in Connext 7.5.0+

    # Make a remote call to walk_to. The call sends a request to the service and
    # returns a coroutine that completes when the reply is received.
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
