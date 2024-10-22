
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
import rti.connextdds as dds
import rti.asyncio

from waitset_status_cond import waitset_status_cond

class waitset_status_condSubscriber:

    class Listener(dds.NoOpDataReaderListener):
        def on_requested_deadline_missed(self, reader: dds.DataReader, status: dds.RequestedDeadlineMissedStatus):
            print("Deadline missed")

        def on_sample_rejected(self, reader: dds.DataReader, status: dds.SampleRejectedStatus):
            print("Sample rejected")

        def on_sample_lost(self, reader: dds.DataReader, status: dds.SampleLostStatus):
            print("Sample lost")

        def on_requested_incompatible_qos(self, reader: dds.DataReader, status: dds.RequestedIncompatibleQosStatus):
            print("Requested incompatible QoS")

        def on_subscription_matched(self, reader: dds.DataReader, status: dds.SubscriptionMatchedStatus):
            print("Subscription matched")

        def on_liveliness_changed(self, reader: dds.DataReader, status: dds.LivelinessChangedStatus):
            print("Liveliness changed")

            
    def __init__(self, domain_id: int):
        # Start communicating in a domain. Usually there is one participant
        # per application. Load a QoS profile from USER_QOS_PROFILES.xml
        participant_qos = dds.QosProvider.default.participant_qos_from_profile(
            "waitset_status_cond_Library::waitset_status_cond_Profile")
        self.participant = dds.DomainParticipant(domain_id, participant_qos)

        # A Topic has a name and a datatype.
        topic = dds.Topic(self.participant, "Example waitset_status_cond", waitset_status_cond)

        # Create a Subscriber, loading QoS profile from USER_QOS_PROFILES.xml
        subscriber_qos = dds.QosProvider.default.subscriber_qos_from_profile(
            "waitset_status_cond_Library::waitset_status_cond_Profile")
        subscriber = dds.Subscriber(self.participant, subscriber_qos)

        # Create a DataReader, loading QoS profile from USER_QOS_PROFILES.xml,
        # and using a listener for events. Note that the DATA_AVAILABLE event is
        # not handled. We will take the data asynchronously in the run() method.
        reader_qos = dds.QosProvider.default.datareader_qos_from_profile(
            "waitset_status_cond_Library::waitset_status_cond_Profile")
        listener = waitset_status_condSubscriber.Listener()
        status_mask = dds.StatusMask.REQUESTED_DEADLINE_MISSED \
            | dds.StatusMask.SAMPLE_REJECTED \
            | dds.StatusMask.SAMPLE_LOST \
            | dds.StatusMask.REQUESTED_INCOMPATIBLE_QOS \
            | dds.StatusMask.SUBSCRIPTION_MATCHED \
            | dds.StatusMask.LIVELINESS_CHANGED
        self.reader = dds.DataReader(
            subscriber,
            topic,
            reader_qos,
            listener,
            status_mask)

        self.samples_read = 0

    async def run(self, sample_count: int):
        async for data in self.reader.take_data_async():
            print(data)
            self.samples_read += 1
            if self.samples_read >= sample_count:
                break


if __name__ == "__main__":
    try:
        rti.asyncio.run(waitset_status_condSubscriber(0).run(sys.maxsize))
    except KeyboardInterrupt:
        pass
