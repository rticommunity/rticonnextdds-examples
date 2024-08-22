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

import rti.asyncio  # required by take_data_async()
import rti.connextdds as dds
from home_automation import DeviceStatus
from rti.types.builtin import KeyedString


async def sensor_monitoring():
    participant = dds.DomainParticipant(domain_id=0)

    status_topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    status_reader = dds.DataReader(status_topic)

    alert_topic = dds.Topic(participant, "HomeAlerts", KeyedString)
    alert_writer = dds.DataWriter(alert_topic)

    async for data, info in status_reader.take_async():
        if info.valid and data.is_open:
            alert = KeyedString(
                key=data.sensor_name,
                value=f"Window in {data.room_name} was just opened",
            )
            alert_writer.write(alert, timestamp=info.source_timestamp)


if __name__ == "__main__":
    rti.asyncio.run(sensor_monitoring())
