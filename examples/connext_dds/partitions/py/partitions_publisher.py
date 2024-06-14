# (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import sys
from time import sleep

import rti.connextdds as dds
from home_automation import DeviceStatus


def publish_sensor(sensor_name: str, room_name: str):
    participant = dds.DomainParticipant(domain_id=0)
    publisher = dds.Publisher(participant)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    writer = dds.DataWriter(publisher, topic)

    device_status = DeviceStatus(sensor_name, room_name, is_open=False)

    partition_names = None

    for i in range(1000):

        if i % 15 == 0:
            # Multiple partitions, with match
            partition_names = ["USA/CA/Sunnyvale", "USA/CA/San Francisco"]
        elif i % 15 == 5:
            # Wildcard match
            partition_names = ["USA/CA/*"]
        elif i % 15 == 10:
            # No match
            partition_names = ["USA/NV/Las Vegas"]

        if partition_names is not None:
            publisher_qos = publisher.qos
            publisher_qos.partition.name = partition_names
            publisher.qos = publisher_qos
            partition_names = None

        # Simulate the window opening and closing
        device_status.is_open = not device_status.is_open

        print(
            f"Writing {device_status} on partition(s) {list(publisher.qos.partition.name)}"
        )

        writer.write(device_status)
        sleep(2)


if __name__ == "__main__":
    sensor_name = sys.argv[1] if len(sys.argv) > 1 else "Window1"
    room_name = sys.argv[2] if len(sys.argv) > 2 else "LivingRoom"
    try:
        publish_sensor(sensor_name, room_name)
    except KeyboardInterrupt:
        pass
