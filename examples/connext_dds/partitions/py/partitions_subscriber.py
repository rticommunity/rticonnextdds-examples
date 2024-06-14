# (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import rti.asyncio  # required by take_data_async()
import rti.connextdds as dds
from home_automation import DeviceStatus


async def sensor_monitoring():
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    reader = dds.DataReader(topic)

    async for data, info in reader.take_async():
        if info.valid:
            if data.is_open:
                pub_data = reader.matched_publication_data(info.publication_handle)
                print(f"WARNING: {data.sensor_name} in {data.room_name} is open!")
                print(
                    f"Received from publisher with partition(s): {pub_data.partition.name}"
                )


if __name__ == "__main__":
    try:
        rti.asyncio.run(sensor_monitoring())
    except KeyboardInterrupt:
        pass
