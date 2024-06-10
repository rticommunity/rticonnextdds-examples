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


class WindowSensor:

    def __init__(self, sensor_name: str, room_name: str) -> None:
        self.status = DeviceStatus(sensor_name, room_name, is_open=False)

        participant = dds.DomainParticipant(domain_id=0)
        topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
        self.writer = dds.DataWriter(topic)

        self.instance = self.writer.register_instance(self.status)
        self.writer.write(self.status, self.instance)

    def open_window(self) -> None:
        print("Opening the window...")
        self.status.is_open = True
        self.writer.write(self.status, self.instance)

    def close_window(self) -> None:
        print("Closing the window...")
        self.status.is_open = False
        self.writer.write(self.status, self.instance)

    def turn_off(self) -> None:
        print("Turning off the sensor...")
        self.writer.dispose_instance(self.instance)

    def turn_on(self) -> None:
        print("Turning on the sensor...")
        self.writer.write(self.status, self.instance)


def control_sensor(sensor_name: str, room_name: str):

    window_sensor = WindowSensor(sensor_name, room_name)
    help_message = """\nEnter one of the following options:
        - open
        - close
        - off
        - on
        - exit\n"""

    while True:
        value = input(help_message).lower()

        if value == "open":
            window_sensor.open_window()
        elif value == "close":
            window_sensor.close_window()
        elif value == "off":
            window_sensor.turn_off()
        elif value == "on":
            window_sensor.turn_on()
        elif value == "exit":
            break
        else:
            print(
                "Invalid option, please enter a valid option [open|close|off|on|exit]."
            )


if __name__ == "__main__":
    sensor_name = sys.argv[1] if len(sys.argv) > 1 else "Window1"
    room_name = sys.argv[2] if len(sys.argv) > 2 else "LivingRoom"

    try:
        control_sensor(sensor_name, room_name)
    except KeyboardInterrupt:
        pass
