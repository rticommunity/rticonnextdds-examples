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

import sys
from datetime import datetime

import rti.connextdds as dds
import rti.asyncio  # required by take_data_async()
from temperature import Temperature


async def sensor_monitoring(subscriber_name: str):
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "Temperature", Temperature)

    reader_qos = dds.QosProvider.default.datareader_qos_from_profile(
        "MyLibrary::Persistence"
    )
    reader_qos.durability.storage_settings.enable = True
    reader_qos.durability.storage_settings.file_name = subscriber_name

    reader = dds.DataReader(topic, reader_qos)

    async for data, info in reader.take_async():
        if not info.valid:
            continue  # skip updates with only meta-data

        timestamp = datetime.fromtimestamp(info.source_timestamp.to_seconds())
        print(
            f"{data.sensor_name}: {data.degrees:.2f} degrees ({timestamp})"
        )


if __name__ == "__main__":
    subscriber_name = sys.argv[1] if len(sys.argv) > 1 else "subscriber1"
    try:
        rti.asyncio.run(sensor_monitoring(subscriber_name))
    except KeyboardInterrupt:
        pass
