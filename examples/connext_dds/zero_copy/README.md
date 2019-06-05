# Example Code to demonstrate the usage of zero copy

*Warning: This example is outdated since RTI Connext 6.0. See [FlatData and ZeroCopy Example](https://github.com/rticommunity/rticonnextdds-examples/tree/master/examples/connext_dds/flat_data_latency).*

This example is a modified version of the helloWorld example that can
be generated using rtiddsgen with the -example command-line option.
The original publisher and subscriber applications have been modified to 
show how to write and read data via shared memory with zero copies.

## zero_copy_publisher 
A publisher application which allocates a shared memory 
segment (FrameSet) identified by a key. The number of frames in the shared
memory segment is configurable using the command-line parameter '-fc'.
The publisher application provides the shared memory key to the matching 
DataReaders during DDS discovery so that the subscribing applications can attach
to the shared memory segment containing the frames.

When the publisher application needs to send a frame, it gets the next frame
available from the shared memory segment, populates it, and writes a reference
to that frame with a DDS DataWriter. In this example a reference is a combination
of an index (in the shared memory segment) and a CRC code for the frame.

The type defining the shared memory reference is defined in zero_copy.idl.

## zero_copy_subscriber 
A subscriber application which creates a DataReader to
receive shared memory frame references. The subscribing application is prepared
to receive frames from multiple DataWriters. For each DataWriter, the subscribing
application will attach to the corresponding shared memory segment using the 
FrameSetView abstraction. The key identifying a shared memory segment is 
propagated as part of the DataWriter discovery information.
 
The example works with a type defined in zero_copy.idl, which 
requires code generation with rtiddsgen.


