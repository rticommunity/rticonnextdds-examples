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


async def sensor_monitoring():
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    content_filtered_topic = dds.ContentFilteredTopic(
        topic,
        "FilterRoomAndOpenWindows",
        dds.Filter("is_open = true and room_name = %0", parameters=["'LivingRoom'"]),
    )
    reader = dds.DataReader(content_filtered_topic)

    # Update the filter to monitor the kitchen
    content_filtered_topic.filter_parameters = ["'Kitchen'"]

    async for data in reader.take_data_async():
        print(f"WARNING: {data.sensor_name} in {data.room_name} is open!")


if __name__ == "__main__":
    try:
        rti.asyncio.run(sensor_monitoring())
    except KeyboardInterrupt:
        pass
