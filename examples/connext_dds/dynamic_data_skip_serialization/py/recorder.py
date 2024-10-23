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
from util import wait_for_reader, wait_for_writer, publish_example_data

EXAMPLE_TYPE = dds.StructType(
    "RecordExample",
    dds.MemberSeq(
        [
            dds.Member("id", dds.StringType(128)),
            dds.Member("payload", dds.SequenceType(dds.Int32Type(), 1024)),
        ]
    ),
)


def record(file_name: str, domain_id: int):
    participant = dds.DomainParticipant(domain_id)

    # To disable the deserialization on the DataReader and get direct access
    # to the serialized CDR buffer, we need to register the DataReader type
    # with the DynamicDataTypeSerializationProperty.skip_deserialization set
    # to True, and then create the DataReader with the name used to register
    # the type.
    dd_property = dds.DynamicDataTypeSerializationProperty()
    dd_property.skip_deserialization = True
    type_name = "RecordExample"
    participant.register_type(type_name, EXAMPLE_TYPE, dd_property)

    topic = dds.DynamicData.Topic(
        participant,
        topic_name="Example Record",
        type_name=type_name,  # Specify the registered type name
        qos=participant.default_topic_qos,
    )

    qos = dds.QosProvider.default.datareader_qos_from_profile(
        dds.BuiltinProfiles.generic_strict_reliable
    )
    reader = dds.DynamicData.DataReader(topic, qos)
    wait_for_writer(reader)

    # open a file for binary writing:
    out_file = open(file_name, "wb")

    # Define the function that will take the data and record it
    def record_data(_) -> None:
        # When the skip_deserialization property is set to True the data must
        # be taken as a loaned collection, since copying the data is not allowed.
        with reader.take_loaned() as samples:
            for data, info in samples:
                if not info.valid:
                    continue

                # Now the only way to access the data is to call get_cdr_buffer,
                # any other field accessor will fail.
                buffer = data.get_cdr_buffer()
                print(f"Recording data sample ({len(buffer)} bytes)")

                # Write the length and the data
                out_file.write(len(buffer).to_bytes(4, byteorder="little"))
                out_file.write(buffer)

    # Set up a ReadCondition to trigger the record_data function when
    # data is available
    read_condition = dds.ReadCondition(reader, dds.DataState.any, record_data)
    waitset = dds.WaitSet()
    waitset += read_condition

    while True:
        try:
            waitset.dispatch(dds.Duration(1))
        except KeyboardInterrupt:
            break

    print("preparing to shut down...")


def replay(file_name: str, domain_id: int):
    participant = dds.DomainParticipant(domain_id)

    # For the replay application we don't need to register the type with any
    # particular property because DynamicData DataWriters are always prepared
    # to write serialized buffers directly
    topic = dds.DynamicData.Topic(participant, "Example Record", EXAMPLE_TYPE)

    qos = dds.QosProvider.default.datawriter_qos_from_profile(
        dds.BuiltinProfiles.generic_strict_reliable
    )
    writer = dds.DynamicData.DataWriter(topic, qos)
    wait_for_reader(writer)

    data = writer.create_data()

    with open(file_name, "rb") as in_file:
        # read the binary file and write the data to the DataWriter:
        while True:
            length_bytes = in_file.read(4)
            if length_bytes == b"":
                break

            length = int.from_bytes(length_bytes, byteorder="little")
            print(f"Replaying data sample ({length} bytes)")
            buffer = in_file.read(length)

            # By calling the set_cdr_buffer method we override the contents
            # of the DynamicData object with the new serialized data. After
            # setting a cdr buffer we can't use any field getters or setters.
            data.set_cdr_buffer(buffer)
            writer.write(data)

    writer.wait_for_acknowledgments(dds.Duration(10))


def main():
    # Parse command line for --record [output file], --replay [input file], --publish, and --domain_id options
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("--record", nargs="?", const="recorded_data.bin")
    parser.add_argument("--replay", nargs="?", const="recorded_data.bin")
    parser.add_argument("--publish", action="store_true")
    parser.add_argument("--domain_id", type=int, default=0)

    args = parser.parse_args()
    if args.record:
        record(args.record, args.domain_id)
    elif args.replay:
        replay(args.replay, args.domain_id)
    elif args.publish:
        publish_example_data(args.domain_id, EXAMPLE_TYPE)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
