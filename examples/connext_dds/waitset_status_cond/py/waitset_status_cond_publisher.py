
# (c) Copyright, Real-Time Innovations, 2024.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import sys
import asyncio

import rti.connextdds as dds
import rti.asyncio

from waitset_status_cond import waitset_status_cond

class waitset_status_condPublisher:

    def __init__(self, domain_id: int):
        # Start communicating in a domain. Usually there is one participant
        # per application. Load a QoS profile from USER_QOS_PROFILES.xml
        participant_qos = dds.QosProvider.default.participant_qos_from_profile(
            "waitset_status_cond_Library::waitset_status_cond_Profile")
        self.participant = dds.DomainParticipant(domain_id, participant_qos)

        # A Topic has a name and a datatype.
        topic = dds.Topic(self.participant, "Example waitset_status_cond", waitset_status_cond)

        # Create a Publisher, loading QoS profile from USER_QOS_PROFILES.xml
        publisher_qos = dds.QosProvider.default.publisher_qos_from_profile(
            "waitset_status_cond_Library::waitset_status_cond_Profile")
        publisher = dds.Publisher(self.participant, publisher_qos)

        # Create a DataWriter, loading QoS profile from USER_QOS_PROFILES.xml
        writer_qos = dds.QosProvider.default.datawriter_qos_from_profile(
            "waitset_status_cond_Library::waitset_status_cond_Profile")
        self.writer = dds.DataWriter(
            publisher,
            topic,
            writer_qos)

        self.samples_written = 0

    async def run(self, sample_count: int):
        sample = waitset_status_cond()
        while self.samples_written < sample_count:
            # Modify the data to be sent here
            sample.x = self.samples_written

            print(f"Writing waitset_status_cond, count {self.samples_written}")
            self.writer.write(sample)

            self.samples_written += 1
            await asyncio.sleep(.5)


if __name__ == "__main__":
    try:
        rti.asyncio.run(waitset_status_condPublisher(0).run(sys.maxsize))
    except KeyboardInterrupt:
        pass
