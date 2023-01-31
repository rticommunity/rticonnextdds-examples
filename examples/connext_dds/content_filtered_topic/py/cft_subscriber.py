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
import textwrap


# Process incoming data in a listener, print out each sample
class CftListener(dds.NoOpDataReaderListener):
    def on_data_available(self, reader):
        for sample in reader.take_data():
            print(sample)


def subscriber_main(domain_id, sample_count, is_cft):
    participant = dds.DomainParticipant(domain_id)

    topic = dds.Topic(participant, "Example cft", CftExample)

    if is_cft:
        # Create a CFT that filters for incoming data within a range
        topic = dds.ContentFilteredTopic(
            topic,
            "ContentFilteredTopic",
            dds.Filter("x >= %0 AND x <= %1", ["1", "4"]),
        )
        print(
            textwrap.dedent(
                """
            ==========================
            Using CFT
            Filter: 1 <= x <= 4
            =========================="""
            )
        )
    else:
        # Filtering disabled by default
        print(
            textwrap.dedent(
                """
            ==========================
            Using Normal Topic
            =========================="""
            )
        )

    reader = dds.DataReader(dds.Subscriber(participant), topic)
    reader.set_listener(CftListener(), dds.StatusMask.DATA_AVAILABLE)

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if is_cft:
            if count == 10:
                # After 10 seconds, udpdate the filter range
                print(
                    textwrap.dedent(
                        """
                    ==========================
                    Changing Filter Parameters
                    Filter: 5 <= x <= 9
                    =========================="""
                    )
                )
                topic.filter_parameters = ["5", "9"]
            if count == 20:
                # After 20 seconds, update the filter again
                print(
                    textwrap.dedent(
                        """
                    ==========================
                    Changing Filter Parameters
                    Filter: 3 <= x <= 9
                    =========================="""
                    )
                )
                topic.filter_parameters = ["3", "9"]
        count += 1


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Content-Filtered Topics (Subscriber)"
    )
    parser.add_argument(
        "-d", "--domain", type=int, default=0, help="DDS Domain ID"
    )
    parser.add_argument(
        "-c",
        "--count",
        type=int,
        default=0,
        help="Number of samples to receive before shutting down",
    )
    parser.add_argument(
        "-n",
        "--normal-topic",
        action="store_true",
        default=False,
        help="Use a normal Topic instead of a Content-Filtered Topic",
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    try:
        subscriber_main(args.domain, args.count, not args.normal_topic)
    except KeyboardInterrupt:
        pass
