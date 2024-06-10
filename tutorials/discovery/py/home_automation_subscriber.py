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


class SensorListener(dds.NoOpDataReaderListener):
    def on_subscription_matched(self, reader, matched_status):
        print(
            f"\nTotal publishers: {matched_status.current_count}, "
            + f"Change: {matched_status.current_count_change}"
        )

        for i, publication in enumerate(reader.matched_publications):
            pub_data = reader.matched_publication_data(publication)
            print(f"Publisher {i}:")
            print(f"    Publisher Virtual GUID: {pub_data.virtual_guid}")
            print(f"    Publisher Name: {pub_data.publication_name.name}")


async def sensor_monitoring():
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    reader = dds.DataReader(topic)

    reader.set_listener(SensorListener(), dds.StatusMask.SUBSCRIPTION_MATCHED)

    async for data in reader.take_data_async():
        if data.is_open:
            print(f"WARNING: {data.sensor_name} in {data.room_name} is open!")


if __name__ == "__main__":
    try:
        rti.asyncio.run(sensor_monitoring())
    except KeyboardInterrupt:
        pass
