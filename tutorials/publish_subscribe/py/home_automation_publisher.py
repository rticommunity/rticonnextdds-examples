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
from time import sleep

import rti.connextdds as dds
from home_automation import DeviceStatus


# This publisher simulates a window that opens and closes over time on the
# WindowStatus topic.
def publish_sensor(sensor_name: str, room_name: str):

    # Create a DomainParticipant within a domain ID. A domain ID is a logical
    # partition for your applications; the DomainParticipant joins that domain
    # and contains all other entities.
    participant = dds.DomainParticipant(domain_id=0)

    # The data shared by a publisher and subscriber is described by a Topic.
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)

    # Create a DataWriter to publish the WindowStatus topic.
    writer = dds.DataWriter(topic)

    device_status = DeviceStatus(sensor_name, room_name, is_open=False)

    # Publish an update every two seconds, simulating the window opening and
    # closing repeatedly.
    for i in range(1000):
        device_status.is_open = not device_status.is_open
        writer.write(device_status)
        sleep(2)


if __name__ == "__main__":
    sensor_name = sys.argv[1] if len(sys.argv) > 1 else "Window1"
    room_name = sys.argv[2] if len(sys.argv) > 2 else "LivingRoom"
    try:
        publish_sensor(sensor_name, room_name)
    except KeyboardInterrupt:
        pass
