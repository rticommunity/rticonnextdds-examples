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

import rti.connextdds as dds
import rti.asyncio  # required by take_data_async()
from temperature import Temperature


async def sensor_monitoring():
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "Temperature", Temperature)
    reader = dds.DataReader(topic)

    async for data, info in reader.take_async():
        if not info.valid:
            continue  # skip updates with only meta-data

        timestamp = datetime.fromtimestamp(info.source_timestamp.to_seconds())
        print(
            f"{data.sensor_name}: {data.degrees:.2f} degrees ({timestamp})"
        )


if __name__ == "__main__":
    try:
        rti.asyncio.run(sensor_monitoring())
    except KeyboardInterrupt:
        pass
