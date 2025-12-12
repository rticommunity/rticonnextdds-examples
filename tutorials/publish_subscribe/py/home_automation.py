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

from rti.types import struct, key

# Defines a data type to model a device status with three fields: a string for
# the sensor name, another string for the room name, and a boolean to indicate
# whether the door or window is open or closed. The sensor name field is a key
# so each sensor is treated as a unique instance.


@struct(member_annotations={"sensor_name": [key]})
class DeviceStatus:
    sensor_name: str = ""
    room_name: str = ""
    is_open: bool = False
