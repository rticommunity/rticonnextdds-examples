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
import argparse
from concurrent.futures import process

import rti.connextdds as dds
from GroupCoherentExample import AlarmCode, Alarm, HeartRate, Temperature


# a listener for the datatypes used for the patient
class PatientDRListener(dds.NoOpDataReaderListener):
    def on_sample_lost(
        self,
        reader: dds.AnyDataReader,
        status: dds.SampleLostStatus):
        if status.last_reason == dds.SampleLostState.LOST_BY_INCOMPLETE_COHERENT_SET:
            print(f"Lost {status.total_count_change} samples in an incomplete coherent set.\n")


def print_coherent_set_info(info: dds.SampleInfo):
    # The coherent_set_info in the SampleInfo is only present if the sample is
    # part of a coherent set. We therefore need to check if it is present
    # before accessing any of the members
    if info.coherent_set_info is not None:
        set_info = info.coherent_set_info
        print(f"Sample is part of \
            {'an incomplete' if set_info.incomplete_coherent_set else 'a complete'} \
            group coherent set with SN({set_info.group_coherent_set_sequence_number.value})\n")
    else:
        print("Sample is not part of a coherent set.\n")


def process_data(subscriber: dds.Subscriber):
    with dds.CoherentAccess(subscriber):    # Begin coherent access
        # readers is a list of dds.AnyDataReaders
        readers = dds.Subscriber.find_datareaders(subscriber, dds.SampleState.NOT_READ)
        for reader in readers:
            # Convert from AnyDataReader to a typed DataReader
            r = dds.DataReader(reader)
            # Take all samples.  Samples are loaned to application, loan is
            # returned when LoanedSamples destructor called or when you return
            # the loan explicitly.
            for data, info in r.take():
                print(data)
                print_coherent_set_info(info)


def run_subscriber_application(domain_id: int, sample_count: int):
    # Start communicating in a domain, usually one participant per application
    participant = dds.DomainParticipant(domain_id)

    # Create three Topics with names and datatypes
    alarm_topic = dds.Topic(participant, "Alarm", Alarm)
    heart_rate_topic = dds.Topic(participant, "HeartRate", HeartRate)
    temperature_topic = dds.Topic(participant, "Temperature", Temperature)

    subscriber_qos = dds.QosProvider.default.subscriber_qos
    subscriber = dds.Subscriber(participant, subscriber_qos)
    reader_qos = dds.QosProvider.default.datareader_qos

    # Create one Reader per Topic using the Subscriber and DataReader QoS
    # specified in the xml file
    alarm_reader = dds.DataReader(subscriber, alarm_topic, reader_qos)
    heart_rate_reader = dds.DataReader(
        subscriber,
        heart_rate_topic,
        reader_qos)
    temperature_reader = dds.DataReader(
        subscriber,
        temperature_topic,
        reader_qos)

    # attach listeners to the readers
    alarm_reader.set_listener(PatientDRListener(), dds.StatusMask.SAMPLE_LOST)
    heart_rate_reader.set_listener(
        PatientDRListener(),
        dds.StatusMask.SAMPLE_LOST)
    temperature_reader.set_listener(
        PatientDRListener(),
        dds.StatusMask.SAMPLE_LOST)

    # Associate a handler with the status condition. This will run when the
    # condition is triggered, in the context of the dispatch call (see below)
    # condition argument is not used
    def alarm_handler(_):
        nonlocal subscriber
        process_data(subscriber)

    # Obtain the Subscriber's Status Condition
    status_condition = dds.StatusCondition(subscriber)

    # Enable the 'data on readers' status and set the handler.
    status_condition.enabled_statuses = dds.StatusMask.DATA_ON_READERS
    status_condition.set_handler(alarm_handler)

    # Create a WaitSet and attach the StatusCondition
    waitset = dds.WaitSet()
    waitset += status_condition

    def heart_rate_handler(_):
        nonlocal subscriber
        process_data(subscriber)

    status_condition.set_handler(heart_rate_handler)
    waitset += status_condition

    def temperature_handler(_):
        nonlocal subscriber
        process_data(subscriber)

    status_condition.set_handler(temperature_handler)
    waitset += status_condition

    while True:
        # Catch control c interrupt
        try:
            # Dispatch will call the handlers associated to the
            # WaitSet conditions when they activate
            print("Subscriber sleeping for 1 seconds...")

            waitset.dispatch(dds.Duration(1))  # Wait up to 1s each time
        except KeyboardInterrupt:
            break

    print('preparing to shut down...')
    participant.close()


def main():
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Group Coherent \
        Presentation (Publisher)"
    )
    parser.add_argument("-d",
                        "--domain",
                        type=int,
                        default=0,
                        help="DDS Domain ID | Range: 0-232 | Default: 0")
    parser.add_argument("-c",
                        "--count",
                        type=int,
                        default=50,
                        help="Number of samples to send | Default 50")
    parser.add_argument("-v",
                        "--verbosity",
                        type=int,
                        default=1,
                        help="How much debugging output to show \
                        | Range: 0-3 | Default: 1")

    args = parser.parse_args()
    assert 0 <= args.domain  < 233
    assert args.count >= 0
    assert 0 <= args.verbosity < 4

    d = {0: dds.Verbosity.SILENT,
         1: dds.Verbosity.EXCEPTION,
         2: dds.Verbosity.WARNING,
         3: dds.Verbosity.STATUS_ALL}

    # Sets Connext verbosity to help debugging
    verbosity = d.get(args.verbosity, dds.Verbosity.EXCEPTION)

    dds.Logger.instance.verbosity = verbosity

    try:
        run_subscriber_application(args.domain, args.count)
    except Exception as e:
        print(f"Exception in run_subscriber_application(): {e}")
    return


if __name__ == '__main__':
    main()
