# (c) Copyright, Real-Time Innovations, 2023.  All rights reserved.
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

from waitset_query_cond import waitset_query_cond


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    topic = dds.Topic(
        participant, "Example waitset_query_cond", waitset_query_cond)
    writer = dds.DataWriter(dds.Publisher(participant), topic)

    instance = waitset_query_cond()

    count = 0
    while (sample_count == 0) or (count < sample_count):
        print("Writing waitset_query_cond, count = {}".format(count))
        instance.x = count
        instance.name = "ODD" if count % 2 == 1 else "EVEN"

        writer.write(instance)
        count += 1
        time.sleep(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Waitsets with Query Conditions (Publisher)"
    )
    parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID")
    parser.add_argument(
        "-c", "--count", type=int, default=0, help="Number of samples to send"
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    publisher_main(args.domain, args.count)
