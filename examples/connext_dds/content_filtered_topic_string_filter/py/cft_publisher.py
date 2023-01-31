#
# (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import rti.connextdds as dds
import time
import argparse
from test_type import CftExample


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    topic = dds.Topic(participant, "Example cft", CftExample)

    writer_qos = dds.QosProvider.default.datawriter_qos
    writer = dds.DataWriter(dds.Publisher(participant), topic, writer_qos)

    sample = CftExample()
    count = 0
    while (sample_count == 0) or (count < sample_count):
        print(f"Writing CftExample, count={count}")
        sample.count = count
        sample.name = "ODD" if count % 2 == 1 else "EVEN"
        writer.write(sample)
        time.sleep(1)
        count += 1


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using String Filters (Publisher)"
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
