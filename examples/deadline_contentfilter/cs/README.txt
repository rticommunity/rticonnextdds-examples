
Purpose
=======
PUBLISHER:
The Publisher adopts a 1.5 second deadline, promising to write samples at least this fast. Note that deadlines apply to each instance individually. After 15 seconds, we stop writing to the 2nd instance, and we get the on_offered_deadline_missed callback.

SUBSCRIBER:
The Subscriber requests a 2 second deadline, and prints out the offending instance when this deadline is missed. Note the interaction with filtering. After 10 seconds, we set up a filter that ignores the 2nd instance. Even though the publisher is still sending samples at this point, the instance is flagged as missing its deadline.

With a time-based filter, however, if the requested deadline is less than the minimum separation, the QoS is considered incompatible.

Building
=======

Before compiling or running the example, make sure the environment variable NDDSHOME is set to the directory where your version of RTI Data Distribution Service is installed.

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32dotnet2.0). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\deadline_contentfilter\cs> rtiddsgen -language C# -example i86Win32dotnet4.0 deadline_contentfilter.idl

...you will see messages that look like this:

File C:\local\deadline_contentfilter\cs\deadline_contentfilter_subscriber.cxx already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File C:\local\deadline_contentfilter\cs\deadline_contentfilter_publisher.cxx already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has not been replaced, which is fine since all the source files for the example are already provided.

Running
=======
In two separate command prompt windows for the publisher and subscriber, navigate to the objs/<arch> directory and run these commands:

Windows systems:

    * deadline_contentfilter_publisher.exe <domain#> 13
    * deadline_contentfilter_subscriber.exe <domain#> 15

UNIX systems:

    * ./deadline_contentfilter_publisher <domain#> 13
    * ./deadline_contentfilter_subscriber <domain#> 15

The applications accept two arguments:

   1. The <domain #>. Both applications must use the same domain # in order to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher and sleep periods for the subscriber. A value of '0' instructs the application to run forever; this is the default.

While generating the output below, we used values that would capture the most interesting behavior.

Publisher Output
=============
Writing instance0, x = 1, y = 1
Writing instance1, x = 1, y = 1
Writing instance0, x = 2, y = 2
Writing instance1, x = 2, y = 2
Writing instance0, x = 3, y = 3
Writing instance1, x = 3, y = 3
Writing instance0, x = 4, y = 4
Writing instance1, x = 4, y = 4
Writing instance0, x = 5, y = 5
Writing instance1, x = 5, y = 5
Writing instance0, x = 6, y = 6
Writing instance1, x = 6, y = 6
Writing instance0, x = 7, y = 7
Writing instance1, x = 7, y = 7
Writing instance0, x = 8, y = 8
Writing instance1, x = 8, y = 8
Writing instance0, x = 9, y = 9
Stopping writes to instance1
Offered deadline missed on instance code = 1
Writing instance0, x = 10, y = 10
Writing instance0, x = 11, y = 11
Offered deadline missed on instance code = 1
Writing instance0, x = 12, y = 12
Offered deadline missed on instance code = 1
Writing instance0, x = 13, y = 13

Subscriber Output
==============
@ t=2.59s, Instance0: <1,1>
@ t=2.61s, Instance1: <1,1>
@ t=3.59s, Instance0: <2,2>
@ t=3.59s, Instance1: <2,2>
@ t=4.59s, Instance0: <3,3>
@ t=4.59s, Instance1: <3,3>
@ t=5.59s, Instance0: <4,4>
@ t=5.59s, Instance1: <4,4>
@ t=6.59s, Instance0: <5,5>
@ t=6.59s, Instance1: <5,5>
Starting to filter out instance1
@ t=7.59s, Instance0: <6,6>
@ t=8.59s, Instance0: <7,7>
Missed deadline @ t=8.61s on instance code = 1
@ t=9.59s, Instance0: <8,8>
@ t=10.59s, Instance0: <9,9>
Missed deadline @ t=10.61s on instance code = 1
@ t=11.59s, Instance0: <10,10>
@ t=12.59s, Instance0: <11,11>
Missed deadline @ t=12.61s on instance code = 1
@ t=13.59s, Instance0: <12,12>
@ t=14.59s, Instance0: <13,13>
Missed deadline @ t=14.61s on instance code = 1