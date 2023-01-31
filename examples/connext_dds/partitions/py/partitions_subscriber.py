# (c) Copyright, Real-Time Innovations, 2022.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import sys
import rti.connextdds as dds
import rti.asyncio

from partitions import PartitionsExample


class PartitionsExampleSubscriber:
    def __init__(self, domain_id: int):
        # Start communicating in a domain. Usually there is one participant
        # per application.
        self.participant = dds.DomainParticipant(domain_id)

        # A Topic has a name and a datatype.
        topic = dds.Topic(
            self.participant, "Example partitions", PartitionsExample
        )

        # Create a Subscriber, loading QoS profile from USER_QOS_PROFILES.xml
        subscriber = dds.Subscriber(self.participant)

        # Create a DataReader, loading QoS profile from USER_QOS_PROFILES.xml,
        # We will take the data asynchronously in the run() method.
        self.reader = dds.DataReader(subscriber, topic)

        self.samples_read = 0

    async def run(self, sample_count: int):
        async for data, info in self.reader.take_async():
            if info.valid:

                if info.state.view_state == dds.ViewState.NEW_VIEW:
                    # When we receive a new instance because a partition
                    # update made it possible, the View state is ViewState.NEW_VIEW
                    print("Found new instance")

                print(f"Received: {data}")
                self.samples_read += 1
                if self.samples_read >= sample_count:
                    break


if __name__ == "__main__":
    try:
        rti.asyncio.run(PartitionsExampleSubscriber(0).run(sys.maxsize))
    except KeyboardInterrupt:
        pass
