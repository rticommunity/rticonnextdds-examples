# (c) Copyright, Real-Time Innovations, 2023.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.


# This example uses a WaitSet to wait for data that matches the query.
# See async_take_query_condition.py for an equivalent example that uses
# async operations (take_data_async).

import textwrap
import time
import argparse

import rti.connextdds as dds

from waitset_query_cond import waitset_query_cond


def query_handler(reader, query_cond):
    for data in reader.select().condition(query_cond).take_data():
        print(data)


def subscriber_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    topic = dds.Topic(
        participant, "Example waitset_query_cond", waitset_query_cond
    )
    reader_qos = dds.QosProvider.default.datareader_qos
    reader = dds.DataReader(dds.Subscriber(participant), topic, reader_qos)

    # Query against even samples at the start
    query_parameters = ["'EVEN'"]
    query = dds.Query(reader, "name MATCH %0", query_parameters)
    query_condition = dds.QueryCondition(query, dds.DataState.any_data)

    # Create a WaitSet and attach the QueryCondition
    waitset = dds.WaitSet()
    waitset += query_condition

    print(
        textwrap.dedent(
            """\
        >>> Timeout: {} sec
        >>> Query conditions: {}
        \t %0 = {}
        ----------------------------------

        """.format(
                1, query_condition.expression, query_parameters[0]
            )
        )
    )

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if count == 7:
            # Update the query after 7 seconds
            query_parameters[0] = "'ODD'"
            query_condition.parameters = query_parameters
            print(
                textwrap.dedent(
                    """
                CHANGING THE QUERY CONDITION
                >>> Query conditions:
                {}
                \t %%0 = {}
                >>> We keep one sample in the history
                -------------------------------------
                """.format(
                        query_condition.expression, query_parameters[0]
                    )
                )
            )

        active = waitset.wait((1, 0))

        # Check to see if the query was triggered
        if query_condition in active:
            query_handler(reader, query_condition)

        count += 1


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Waitsets with Query Conditions (Subscriber)"
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

    subscriber_main(args.domain, args.count)
