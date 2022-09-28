import sys
import rti.idl as idl
from enum import IntEnum, auto
from typing import Sequence, Optional
from dataclasses import field

@idl.struct
class async_type:
    x: idl.int32 = 0