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
import rti.asyncio

import rti.connextdds as dds

from async_type import async_type


async def process_data(reader: dds.DataReader):
    # Print data as it arrives, suspending the coroutine until data is
    # available.
    async for data in reader.take_data_async():
        print(f"Received: {data}")


def run_subscriber_application(
    domain_id: int, sample_count: int, use_xml_qos: bool = False
):
    # Start communicating in a domain, usually one participant per application
    participant = dds.DomainParticipant(domain_id)

    # Create a Topic
    topic = dds.Topic(participant, "Example async", async_type)

    if use_xml_qos:
        # Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
        reader_qos = dds.QosProvider.default.datareader_qos
    else:
        # Configure the DataReaderQos in code, to the same effect as the XML file.
        reader_qos = participant.default_datareader_qos
        reader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
        reader_qos.history.kind = dds.HistoryKind.KEEP_ALL

    # Create a DataReader with the QoS in our profile or configured programatically
    reader = dds.DataReader(participant.implicit_subscriber, topic, reader_qos)

    try:
        print("async subscriber sleeping")
        rti.asyncio.run(process_data(reader))
        # await process_data(reader)
    except KeyboardInterrupt:
        pass

    print("preparing to shut down...")


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
        # rti.asyncio.run(run_subscriber_application(args.domain, args.count, args.qos))
        run_subscriber_application(args.domain, args.count, args.qos)
    except Exception as e:
        print(f"Exception in run_subscriber_application(): {e}")
    return


if __name__ == "__main__":
    main()
