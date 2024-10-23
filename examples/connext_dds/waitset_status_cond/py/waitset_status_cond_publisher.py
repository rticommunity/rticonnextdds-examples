# (c) Copyright, Real-Time Innovations, 2024.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import time
import argparse

import rti.connextdds as dds

from foo import Foo


def publisher_main(domain_id, sample_count):
    # Create a DomainParticipant with default QoS
    participant = dds.DomainParticipant(domain_id)

    # Create a Topic and automatically register the type
    topic = dds.Topic(participant, "Example waitset_status_cond", Foo)

    # Create a DataWriter with default QoS (Publisher created in-line)
    writer = dds.DataWriter(dds.Publisher(participant), topic)

    # Instantiate a sample
    sample = Foo()

    # Write every second until the specified amount of samples is reached
    count = 0
    while (sample_count == 0) or (count < sample_count):
        # Catch control-C interrupt
        try:
            print(f"Writing Foo, count = {count}")
            sample.x = count
            writer.write(sample)
            count += 1

            time.sleep(1)
        except KeyboardInterrupt:
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Waitsets with Status Conditions (Publisher)"
    )
    parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID")
    parser.add_argument(
        "-c", "--count", type=int, default=0, help="Number of samples to send"
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    publisher_main(args.domain, args.count)
