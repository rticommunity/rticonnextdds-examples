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

from time import sleep

import rti.connextdds as dds
from home_automation import DeviceStatus

if __name__ == "__main__":
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    reader = dds.DataReader(topic)

    while True:
        sleep(4)
        disposed_instances = {
            reader.key_value(info.instance_handle).sensor_name
            for _, info in reader.select()
            .state(dds.InstanceState.NOT_ALIVE_DISPOSED)
            .read()
        }

        print(
            f"Disposed instances: {disposed_instances if len(disposed_instances) > 0 else 'None'}"
        )
