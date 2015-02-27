======================================
 Example Code -- Ordered Presentation
======================================

Building Java Example
=====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2010 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2010) run:

rtiddsgen -language Java -example i86Win32VS2010 ordered.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.) run:

rtiddsgen -language Java -example i86Linux2.6gcc4.4.3 ordered.idl

You will see messages that look like this:

File C:\local\Ordered_Presentation\java\orderedSubscriber.java already exists 
and will not be replaced with updated content. If you would like to get a new 
file with the new content, either remove this file or supply -replace option.
File C:\local\Ordered_Presentation\java\orderedPublisher.java already exists 
and will not be replaced with updated content. If you would like to get a new 
file with the new content, either remove this file or supply -replace option.
File C:\local\Ordered_Presentation\java\USER_QOS_PROFILES.xml already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.

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
Run the following commands from the example directory (this is necessary to 
ensure the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

java -cp .;%NDDSHOME%\lib\java\nddsjava.jar orderedPublisher  <domain_id> <samples_to_send>
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar orderedSubscriber <domain_id> <sleep_periods>

On Unix systems (including Linux and MacOS X) run:

java -cp .:$NDDSHOME/lib/java/nddsjava.jar orderedPublisher  <domain_id> <samples_to_send>
java -cp .:$NDDSHOME/lib/java/nddsjava.jar orderedSubscriber <domain_id> <sleep_periods>

The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.
   
While generating the output below, we used values that would capture the most 
interesting behavior. 
This ouput it is from: 
    orderedPublisher <domain_id> 10
    orderedSubscriber <domain_id> 3
   
Publisher Output
================
writing instance0, value->0
writing instance1, value->0
writing instance0, value->1
writing instance1, value->1
writing instance0, value->2
writing instance1, value->2
writing instance0, value->3
writing instance1, value->3
writing instance0, value->4
writing instance1, value->4
writing instance0, value->5
writing instance1, value->5
writing instance0, value->6
writing instance1, value->6
writing instance0, value->7
writing instance1, value->7
writing instance0, value->8
writing instance1, value->8
writing instance0, value->9
writing instance1, value->9

Subscriber Output
=================
Subscriber 0 using Instance access scope
Subscriber 1 using Topic access scope

Sleeping for 4 sec...
-----
Reader 0: Instance0->value = 0
Reader 0: Instance0->value = 1
Reader 0: Instance0->value = 2
Reader 0: Instance0->value = 3
Reader 0: Instance1->value = 0
Reader 0: Instance1->value = 1
Reader 0: Instance1->value = 2
Reader 0: Instance1->value = 3
    Reader 1: Instance0->value = 0
    Reader 1: Instance1->value = 0
    Reader 1: Instance0->value = 1
    Reader 1: Instance1->value = 1
    Reader 1: Instance0->value = 2
    Reader 1: Instance1->value = 2
    Reader 1: Instance0->value = 3
    Reader 1: Instance1->value = 3

Sleeping for 4 sec...
-----
Reader 0: Instance0->value = 4
Reader 0: Instance0->value = 5
Reader 0: Instance0->value = 6
Reader 0: Instance0->value = 7
Reader 0: Instance1->value = 4
Reader 0: Instance1->value = 5
Reader 0: Instance1->value = 6
Reader 0: Instance1->value = 7
    Reader 1: Instance0->value = 4
    Reader 1: Instance1->value = 4
    Reader 1: Instance0->value = 5
    Reader 1: Instance1->value = 5
    Reader 1: Instance0->value = 6
    Reader 1: Instance1->value = 6
    Reader 1: Instance0->value = 7
    Reader 1: Instance1->value = 7

Sleeping for 4 sec...
-----
Reader 0: Instance0->value = 8
Reader 0: Instance0->value = 9
Reader 0: Instance1->value = 8
Reader 0: Instance1->value = 9
    Reader 1: Instance0->value = 8
    Reader 1: Instance1->value = 8
    Reader 1: Instance0->value = 9
    Reader 1: Instance1->value = 9