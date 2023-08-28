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
from hello import HelloWorld
import time
import argparse
import textwrap
import pathlib


XML_FILE_PATH = (
    str(pathlib.Path(__file__).parent.absolute()) + "/SecureQos.xml"
)

# Process incoming data in a listener, print out each sample


class SecurityListener(dds.NoOpDataReaderListener):
    def on_data_available(self, reader):
        for sample in reader.take_data():
            print(sample)


def subscriber_main(domain_id, sample_count, profile):
    print(f"Running with profile {profile}")
    provider = dds.QosProvider(XML_FILE_PATH)
    qos = provider.participant_qos_from_profile(
        f"SecurityExampleProfiles::{profile}"
    )
    participant = dds.DomainParticipant(domain_id, qos)

    topic = dds.Topic(participant, "Example HelloWorld", HelloWorld)

    reader = dds.DataReader(
        dds.Subscriber(participant),
        topic,
        dds.DataReaderQos(),
        SecurityListener(),
        dds.StatusMask.DATA_AVAILABLE,
    )

    count = 0
    while (sample_count == 0) or (count < sample_count):
        print("Waiting for data...")
        time.sleep(1)
        count += 1


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Security (Subscriber)"
    )
    parser.add_argument(
        "-d",
        "--domain",
        type=int,
        default=0,
        help="DDS Domain ID",
        choices=range(0, 233),
    )
    parser.add_argument(
        "-c",
        "--count",
        type=int,
        default=0,
        help="Number of samples to receive before shutting down",
    )
    parser.add_argument(
        "-p",
        "--profile",
        type=str,
        default="A",
        help="Security profile to use (A, B, RSA_A, RSA_B, ECDSA_P384_A, or ECDSA_P384_B)",
        choices=["A", "B", "RSA_A", "RSA_B", "ECDSA_P384_A", "ECDSA_P384_B"],
    )

    args = parser.parse_args()

    try:
        subscriber_main(args.domain, args.count, args.profile.upper())
    except KeyboardInterrupt:
        pass
