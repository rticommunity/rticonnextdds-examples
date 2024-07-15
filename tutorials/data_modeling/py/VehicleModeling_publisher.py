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


class VehicleMetricsPublisher:

    @staticmethod
    def run_publisher(domain_id: int, sample_count: int):

        # A DomainParticipant allows an application to begin communicating in
        # a DDS domain. Typically there is one DomainParticipant per application.
        # DomainParticipant QoS is configured in USER_QOS_PROFILES.xml
        participant = dds.DomainParticipant(domain_id)

        # A Topic has a name and a datatype.
        topic = dds.Topic(
            participant, "Example VehicleMetrics", VehicleMetrics
        )

        # This DataWriter will write data on Topic "Example VehicleMetrics"
        qos_provider = dds.QosProvider("VehicleModeling.xml")

        writer_qos = qos_provider.datawriter_qos_from_profile(
            "VehicleModeling_Library::VehicleMetrics_Profile"
        )
        writer = dds.DataWriter(
            participant.implicit_publisher, topic, writer_qos
        )
        sample = VehicleMetrics()

        for count in range(sample_count):
            # Catch control-C interrupt
            try:
                # Modify the data to be sent here

                print(f"Writing VehicleMetrics, count {count}")
                writer.write(sample)
                time.sleep(1)
            except KeyboardInterrupt:
                break

        print("preparing to shut down...")


if __name__ == "__main__":
    VehicleMetricsPublisher.run_publisher(
        domain_id=0, sample_count=sys.maxsize
    )
