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

# Note, I have not run this program yet. Need to pull on my local laptop so that I can download the wheel
import rti.connextdds as dds
# Note: cannot do 'from async' because async is a keyword since Python 3.5 
# therefore I have to update the name of async.idl to async_type.idl
from async_type import async_type


def process_data(reader: dds.DataReader):
    # Take all samples. Samples are loaned to application, loan is
    # returned when LoanedSamples destructor is called or when you
    # return the loan explicitly
    samples_read = 0
    samples = reader.take()

    for (data, info) in samples:
        if info.valid:
            print("Received: {}".format(data))
            samples_read += 1
    return samples_read


def run_subscriber_application(domain_id: int, sample_count: int):
    # Start communicating in a domain, usually one participant per application
    participant = dds.DomainParticipant(domain_id)

    # Create a Topic
    topic = dds.Topic(participant, "Example async", async_type)

    # Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    reader_qos = dds.QosProvider.default.datareader_qos

    # Note: Update this to Python
    """ # If you want to change the DataWriter's QoS programmatically rather than
    # using the XML file, uncomment the following lines.

    reader_qos << Reliability::Reliable();
    reader_qos << History::KeepAll();"""

    # Create a DataReader with the QoS in our profile
    reader = dds.DataReader(participant.implicit_subscriber, topic, reader_qos)

    # Initialize samples_read to zero
    samples_read = 0

    # Assocaite a handler with the status condition. This will run when the
    # condition is triggered, in the context of the dispatch call (see below)
    # condition argument is not used 
    def handler(_):
        nonlocal samples_read
        nonlocal reader
        samples_read += process_data(reader)

    # Obtain the DataReader's Status Condition
    status_condition = dds.StatusCondition(reader)

    # Enable the 'data_available' status and set the handler
    status_condition.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
    status_condition.set_handler(handler)

    # Create a WaitSet and attach the StatusCondition
    waitset = dds.WaitSet()
    waitset += status_condition

    while samples_read < sample_count:
        # Catch control c interrupt
        try:
            # Dispatch wil call the handlers associated to the WaitSet conditions
            # when they activate
            print("async subscriber sleeping up to 1 sec...")
            waitset.dispatch(dds.Duration(1))  # Wait up to 1s each time
        except KeyboardInterrupt:
            break

    print("preparing to shut down...")
    reader.close()
    topic.close()
    participant.close()


def main():
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Asynchronous Publishing"
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
        run_subscriber_application(args.domain, args.count)
    except Exception as e:
        print(f"Exception in run_subscriber_application(): {e}")
    return


if __name__ == "__main__":
    main()
