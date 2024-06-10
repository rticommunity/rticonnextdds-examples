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


def wait_for_input(query: str):
    input(f"\n\nPress Enter to run the query:\n\t{query}\n")


if __name__ == "__main__":
    participant = dds.DomainParticipant(domain_id=0)
    topic = dds.Topic(participant, "WindowStatus", DeviceStatus)
    reader = dds.DataReader(topic)

    print("3. Query the cache")
    sleep(1)

    # Display all the full cache
    wait_for_input("reader.read_data()")
    print(reader.read_data())

    # Display all the samples with the attribute is_open set to true
    wait_for_input(
        'reader.select().content(dds.Query(reader, "is_open = true")).read_data()'
    )
    print(reader.select().content(dds.Query(reader, "is_open = true")).read_data())

    # Display all the samples with the attribute sensor_name set to Window1
    wait_for_input(
        "reader.select().content(dds.Query(reader, \"sensor_name = 'Window1'\")).read_data()"
    )
    print(
        reader.select()
        .content(dds.Query(reader, "sensor_name = 'Window1'"))
        .read_data()
    )

    # Display all the samples with the attribute room_name set to LivingRoom
    wait_for_input(
        "reader.select().content(dds.Query(reader, \"room_name = 'LivingRoom'\")).read_data()"
    )
    print(
        reader.select()
        .content(dds.Query(reader, "room_name = 'LivingRoom'"))
        .read_data()
    )

    # Display all the samples of the instance with the attribute sensor_name set to Window1
    wait_for_input(
        'window1_handle = reader.lookup_instance(DeviceStatus(sensor_name="Window1"))\n'
        + "\treader.select().instance(window1_handle).read_data()"
    )
    window1_handle = reader.lookup_instance(DeviceStatus(sensor_name="Window1"))
    print(reader.select().instance(window1_handle).read_data())

    # Display all the samples that you have not read yet
    wait_for_input("reader.select().state(dds.SampleState.NOT_READ).read_data()")
    print(reader.select().state(dds.SampleState.NOT_READ).read_data())

    # Display all the samples that you have not read yet
    wait_for_input("reader.select().state(dds.SampleState.NOT_READ).read_data()")
    print(reader.select().state(dds.SampleState.NOT_READ).read_data())

    # Display all the new instances
    wait_for_input("reader.select().state(dds.ViewState.NEW_VIEW).read_data()")
    print(reader.select().state(dds.ViewState.NEW_VIEW).read_data())

    print("4. Track instance lifecycle")
    sleep(1)

    # Query the cache again using read instead of read_data to get the SampleInfo
    wait_for_input("reader.read()")
    print(reader.read())

    # Display the sensor names of the instances that are not alive
    wait_for_input(
        """{
            reader.key_value(info.instance_handle).sensor_name
            for _, info in reader.select().state(dds.InstanceState.NOT_ALIVE_MASK).read()
        }"""
    )
    print(
        {
            reader.key_value(info.instance_handle).sensor_name
            for _, info in reader.select()
            .state(dds.InstanceState.NOT_ALIVE_MASK)
            .read()
        }
    )
