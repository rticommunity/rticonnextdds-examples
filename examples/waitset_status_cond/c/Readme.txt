Applies to RTI Data Distribution Service 4.0 and above.

Purpose
=======
PUBLISHER:
Waitsets are completely independent of publishers.

SUBSCRIBER:
This example shows how to use WaitSets with a StatusCondition to read data. As the listener callback on_data_available is now unused, we remove it. To initialize the WaitSet, we create the condition on which to wait, and attach it to the Waitset.

Note that the WaitSet is a "top-level" entity, not one created by a DDS entity. It is only related to a reader by the contained conditions, which are created via DataReaders.

Also note that WaitSets block in the context of the application rather than DDS threads. Thus, there are fewer constraints on processing data than if we're working in an on_data_available callback, since DDS housekeeping threads continue to run.

Building
=======
Make sure you are using one of the relevant RTI Data Distribution Service versions, as specified at the top of the Solution.

Before compiling or running the example, make sure the environment variable NDDSHOME is set to the directory where your version of RTI Data Distribution Service is installed.

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32VS2010). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\Waitsets\c++> rtiddsgen -language C++ -example i86Win32VS2010 waitset_statuscond.idl

...you will see a message that looks like this:

File C:\local\Waitsets\c\waitset_statuscond_subscriber.c already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber code has not been replaced, which is fine since all the source files for the example are already provided.

Running
=======
In two separate command prompt windows for the publisher and subscriber, navigate to the objs/<arch> directory and run these commands:

Windows systems:

    * waitset_statuscond_publisher.exe <domain#> 6
    * waitset_statuscond_subscriber.exe <domain#> 10

UNIX systems:

    * ./waitset_statuscond_publisher <domain#> 6
    * ./waitset_statuscond_subscriber <domain#> 10

The applications accept two arguments:

   1. The <domain #>. Both applications must use the same domain # in order to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher and sleep periods for the subscriber. A value of '0' instructs the application to run forever; this is the default.

While generating the output below, we used values that would capture the most interesting behavior.

Publisher Output
=============
Writing waitset_statuscond, count 0
Writing waitset_statuscond, count 1
Writing waitset_statuscond, count 2
Writing waitset_statuscond, count 3
Writing waitset_statuscond, count 4
Writing waitset_statuscond, count 5

Subscriber Output
==============
got 1 active conditions
   x: 1
got 1 active conditions
   x: 2
got 1 active conditions
   x: 3
got 1 active conditions
   x: 4
got 1 active conditions
   x: 5
got 1 active conditions
   Got metadata
wait timed out
wait timed out
wait timed out