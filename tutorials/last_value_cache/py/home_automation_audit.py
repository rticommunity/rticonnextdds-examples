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

import rti.asyncio  # required by take_data_async()
import rti.connextdds as dds
from home_automation import DeviceStatus

async def sensor_monitoring():
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    reader = dds.DataReader(
        topic,
        qos=dds.QosProvider.default.datareader_qos_from_profile("Last10Cache"))

    async for data, info in reader.take_async():
        if not info.valid:
            continue

        timestamp = datetime.fromtimestamp(info.source_timestamp.to_seconds())
        print(f"{timestamp} - {data}")

if __name__ == '__main__':
    try:
        rti.asyncio.run(sensor_monitoring())
    except KeyboardInterrupt:
        pass
