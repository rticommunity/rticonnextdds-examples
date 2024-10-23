# (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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


def wait_for_writer(reader: dds.DynamicData.DataReader) -> None:
    """Wait for a DataWriter to be matched with the input DataReader"""

    print("Waiting for a matching DataWriter...")
    while len(reader.matched_publications) == 0:
        sleep(0.1)

    print("DataWriter found")


def wait_for_reader(writer: dds.DynamicData.DataWriter) -> None:
    """Wait for a DataReader to be matched with the input DataWriter"""

    print("Waiting for a matching DataReader...")
    while len(writer.matched_subscriptions) == 0:
        sleep(0.1)

    print("DataReader found")


def publish_example_data(domain_id: int, type: dds.DynamicType):
    """Publishes a few data samples as an example"""

    participant = dds.DomainParticipant(domain_id)
    topic = dds.DynamicData.Topic(participant, "Example Record", type)

    qos = dds.QosProvider.default.datawriter_qos_from_profile(
        dds.BuiltinProfiles.generic_strict_reliable
    )
    writer = dds.DynamicData.DataWriter(topic, qos)
    wait_for_reader(writer)

    sleep(1)

    # Write a few data samples
    data = writer.create_data()
    data["id"] = "Example"
    data["payload"] = dds.Int32Seq(range(10))
    writer.write(data)

    sleep(1)

    data["id"] = "Example2"
    data["payload"] = dds.Int32Seq([7, 11, 13, 17, 19])
    writer.write(data)

    sleep(1)

    data["id"] = "Example3"
    data["payload"] = dds.Int32Seq(range(100, 150, 5))
    writer.write(data)

    sleep(1)
