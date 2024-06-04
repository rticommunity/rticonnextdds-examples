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
import random
from time import sleep

import rti.connextdds as dds
from temperature import Temperature

def publish_temperature(sensor_name: str):
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "Temperature", Temperature)
    writer = dds.DataWriter(topic)

    temp_reading = Temperature(sensor_name)
    for _ in range(1000):
        temp_reading.degrees = random.uniform(30, 40)
        writer.write(temp_reading)
        sleep(1)


if __name__ == "__main__":
    sensor_name = sys.argv[1] if len(sys.argv) > 1 else "Sensor1"
    try:
        publish_temperature(sensor_name)
    except KeyboardInterrupt:
        pass
