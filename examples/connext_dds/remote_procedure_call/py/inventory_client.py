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
from asyncio import sleep
import rti.connextdds as dds
import rti.rpc as rpc
import rti.asyncio

from Inventory import Item, InventoryService, UnknownItemError

class InventoryClient(InventoryService, rpc.ClientBase):
    ...

async def run_client(args):
    participant = dds.DomainParticipant(args.domain)
    client = InventoryClient(participant, "Inventory")

    print("Initial inventory: ", await client.get_inventory())

    if args.add:
        print(f"Add {args.quantity} {args.add}")
        await client.add_item(Item(args.add, args.quantity))
    elif args.remove:
        print(f"Remove {args.quantity} {args.remove}")
        try:
            await client.remove_item(Item(args.remove, args.quantity))
        except UnknownItemError as e:
            print("Unknown item: " + e.name)
    else:
        raise ValueError("Must specify --add or --remove")

    print("Updated inventory: ", await client.get_inventory())


def main():
    parser = argparse.ArgumentParser(description="Inventory client")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        "-r", "--remove", type=str, help="Remove item from inventory"
    )
    group.add_argument("-a", "--add", type=str, help="Add item to inventory")

    parser.add_argument(
        "-q",
        "--quantity",
        type=int,
        default=1,
        help="Quantity of item to add or remove (default: 1)",
    )
    parser.add_argument(
        "-d",
        "--domain",
        type=int,
        default=0,
        help="DDS domain ID (default: 0)",
    )
    args = parser.parse_args()

    rti.asyncio.run(run_client(args))


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Shutting down...")
