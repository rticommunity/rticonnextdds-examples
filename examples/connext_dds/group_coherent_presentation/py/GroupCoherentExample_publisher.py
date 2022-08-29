#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#
import random
import time
import argparse

import rti.connextdds as dds
from GroupCoherentExample import AlarmCode, Alarm, HeartRate, Temperature


def get_patient_heart_rate():
    heart_rates = [35, 56, 85, 110]
    return random.choice(heart_rates)


def get_patient_temperature():
    temperatures = [95.0, 98.1, 99.2, 101.7]
    return random.choice(temperatures)


def run_publisher_application(domain_id: int, sample_count: int):
    # Start communicating in a domain, usually one participant per application
    participant = dds.DomainParticipant(domain_id)

    # Create three Topics with the names and datatypes
    alarm_topic = dds.Topic(participant, "Alarm", Alarm)
    heart_rate_topic = dds.Topic(participant, "HeartRate", HeartRate)
    temperature_topic = dds.Topic(participant, "Temperature", Temperature)

    publisher_qos = dds.QosProvider.default.publisher_qos
    publisher = dds.Publisher(participant, publisher_qos)
    writer_qos = dds.QosProvider.default.datawriter_qos

    # Create one Writer per Topic using the Publisher and DataWriter QoS
    # specified in the xml file
    alarm_writer = dds.DataWriter(publisher, alarm_topic, writer_qos)
    heart_rate_writer = dds.DataWriter(publisher, heart_rate_topic, writer_qos)
    temperature_writer = dds.DataWriter(
        publisher,
        temperature_topic,
        writer_qos)

    # Create the three data samples and initialize the values
    alarm_data = Alarm(patient_id=1, alarm_code=AlarmCode.PATIENT_OK)
    heart_rate_data = HeartRate(patient_id=1, beats_per_minute=65)
    temperature_data = Temperature(patient_id=1, temperature=98.6)

    # Below we will write a coherent set any time the patient's vitals are
    # abnormal. Otherwise we will publish the patient's vital normally
    for count in range(sample_count):
        # Catch control c interrupt
        try:
            heart_rate_data.beats_per_minute = get_patient_heart_rate()
            temperature_data.temperature = get_patient_temperature()
            if (
                heart_rate_data.beats_per_minute >= 100
                or heart_rate_data.beats_per_minute <= 40
                or temperature_data.temperature >= 100.0
                or temperature_data.temperature <= 95.0
            ):
                # Sound an alarm. In this case, we want all of the
                # patients vitals along with the alarm to be delivered as
                # a single coherent set of data so that we can correlate
                # the alarm with the set of vitals that triggered it
                print("Patient's vitals are abnormal, raising alarm and sending coherent set")
                with dds.CoherentSet(publisher):   # Start a coherent set
                    heart_rate_writer.write(heart_rate_data)
                    temperature_writer.write(temperature_data)

                    alarm_data.alarm_code = AlarmCode.ABNORMAL_READING
                    alarm_writer.write(alarm_data)
                    # End of coherent set
            else:
                # No alarm necessary, publish the patient's vitals as normal
                heart_rate_writer.write(heart_rate_data)
                temperature_writer.write(temperature_data)
                print("Writing patient's vitals...")
            time.sleep(2)
        except KeyboardInterrupt:
            break

    print("preparing to shut down...")
    participant.close()


def main():
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Group Coherent \
        Presentation (Publisher)"
    )
    parser.add_argument(
        "-d",
        "--domain",
        type=int,
        default=0,
        help="DDS Domain ID | Range: 0-232 | Default: 0",
    )
    parser.add_argument(
        "-c",
        "--count",
        type=int,
        default=50,
        help="Number of samples to send | Default 50",
    )
    parser.add_argument(
        "-v",
        "--verbosity",
        type=int,
        default=1,
        help="How much debugging output to show | Range: 0-3 | Default: 1",
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0
    assert 0 <= args.verbosity < 4

    verbosity_levels = {
        0: dds.Verbosity.SILENT,
        1: dds.Verbosity.EXCEPTION,
        2: dds.Verbosity.WARNING,
        3: dds.Verbosity.STATUS_ALL,
    }

    # Sets Connext verbosity to help debugging
    verbosity = verbosity_levels.get(args.verbosity, dds.Verbosity.EXCEPTION)

    dds.Logger.instance.verbosity = verbosity

    try:
        run_publisher_application(args.domain, args.count)
    except Exception as e:
        print(f"Exception in run_publisher_application(): {e}")
    return


if __name__ == "__main__":
    main()
