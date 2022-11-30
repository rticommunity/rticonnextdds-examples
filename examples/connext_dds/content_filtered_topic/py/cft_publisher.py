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

import rti.connextdds as dds
from test_type import CftExample
import time
import argparse


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    topic = dds.Topic(participant, "Example cft", CftExample)
    writer = dds.DataWriter(participant.implicit_publisher, topic)

    sample = CftExample()

    # Increment count every time we send a sample and reset x to 0 every time we
    # send 10 samples(x=0, 1, .., 9).
    count = 0
    while sample_count == 0 or count < sample_count:
        sample.count = count
        sample.x = count % 10
        print(f"Writing cft, {sample}")
        writer.write(sample)
        time.sleep(1)
        count += 1


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Content-Filtered Topics (Publisher)"
    )
    parser.add_argument(
        "-d", "--domain", type=int, default=0, help="DDS Domain ID"
    )
    parser.add_argument(
        "-c",
        "--count",
        type=int,
        default=0,
        help="Number of samples to send before shutting down",
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    try:
        publisher_main(args.domain, args.count)
    except KeyboardInterrupt:
        pass

