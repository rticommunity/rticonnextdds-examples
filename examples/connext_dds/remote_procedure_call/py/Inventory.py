
# WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

# This file was generated from Inventory.idl
# using RTI Code Generator (rtiddsgen) version 4.4.0.
# The rtiddsgen tool is part of the RTI Connext DDS distribution.
# For more information, type 'rtiddsgen -help' at a command shell
# or consult the Code Generator User's Manual.

from dataclasses import field
from typing import Union, Sequence, Optional
import rti.idl as idl
import rti.rpc as rpc
from enum import IntEnum
import sys
import os
from abc import ABC



@idl.struct(
    member_annotations = {
        'name': [idl.bound(255)],
    }
)
class Item:
    name: str = ""
    quantity: int = 0

@idl.struct(
    member_annotations = {
        'items': [idl.bound(100)],
    }
)
class InventoryContents:
    items: Sequence[Item] = field(default_factory = list)


@idl.struct(
    member_annotations = {
        'name': [idl.bound(255)],
    }
)
class UnknownItemError(Exception):
    name: str = ""


@rpc.service
class InventoryService(ABC):
    @rpc.operation
    async def get_inventory(self) -> InventoryContents:
        """Get the current inventory inventory"""
        ...

    @rpc.operation
    async def add_item(self, item: Item):
        """Add quantity to an existing item"""
        ...

    @rpc.operation(raises=[UnknownItemError])
    async def remove_item(self, item: Item):
        """Remove quantity from an existing item"""
        ...
