# (c) Copyright, Real-Time Innovations, 2024.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import argparse

import rti.connextdds as dds

from foo import Foo


def subscriber_main(domain_id, sample_count):
    # Create a DomainParticipant with default QoS
    participant = dds.DomainParticipant(domain_id)

    # Create a Topic and automatically register the type
    topic = dds.Topic(participant, "Example waitset_status_cond", Foo)

    # Create a DataReader with default QoS (Subscriber created in-line)
    reader = dds.DataReader(dds.Subscriber(participant), topic)

    # Create a Status Condition for the reader
    status_condition = dds.StatusCondition(reader)

    # Enable statuses configuration for the Status Condition
    status_condition.enabled_statuses = (
        dds.StatusMask.LIVELINESS_CHANGED | dds.StatusMask.DATA_AVAILABLE
    )

    # This counter will keep track of the samples we have processed
    samples_read = 0

    # Define a handler for the status_condition
    def status_handler(_):
        status_mask = reader.status_changes

        # Handle liveliness status changes
        if dds.StatusMask.LIVELINESS_CHANGED in status_mask:
            st = reader.liveliness_changed_status
            print(f"Liveliness changed => Active writers = {st.alive_count}")

        # Handle when there's new data available
        if dds.StatusMask.DATA_AVAILABLE in status_mask:
            nonlocal samples_read
            for data, info in reader.take():
                if info.valid:
                    samples_read += 1
                    print(data)

    status_condition.set_handler(status_handler)

    # Create a WaitSet and attach our Status Condition
    waitset = dds.WaitSet()
    waitset += status_condition

    # Loop until the application is shut down or the sample count is reached
    while (sample_count == 0) or (samples_read < sample_count):
        # Catch control-C interrupt
        try:
            # Dispatch will call the handlers associated to the
            # WaitSet conditions when they activate
            waitset.dispatch(dds.Duration(4, 0))  # Wait up to 4s each time
        except KeyboardInterrupt:
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Waitsets with Status Conditions (Subscriber)"
    )
    parser.add_argument(
        "-d", "--domain", type=int, default=0, help="DDS Domain ID"
    )
    parser.add_argument(
        "-c",
        "--count",
        type=int,
        default=0,
        help="Number of samples to receive",
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    subscriber_main(args.domain, args.count)
