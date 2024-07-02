
# WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

# This file was generated from VehicleModeling.idl
# using RTI Code Generator (rtiddsgen) version 4.3.0.
# The rtiddsgen tool is part of the RTI Connext DDS distribution.
# For more information, type 'rtiddsgen -help' at a command shell
# or consult the Code Generator User's Manual.

from dataclasses import field
from typing import Union, Sequence, Optional
import rti.idl as idl
from enum import IntEnum
import sys
import os


VIN_LENGTH = 17

VIN = str

@idl.struct(
    type_annotations = [idl.final])
class Coord:
    lat: float = 0.0
    lon: float = 0.0

VehicleMetricsTopic = "VehicleMetrics"

@idl.struct(
    member_annotations = {
        'vehicle_vin': [idl.key, idl.bound(VIN_LENGTH)],
        'current_route': [idl.bound(100)],
    }
)
class VehicleTransit:
    vehicle_vin: str = ""
    current_position: Coord = field(default_factory = Coord)
    current_route: Optional[Sequence[Coord]] = None

Percentage = float

VehicleTransitTopic = "VehicleTransit"

@idl.struct(
    member_annotations = {
        'vehicle_vin': [idl.key, idl.bound(VIN_LENGTH)],
    }
)
class VehicleMetrics:
    vehicle_vin: str = ""
    fuel_level: float = 0.0
