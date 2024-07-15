
#
# (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import time
import sys
import rti.connextdds as dds
from VehicleModeling import VehicleMetrics

class VehicleMetricsSubscriber:

    @staticmethod
    def process_data(reader):
        # take_data() returns copies of all the data samples in the reader
        # and removes them. To also take the SampleInfo meta-data, use take().
        # To not remove the data from the reader, use read_data() or read().
        samples = reader.take_data()
        for sample in samples:
            print(f"Received: {sample}")
    
        return len(samples)

    @staticmethod
    def run_subscriber(domain_id: int, sample_count: int):

        # A DomainParticipant allows an application to begin communicating in
        # a DDS domain. Typically there is one DomainParticipant per application.
        # DomainParticipant QoS is configured in USER_QOS_PROFILES.xml
        participant = dds.DomainParticipant(domain_id)

        # A Topic has a name and a datatype.
        topic = dds.Topic(participant, "Example VehicleMetrics", VehicleMetrics)

        # This DataReader reads data on Topic "Example VehicleMetrics".
        qos_provider = dds.QosProvider("VehicleModeling.xml")

        reader_qos = qos_provider.datareader_qos_from_profile(
            "VehicleModeling_Library::VehicleMetrics_Profile"
        )
        reader = dds.DataReader(participant.implicit_subscriber, topic, reader_qos)

        # Initialize samples_read to zero
        samples_read = 0

        # Associate a handler with the status condition. This will run when the
        # condition is triggered, in the context of the dispatch call (see below)
        # condition argument is not used
        def condition_handler(_):
            nonlocal samples_read
            nonlocal reader
            samples_read += VehicleMetricsSubscriber.process_data(reader)

        # Obtain the DataReader's Status Condition
        status_condition = dds.StatusCondition(reader)

        # Enable the "data available" status and set the handler.
        status_condition.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
        status_condition.set_handler(condition_handler)

        # Create a WaitSet and attach the StatusCondition
        waitset = dds.WaitSet()
        waitset += status_condition

        while samples_read < sample_count:
            # Catch control-C interrupt
            try:
                # Dispatch will call the handlers associated to the WaitSet conditions
                # when they activate
                print("Hello World subscriber sleeping for 1 seconds...")

                waitset.dispatch(dds.Duration(1))  # Wait up to 1s each time
            except KeyboardInterrupt:
                break

        print("preparing to shut down...")


if __name__ == "__main__":
    VehicleMetricsSubscriber.run_subscriber(
            domain_id=0,
            sample_count=sys.maxsize)
