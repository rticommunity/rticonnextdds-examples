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

import rti.connextdds as dds
from GroupCoherentExample import AlarmCode, Alarm, HeartRate, Temperature


def process_data(reader):

    # Take all samples.  Samples are loaned to application, loan is
    # returned when LoanedSamples destructor called or when you return
    # the loan explicitly.
    samples_read = 0
    samples = reader.take()

    for (data, info) in samples:
        if info.valid:
            print(f"Received: {data}")
            samples_read += 1

    return samples_read


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

    # Initialize samples_read to zero
    samples_read = 0

    # Associate a handler with the status condition. This will run when the
    # condition is triggered, in the context of the dispatch call (see below)
    # condition argument is not used
    def alarm_handler(_):
        nonlocal samples_read
        nonlocal alarm_reader
        samples_read += process_data(alarm_reader)

    # Obtain the DataReader's Status Condition
    status_condition = dds.StatusCondition(alarm_reader)

    # Enable the 'data available' status and set the handler.
    status_condition.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
    status_condition.set_handler(alarm_handler)

    # Create a WaitSet and attach the StatusCondition
    waitset = dds.WaitSet()
    waitset += status_condition

    def heart_rate_handler(_):
        nonlocal samples_read
        nonlocal heart_rate_reader
        samples_read += process_data(heart_rate_reader)

    status_condition = dds.StatusCondition(heart_rate_reader)
    status_condition.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
    status_condition.set_handler(heart_rate_handler)
    waitset += status_condition

    def temperature_handler(_):
        nonlocal samples_read
        nonlocal temperature_reader
        samples_read += process_data(temperature_reader)

    status_condition = dds.StatusCondition(temperature_reader)
    status_condition.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
    status_condition.set_handler(temperature_handler)
    waitset += status_condition

    while samples_read < sample_count:
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

    # Sets Connext verbosity to help debugging
    verbosity = dds.Verbosity.EXCEPTION
    if args.verbosity == 0:
        verbosity = dds.Verbosity.SILENT
    elif args.verbosity == 1:
        verbosity = dds.Verbosity.EXCEPTION
    elif args.verbosity == 2:
        verbosity = dds.Verbosity.WARNING
    else:
        verbosity = dds.Verbosity.STATUS_ALL

    dds.Logger.instance.verbosity = verbosity

    try:
        run_subscriber_application(args.domain, args.count)
    except Exception as e:
        print(f"Exception in run_subscriber_application(): ", e)
    return


if __name__ == '__main__':
    main()
