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

import argparse
import rti.connextdds as dds
import rti.rpc as rpc
import rti.asyncio
from asyncio import sleep

from Inventory import (
    Item,
    InventoryContents,
    InventoryService,
    UnknownItemError,
)


class InventoryImpl(InventoryService):
    """Implements a simple InventoryService"""

    def __init__(self, delay: int = 0):
        """Creates an InventoryImpl with a few initial items"""

        self.delay = delay
        self.inventory = {"apples": 100, "oranges": 50}

    def get_inventory(self):
        return InventoryContents(
            [Item(name, qty) for name, qty in self.inventory.items()]
        )

    async def add_item(self, item: Item):
        if self.delay > 0:
            # Simulate a long-running operation that suspends add_item
            await sleep(self.delay)

        if item.name not in self.inventory:
            self.inventory[item.name] = item.quantity
        else:
            self.inventory[item.name] += item.quantity

    async def remove_item(self, item: Item):
        if item.name not in self.inventory:
            raise UnknownItemError(item.name)

        if self.delay > 0:
            # Simulate a long-running operation that suspends remove_item
            await sleep(self.delay)

        self.inventory[item.name] -= item.quantity
        if self.inventory[item.name] <= 0:
            del self.inventory[item.name]


async def main():
    parser = argparse.ArgumentParser(description="Inventory client")
    parser.add_argument(
        "-d",
        "--domain",
        type=int,
        default=0,
        help="DDS domain ID (default: 0)",
    )

    parser.add_argument(
        "-t",
        "--delay",
        type=int,
        default=0,
        help="Delay in seconds for the add and remove operations (default: 0)",
    )

    args = parser.parse_args()

    participant = dds.DomainParticipant(args.domain)
    service = rpc.Service(InventoryImpl(args.delay), participant, "Inventory")

    await service.run()


if __name__ == "__main__":
    dds.Logger.instance.verbosity = dds.Verbosity.WARNING
    try:
        # Run the service until Ctrl-C is pressed
        rti.asyncio.run(main())
    except KeyboardInterrupt:
        print("Shutting down...")
