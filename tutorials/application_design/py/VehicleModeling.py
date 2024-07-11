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

import os
import sys
from dataclasses import field
from enum import IntEnum
from typing import Optional, Sequence, Union

import rti.idl as idl

VIN_LENGTH = 17

VIN = str


@idl.struct(type_annotations=[idl.final])
class Coord:
    lat: float = 0.0
    lon: float = 0.0


VehicleMetricsTopic = "VehicleMetrics"


@idl.struct(
    member_annotations={
        "vehicle_vin": [idl.key, idl.bound(VIN_LENGTH)],
        "current_route": [idl.bound(100)],
    }
)
class VehicleTransit:
    vehicle_vin: str = ""
    current_position: Coord = field(default_factory=Coord)
    current_route: Optional[Sequence[Coord]] = None


Percentage = float

VehicleTransitTopic = "VehicleTransit"


@idl.struct(
    member_annotations={
        "vehicle_vin": [idl.key, idl.bound(VIN_LENGTH)],
    }
)
class VehicleMetrics:
    vehicle_vin: str = ""
    fuel_level: float = 0.0
