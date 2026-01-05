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

from datetime import datetime

import rti.asyncio  # required by take_async()
import rti.connextdds as dds
from home_automation import DeviceStatus


async def sensor_monitoring():

    # Create a DomainParticipant and WindowStatus topic using the same domain
    # ID and name as the publisher.
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)

    # Create a DataReader to receive the WindowStatus data.
    reader = dds.DataReader(topic)

    # Take each update with its metadata so we can include the source timestamp
    # when the window was opened.
    print("Subscriber started. Waiting for WindowStatus updates...")
    async for data, info in reader.take_async():
        if not info.valid:
            continue  # skip updates with only meta-data

        if data.is_open:
            # Include the original source timestamp of the WindowStatus update
            # in the alert output.
            timestamp = datetime.fromtimestamp(
                info.source_timestamp.to_seconds()
            )
            print(
                f"WARNING: {data.sensor_name} in {data.room_name} is open ({timestamp})"
            )


if __name__ == "__main__":
    try:
        rti.asyncio.run(sensor_monitoring())
    except KeyboardInterrupt:
        pass
