#
# (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

from abc import ABC
from dataclasses import field
from typing import Sequence
import rti.idl as idl
import rti.rpc as rpc


@idl.struct
class Item:
    name: str = ""
    quantity: int = 0


@idl.struct
class InventoryContents:
    items: Sequence[Item] = field(default_factory=list)


@idl.struct
class UnknownItemError(Exception):
    name: str = ""


@rpc.service
class InventoryService(ABC):
    @rpc.operation
    def get_inventory(self) -> InventoryContents:
        """Get the current inventory inventory"""
        ...

    @rpc.operation
    def add_item(self, item: Item):
        """Add quantity to an existing item"""
        ...

    @rpc.operation(raises=[UnknownItemError])
    def remove_item(self, item: Item):
        """Remove quantity from an existing item"""
        ...
