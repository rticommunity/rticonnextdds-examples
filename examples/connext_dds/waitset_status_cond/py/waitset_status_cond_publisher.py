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


def publisher_main(domain_id, sample_count):
    # Create a DomainParticipant with default QoS
    participant = dds.DomainParticipant(domain_id)

    # Create a Topic and automatically register the type
    topic = dds.Topic(participant, "Example Foo", Foo)

    # Create a DataWriter with default QoS
    writer = dds.DataWriter(topic)

    # Create a Status Condition for the writer
    status_condition = dds.StatusCondition(writer)

    # Enable statuses configuration for the Status Condition
    status_condition.enabled_statuses = dds.StatusMask.PUBLICATION_MATCHED

    # Define a handler for the Status Condition
    def status_handler(_):
        st = writer.publication_matched_status
        print(f"Publication matched changed => Matched readers = {st.current_count}")

    status_condition.set_handler(status_handler)

    # Create a WaitSet and attach our Status Condition
    waitset = dds.WaitSet()
    waitset += status_condition

    # Instantiate a sample
    sample = Foo()

    # Write every second until the specified amount of samples is reached
    samples_sent = 0
    while (sample_count == 0) or (samples_sent < sample_count):
        # Catch control-C interrupt
        try:
            # Dispatch will call the handlers associated to the
            # WaitSet conditions when they activate
            waitset.dispatch(dds.Duration(1, 0))  # Wait up to 1s each time

            print(f"Writing Foo, count = {samples_sent}")
            sample.x = samples_sent
            writer.write(sample)
            samples_sent += 1
        except KeyboardInterrupt:
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Waitsets with Status Conditions (Publisher)"
    )
    parser.add_argument(
        "-d", "--domain", type=int, default=0, help="DDS Domain ID"
    )
    parser.add_argument(
        "-c", "--count", type=int, default=0, help="Number of samples to send"
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    publisher_main(args.domain, args.count)
