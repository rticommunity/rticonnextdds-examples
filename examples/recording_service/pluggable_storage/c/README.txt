-------------------------------------------------------------------------------
(c) Copyright, Real-Time Innovations
All rights reserved.
No duplications, whole or partial, manual or electronic, may be made
without express written permission.  Any such copies, or
revisions thereof, must display this notice unaltered.
This code contains trade secrets of Real-Time Innovations, Inc.
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
Recording Service Example
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

This example shows a storage plug-in written in C. The example is composed of
two parts: the writing side (that can be plugged into Recorder or Converter) 
and the reading side (that can be plugged into Replay or Converter - although 
only a Replay configuration file is supplied).

The example works with one single type called HelloMsg. The IDL for the type is
supplied. Any other topics or types won't be recorded or replayed. The purpose
of this example is to show how the Recorder and Replay APIs in C can be used to
plug-in custom storage needs. In this simple example, Recorder will store the
discovered samples in a file called C_PluggableStorage.dat and a companion file
called C_PluggableStorage.dat.info, by using the storage plug-in. The samples 
are stored in a textual format. These samples are later retrieved by Replay by 
using the reading plug-in.

BUILDING THE EXAMPLE
--------------------

***
Note: the build environment assumes that you define the following variable:

NDDSHOME - Set to point to an installation of RTI Connext DDS.

For explanation purposes, we'll call the location of this example (the folder
this file is located in) EXAMPLEDIR. The different steps are explained with
unix-based path notation.

***

There are two different parts that have to be built for this example to work.

1) The surrounding DDS system able to publish and subscribe to the HelloMsg type

The IDL file is located in the 'src' folder. First we need to use RTI Code 
Generator in NDDSHOME to generate the code that publishes and subscribes to the
type.

$NDDSHOME/bin/rtiddsgen -example <your arch> -language C -d $EXAMPLEDIR/src $EXAMPLEDIR/src/HelloMsg.idl

This should not override the publisher.c and subscriber.c files already bundled
in the example.

Once the code has been generated, the code can be built following normal 
Code Generator steps.

2) The two dynamic libraries that are to be loaded by Recorder and Replay.

These two libraries are called:
FileStorageWriter.dll and FileStorageReader.dll (on Windows)
libFileStorageWriter.so and libFileStorageReader.so (on Linux)
libFileStorageWriter.dylib and libFileStorageReader.dylib (on OS X)

For Linux and OS X builds, use the directory $EXAMPLEDIR/make. You can execute
gmake -f Makefile.<your arch>

For Windows builds, use the VS 2010 projects inside the win32 folder. You can
load the provided VS solution into VS and build normally.

RUNNING THE EXAMPLE
-------------------

In order for Replay to have something to replay with, we recommend that Recorder
is run first to obtain a 'database' file.

You need to make sure that the FileStorageWriter library is in the library 
path.  On OS X, you must use the variable RTI_LD_LIBRARY_PATH instead of 
the DYLD_LIBRARY_PATH. 

Run the HelloMsg publisher:

cd $EXAMPLEDIR/src
objs/<your arch>/HelloMsg_publisher <your domain ID>

Now run Recorder:

$NDDSHOME/bin/rtirecordingservice -cfgFile pluggable_storage_example.xml -cfgName C_StorageExample -domainIdBase <your domain ID>

Check that two files are created: C_PluggableStorage.dat and 
C_PluggableStorage.dat.info. Verify that C_PluggableStorage.dat is growing in 
size.

Once you stop the publisher and Recorder, you can run the subscriber and Replay
to verify that you can read the samples recorded in the C_PluggableStorage.dat
file.

Now that we have recorded data using our custom plugin, we will again use our 
custom plugin, this time to read that data into replay, and publish it back 
into a DDS subscriber.

Run the HelloMsg subscriber:

cd $EXAMPLEDIR/src
objs/<your arch>/HelloMsg_subscriber <your domain ID>

Now run Replay:

$NDDSHOME/bin/rtireplayservice -cfgFile pluggable_replay_example.xml -cfgName C_ReaderExample -domainIdBase <your domain ID>

You should see the samples in the file being published and received by the 
subscribing app. Note: If you started Recording Service before starting the 
publisher app, it may look like Replay is taking a long time to start. This 
happens because the start time of the recording (written in the 
C_PluggableStorage.dat.info file) will be much earlier than the first recorded 
sample. For this example, we recommend that you run the publisher before you 
run Recorder, so the start time and the time of the first sample will be 
similar. 


