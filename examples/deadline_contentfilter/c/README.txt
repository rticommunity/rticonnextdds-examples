
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

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32VS2005). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\deadline_contentfilter\c> rtiddsgen -language C -example i86Win32VS2005 deadline_contentfilter.idl

...you will see messages that look like this:

File C:\local\deadline_contentfilter\c\deadline_contentfilter_subscriber.c already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File C:\local\deadline_contentfilter\c\deadline_contentfilter_publisher.c already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File c:\local\deadline_contentfilter\c\USER_QOS_PROFILES.xml already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

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
Writing instance0, x = 1, y = 1Writing instance1, x = 1, y = 1Writing instance0, x = 2, y = 2Writing instance1, x = 2, y = 2Writing instance0, x = 3, y = 3Writing instance1, x = 3, y = 3Writing instance0, x = 4, y = 4Writing instance1, x = 4, y = 4Writing instance0, x = 5, y = 5Writing instance1, x = 5, y = 5Writing instance0, x = 6, y = 6Writing instance1, x = 6, y = 6Writing instance0, x = 7, y = 7Writing instance1, x = 7, y = 7Writing instance0, x = 8, y = 8Writing instance1, x = 8, y = 8Writing instance0, x = 9, y = 9Writing instance1, x = 9, y = 9Writing instance0, x = 10, y = 10Writing instance1, x = 10, y = 10Writing instance0, x = 11, y = 11Writing instance1, x = 11, y = 11Writing instance0, x = 12, y = 12Writing instance1, x = 12, y = 12Writing instance0, x = 13, y = 13Writing instance1, x = 13, y = 13Writing instance0, x = 14, y = 14Writing instance1, x = 14, y = 14Writing instance0, x = 15, y = 15Writing instance1, x = 15, y = 15Writing instance0, x = 16, y = 16Stopping writes to instance1Writing instance0, x = 17, y = 17Writing instance0, x = 18, y = 18

Subscriber Output
==============
@ t=3.00s, Instance0: <1,1>@ t=3.00s, Instance1: <1,1>@ t=4.00s, Instance0: <2,2>@ t=4.00s, Instance1: <2,2>@ t=5.00s, Instance0: <3,3>@ t=5.00s, Instance1: <3,3>@ t=6.00s, Instance0: <4,4>@ t=6.00s, Instance1: <4,4>@ t=7.00s, Instance0: <5,5>@ t=7.00s, Instance1: <5,5>@ t=8.00s, Instance0: <6,6>@ t=8.00s, Instance1: <6,6>@ t=9.00s, Instance0: <7,7>@ t=9.00s, Instance1: <7,7>@ t=10.00s, Instance0: <8,8>@ t=10.00s, Instance1: <8,8>Starting to filter out instance1 at time 10@ t=11.00s, Instance0: <9,9>@ t=12.00s, Instance0: <10,10>Missed deadline @ t=12.00s on instance code = 1@ t=13.00s, Instance0: <11,11>@ t=14.00s, Instance0: <12,12>Missed deadline @ t=14.00s on instance code = 1@ t=15.00s, Instance0: <13,13>@ t=16.00s, Instance0: <14,14>Missed deadline @ t=16.00s on instance code = 1@ t=17.00s, Instance0: <15,15>@ t=18.00s, Instance0: <16,16>Missed deadline @ t=18.00s on instance code = 1@ t=19.00s, Instance0: <17,17>^C