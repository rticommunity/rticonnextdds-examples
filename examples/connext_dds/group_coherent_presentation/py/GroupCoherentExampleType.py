#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import rti.idl as idl
from enum import IntEnum, auto
from dataclasses import field


@idl.enum
class AlarmCode(IntEnum):
    PATIENT_OK = auto()
    PATIENT_REQUEST = auto()
    SENSOR_OFFLINE = auto()
    ABNORMAL_READING = auto()


@idl.struct(
    type_annotations={
        'patient_id': [idl.key]
    }
)
class Alarm:
    patient_id: int = 0
    alarm_code: AlarmCode = field(default=AlarmCode.PATIENT_OK)


@idl.struct(
    type_annotations={
        'patient_id': [idl.key]
    }
)
class HeartRate:
    patient_id: int = 0
    beats_per_minute: idl.uint32 = 0


@idl.struct(
    type_annotations={
        'patient_id': [idl.key]
    }
)
class Temperature:
    patient_id: int = 0
    temperature: float = 0.0
