# (c) Copyright, Real-Time Innovations, 2023.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

import asyncio
import sys
import rti.connextdds as dds
import rti.asyncio

from profiles import ProfilesExample


class ProfilesExampleSubscriber:
    def __init__(self, domain_id: int):
        # Retrieve QoS from custom profile XML and USER_QOS_PROFILES.xml
        qos_provider = dds.QosProvider("my_custom_qos_profiles.xml")

        # Create a DomainParticipant with the default QoS of the provider.
        self.participant = dds.DomainParticipant(
            domain_id, qos_provider.participant_qos
        )

        # Create a Subscriber with default QoS.
        self.subscriber = dds.Subscriber(
            self.participant, qos_provider.subscriber_qos
        )

        # Create a Topic with default QoS.
        self.topic = dds.Topic(
            self.participant,
            "Example profiles",
            ProfilesExample,
            qos_provider.topic_qos,
        )

        # Create a DataReader with the QoS profile "transient_local_profile",
        # which is in the QoS library "profiles_Library".
        self.reader_transient_local = dds.DataReader(
            self.subscriber,
            self.topic,
            qos_provider.datareader_qos_from_profile(
                "profiles_Library::transient_local_profile"
            ),
        )

        # Create a DataReader with the QoS profile "volatile_profile", which is
        # in the QoS library "profiles_Library".
        self.reader_volatile = dds.DataReader(
            self.subscriber,
            self.topic,
            qos_provider.datareader_qos_from_profile(
                "profiles_Library::volatile_profile"
            ),
        )

        self.samples_read = 0

    async def run_reader(self, sample_count: int, reader, profile_name):
        async for data in reader.take_data_async():
            print(f"- Received data from {profile_name}: {data}")
            self.samples_read += 1
            if self.samples_read >= sample_count:
                break

    async def run(self, sample_count: int):
        # Receive data from both readers in parallel
        await asyncio.gather(
            self.run_reader(
                sample_count,
                self.reader_transient_local,
                "transient_local_profile",
            ),
            self.run_reader(
                sample_count, self.reader_volatile, "volatile_profile"
            ),
        )


if __name__ == "__main__":
    try:
        rti.asyncio.run(ProfilesExampleSubscriber(0).run(sys.maxsize))
    except KeyboardInterrupt:
        pass
