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
import textwrap
from test_type import CftExample

# Process data with a listener
class CftListener(dds.NoOpDataReaderListener):
    def on_data_available(self, reader):
        for data in reader.take_data():
            print(data)


def subscriber_main(domain_id, sample_count, is_cft):
    participant = dds.DomainParticipant(domain_id)

    topic = dds.Topic(participant, "Example cft", CftExample)

    if is_cft:
        # Use a stringmatch CFT
        str_filter = dds.Filter("name MATCH %0", ["SOME_STRING"])
        str_filter.name = dds.Filter.STRINGMATCH_FILTER_NAME
        topic = dds.ContentFilteredTopic(
            topic, "ContentFilteredTopic", str_filter
        )
        # Initial filter is a simple name match
        print(
            textwrap.dedent(
                """
            ==========================
            Using ContentFilteredTopic
            name MATCH %0
            =========================="""
            )
        )
    else:
        # Default topic does not use a CFT
        print(
            textwrap.dedent(
                """
            ==========================
            Using Normal Topic
            =========================="""
            )
        )

    reader_qos = dds.QosProvider.default.datareader_qos
    reader = dds.DataReader(dds.Subscriber(participant), topic, reader_qos)
    reader.set_listener(CftListener(), dds.StatusMask.DATA_AVAILABLE)

    # Change the filter
    if is_cft:
        print('Now setting a new filter: name MATCH "EVEN"')
        topic.append_to_expression_parameter(0, "EVEN")

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if is_cft:
            if count == 10:
                # Change the filter again after 10 seconds
                print(
                    textwrap.dedent(
                        """
                    ==========================
                    Changing Filter Parameters
                    Append 'ODD' filter
                    =========================="""
                    )
                )
                topic.append_to_expression_parameter(0, "ODD")
            if count == 20:
                # Change the filter one more time after 20 seconds
                print(
                    textwrap.dedent(
                        """
                    ==========================
                    Changing Filter Parameters
                    Remove 'EVEN' filter
                    =========================="""
                    )
                )
                topic.remove_from_expression_parameter(0, "EVEN")
        count += 1


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using String Filters (Subscriber)"
    )
    parser.add_argument(
        "-d", "--domain", type=int, default=0, help="DDS Domain ID"
    )
    parser.add_argument(
        "-c", "--count", type=int, default=0, help="Number of samples to send"
    )
    parser.add_argument(
        "-f", "--filter", action="store_true", default=False, help="Use a CFT"
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    subscriber_main(args.domain, args.count, args.filter)
