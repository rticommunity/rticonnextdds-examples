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

import abc
import rti.types as idl
import rti.rpc as rpc


@idl.struct
class Coordinates:
    x: int = 0
    y: int = 0


@rpc.service
class RobotControl(abc.ABC):
    @rpc.operation
    async def walk_to(
        self, destination: Coordinates, speed: idl.float32
    ) -> Coordinates:
        pass

    @rpc.operation
    async def get_speed(self) -> idl.float32:
        pass
