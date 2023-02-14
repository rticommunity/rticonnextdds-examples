# (c) Copyright, Real-Time Innovations, 2023.  All rights reserved.
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

from profiles import ProfilesExample


class ProfilesExamplePublisher:
    def __init__(self, domain_id: int):
        # Retrieve QoS from custom profile XML and USER_QOS_PROFILES.xml
        qos_provider = dds.QosProvider("my_custom_qos_profiles.xml")

        # Create a DomainParticipant with the default QoS of the provider.
        self.participant = dds.DomainParticipant(
            domain_id, qos_provider.participant_qos
        )

        # Create a Publisher with default QoS.
        publisher = dds.Publisher(self.participant, qos_provider.publisher_qos)

        # Create a Topic with default QoS.
        topic = dds.Topic(
            self.participant,
            "Example profiles",
            ProfilesExample,
            qos_provider.topic_qos,
        )

        # Create a DataWriter with the QoS profile "transient_local_profile",
        # from QoS library "profiles_Library".
        self.writer_transient_local = dds.DataWriter(
            publisher,
            topic,
            qos_provider.datawriter_qos_from_profile(
                "profiles_Library::transient_local_profile"
            ),
        )

        # Create a DataWriter with the QoS profile "volatile_profile",
        # from the QoS library "profiles_Library".
        self.writer_volatile = dds.DataWriter(
            publisher,
            topic,
            qos_provider.datawriter_qos_from_profile(
                "profiles_Library::volatile_profile"
            ),
        )

        self.samples_written = 0

    async def run(self, sample_count: int):
        sample = ProfilesExample()
        while self.samples_written < sample_count:
            # Update the counter value of the sample.
            sample.x = self.samples_written

            sample.profile_name = "volatile_profile"
            print(f"* Writing {sample}")
            self.writer_volatile.write(sample)

            # Send the sample using the DataWriter with "transient_local" durability.
            sample.profile_name = "transient_local_profile"
            print(f"* Writing {sample}")
            self.writer_transient_local.write(sample)

            self.samples_written += 1
            await asyncio.sleep(1)


if __name__ == "__main__":
    try:
        rti.asyncio.run(ProfilesExamplePublisher(0).run(sys.maxsize))
    except KeyboardInterrupt:
        pass
