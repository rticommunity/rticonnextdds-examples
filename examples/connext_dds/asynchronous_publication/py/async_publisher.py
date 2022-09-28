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
import time

# Note, I have not run this program yet. Need to pull on my local laptop so that I can download the wheel
import rti.connextdds as dds
# Note: cannot do 'from async' because async is a keyword since Python 3.5 
# therefore I have to update the name of async.idl to async_type.idl
from async_type import async_type


def run_publisher_application(domain_id: int, sample_count: int):
    # Start communicating in a domain, usually one participant per application
    participant = dds.DomainParticipant(domain_id)

    # Create a Topic
    topic = dds.Topic(participant, "Example async", async_type)

    # Retrieve the default DataWriter QoS, from USER_QOS_PROFILES.xml
    writer_qos = dds.QosProvider.default.datawriter_qos

    # Note: Update this to Python
    """ # If you want to change the DataWriter's QoS programmatically rather than
    # using the XML file, uncomment the following lines.

    # In this case, we set the publish mode QoS to asynchronous publish mode,
    # which enables asynchronous publishing. We also set the flow controller
    # to be the fixed rate flow controller, and we increase the history depth.
    writer_qos << Reliability::Reliable(Duration(60));

    DataWriterProtocol writer_protocol_qos;
    RtpsReliableWriterProtocol rtps_writer_protocol;
    rtps_writer_protocol.min_send_window_size(50);
    rtps_writer_protocol.max_send_window_size(50);
    writer_protocol_qos.rtps_reliable_writer(rtps_writer_protocol);
    writer_qos << writer_protocol_qos;

    # Since samples are only being sent once per second, DataWriter will need
    # to keep them on queue.  History defaults to only keeping the last
    # sample enqueued, so we increase that here.
    writer_qos << History::KeepLast(12);

    # Set flowcontroller for DataWriter
    writer_qos << PublishMode::Asynchronous(FlowController::FIXED_RATE_NAME); """

    # Create a DataWriter with the QoS in our profile
    writer = dds.DataWriter(participant.implicit_publisher, topic, writer_qos)

    # Create data sample for writing
    sample = async_type(x=42)

    for count in range(sample_count):
        try:
            # Modify the data to be sent here
            sample.x = count
            print("Writing async_type, count {}".format(count))
            writer.write(sample)
            time.sleep(1)
        except KeyboardInterrupt:
            break
    print("preparing to shut down...")
    writer.close()
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
        run_publisher_application(args.domain, args.count)
    except Exception as e:
        print(f"Exception in run_publisher_application(): {e}")
    return


if __name__ == "__main__":
    main()