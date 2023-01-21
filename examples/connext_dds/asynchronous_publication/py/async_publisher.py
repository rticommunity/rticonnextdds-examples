#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#
import argparse
import time

import rti.connextdds as dds

# Note: cannot do 'from async' because async is a keyword since Python 3.5
# therefore I have to update the name of async.idl to async_type.idl
from async_type import async_type


def run_publisher_application(
    domain_id: int, sample_count: int, use_xml_qos: bool = False
):
    # Start communicating in a domain, usually one participant per application
    participant = dds.DomainParticipant(domain_id)

    # Create a Topic
    topic = dds.Topic(participant, "Example async", async_type)

    if use_xml_qos:
        # Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
        writer_qos = dds.QosProvider.default.datawriter_qos
    else:
        # Configure the DataWriterQos in code, to the same effect as the XML file.
        writer_qos = participant.default_datawriter_qos
        writer_qos.reliability = dds.Reliability.reliable(
            dds.Duration.from_seconds(60)
        )
        # Note: Admin Console is reporting history depth of 1 still regardless of changes in the QoS file or programatically
        # writer_qos.history. = dds.History.keep_last(12)
        writer_qos.history.kind = dds.HistoryKind.KEEP_LAST
        writer_qos.history.depth = 12
        writer_qos.data_writer_protocol.rtps_reliable_writer.min_send_window_size = (
            50
        )
        writer_qos.data_writer_protocol.rtps_reliable_writer.max_send_window_size = (
            50
        )

        # Note: Without specifying a name it defaults to DDS_DEFAULT_FLOW_CONTROLLER_NAME. Do I need to specify the name?
        writer_qos.publish_mode = dds.PublishMode.asynchronous(
            "DDS_FIXED_RATE_FLOW_CONTROLLER_NAME"
        )

    # Create a DataWriter with the QoS in our profile or configured programatically
    writer = dds.DataWriter(participant.implicit_publisher, topic, writer_qos)

    # Create data sample for writing
    sample = async_type(x=42)

    for count in range(sample_count):
        try:
            # Modify the data to be sent here
            sample.x = count
            print(f"Writing async_type, count {count}")
            writer.write(sample)
            time.sleep(1)
        except KeyboardInterrupt:
            break
    # You can wait until all written samples have been actually published
    # (note that this doesn't ensure that they have actually been received
    # if the sample required retransmission)
    writer.wait_for_asynchronous_publishing
    print("preparing to shut down...")
    participant.close()


def main():
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Asynchronous Publishing"
    )
    parser.add_argument(
        "-d",
        "--domain",
        type=int,
        default=0,
        help="DDS Domain ID | Range: 0-232 | Default: 0",
    )
    parser.add_argument(
        "-c",
        "--count",
        type=int,
        default=50,
        help="Number of samples to send | Default 50",
    )
    parser.add_argument(
        "-v",
        "--verbosity",
        type=int,
        default=1,
        help="How much debugging output to show | Range: 0-3 | Default: 1",
    )
    parser.add_argument(
        "-q",
        "--qos",
        action="store_true",
        default=False,
        help="Use QoS profile from USER_QOS_PROFILES.xml | Default: False",
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0
    assert 0 <= args.verbosity < 4

    verbosity_levels = {
        0: dds.Verbosity.SILENT,
        1: dds.Verbosity.EXCEPTION,
        2: dds.Verbosity.WARNING,
        3: dds.Verbosity.STATUS_ALL,
    }

    # Sets Connext verbosity to help debugging
    verbosity = verbosity_levels.get(args.verbosity, dds.Verbosity.EXCEPTION)

    dds.Logger.instance.verbosity = verbosity

    try:
        # Note: C# example doesn't seem to allow using the use_xml_qos option?
        run_publisher_application(args.domain, args.count, args.qos)
    except Exception as e:
        print(f"Exception in run_publisher_application(): {e}")
    return


if __name__ == "__main__":
    main()
