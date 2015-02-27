=================================================
 Example Code -- Deadline Content Filter Example
=================================================

Building Java Example
=====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2010 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems run:

rtiddsgen -language Java -example i86Win32VS2010 deadline_contentfilter.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language Java -example i86Linux2.6gcc4.4.3 deadline_contentfilter.idl

...you will see messages that look like this:

File C:\local\deadline_contentfilter\java\deadline_contentfilterSubscriber.java 
already exists and will not be replaced with updated content. If you would like
to get a new file with the new content, either remove this file or 
supply -replace option.
File C:\local\deadline_contentfilter\java\deadline_contentfilterPublisher.java 
already exists and will not be replaced with updated content. If you would like 
to get a new file with the new content, either remove this file or 
supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

On Windows systems run:

javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar *.java

On Unix systems (including Linux and MacOS X):

javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar *.java


Running Java Example
====================
In two separate command prompt windows for the publisher and subscriber. 
Run the following commands from the example directory:

On Windows systems run:

java -cp .;%NDDSHOME%\lib\java\nddsjava.jar deadline_contentfilterPublisher  <domain_id> <samples_to_send>
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar deadline_contentfilterSubscriber <domain_id> <sleep_periods>

On Unix systems (including Linux and MacOS X) run:

java -cp .:$NDDSHOME/lib/java/nddsjava.jar deadline_contentfilterPublisher  <domain_id> <samples_to_send>
java -cp .:$NDDSHOME/lib/java/nddsjava.jar deadline_contentfilterSubscriber <domain_id> <sleep_periods>

The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.

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