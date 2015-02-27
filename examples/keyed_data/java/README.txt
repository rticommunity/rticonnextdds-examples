===========================================
 Example Code -- Time based filters
===========================================

Building Java Example
=====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2005 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems run:

rtiddsgen -language Java -example i86Win32VS2010 keys.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language Java -example i86Linux2.6gcc4.4.3 keys.idl


File C:\local\keyed_data\java\keysSubscriber.java already exists and will
not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.
File C:\local\keyed_data\java\keysPublisher.java already exists and will 
not be replaced with updated content. If you would like to get a new file with 
the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

On Windows systems run:

javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar keys.java keysSeq.java keysTypeSupport.java keysTypeCode.java keysDataReader.java keysDataWriter.java keysSubscriber.java keysPublisher.java

On Unix systems (including Linux and MacOS X):

javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar keys.java keysSeq.java keysTypeSupport.java keysTypeCode.java keysDataReader.java keysDataWriter.java keysSubscriber.java keysPublisher.java

Running Java Example
====================
In two separate command prompt windows for the publisher and subscriber. 
Run the following commands from the example directory (this is necessary to 
ensure the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

java -cp .;%NDDSHOME%\lib\java\nddsjava.jar keysPublisher  <domain_id> 16
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar keysSubscriber <domain_id> 18

On Unix systems (including Linux and MacOS X) run:

java -cp .:$NDDSHOME/lib/java/nddsjava.jar keysPublisher  <domain_id> 16
java -cp .:$NDDSHOME/lib/java/nddsjava.jar keysSubscriber <domain_id> 18

The applications accept two arguments:
   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.
   While generating the output below, we used values that would capture the most 
   interesting behavior.


Publisher Output
=============
Registering instance 0
Writing instance 0, x: 1000, y: 0
Writing instance 0, x: 1000, y: 1
Writing instance 0, x: 1000, y: 2
Writing instance 0, x: 1000, y: 3
Writing instance 0, x: 1000, y: 4
----Registering instance 1
----Registering instance 2
Writing instance 0, x: 1000, y: 5
Writing instance 1, x: 2000, y: 5
Writing instance 2, x: 3000, y: 5
Writing instance 0, x: 1000, y: 6
Writing instance 1, x: 2000, y: 6
Writing instance 2, x: 3000, y: 6
Writing instance 0, x: 1000, y: 7
Writing instance 1, x: 2000, y: 7
Writing instance 2, x: 3000, y: 7
Writing instance 0, x: 1000, y: 8
Writing instance 1, x: 2000, y: 8
Writing instance 2, x: 3000, y: 8
Writing instance 0, x: 1000, y: 9
Writing instance 1, x: 2000, y: 9
Writing instance 2, x: 3000, y: 9
----Unregistering instance 1
Writing instance 0, x: 1000, y: 10
Writing instance 2, x: 3000, y: 10
Writing instance 0, x: 1000, y: 11
Writing instance 2, x: 3000, y: 11
Writing instance 0, x: 1000, y: 12
Writing instance 2, x: 3000, y: 12
Writing instance 0, x: 1000, y: 13
Writing instance 2, x: 3000, y: 13
Writing instance 0, x: 1000, y: 14
Writing instance 2, x: 3000, y: 14
----Disposing instance 2
Writing instance 0, x: 1000, y: 15

Subscriber Output
==============
Found new instance; code = 0
Instance 0: x: 1000, y: 0
Instance 0: x: 1000, y: 1
Instance 0: x: 1000, y: 2
Instance 0: x: 1000, y: 3
Instance 0: x: 1000, y: 4
Instance 0: x: 1000, y: 5
Found new instance; code = 1
Instance 1: x: 2000, y: 5
Found new instance; code = 2
Instance 2: x: 3000, y: 5
Instance 0: x: 1000, y: 6
Instance 1: x: 2000, y: 6
Instance 2: x: 3000, y: 6
Instance 0: x: 1000, y: 7
Instance 1: x: 2000, y: 7
Instance 2: x: 3000, y: 7
Instance 0: x: 1000, y: 8
Instance 1: x: 2000, y: 8
Instance 2: x: 3000, y: 8
Instance 0: x: 1000, y: 9
Instance 1: x: 2000, y: 9
Instance 2: x: 3000, y: 9
Instance 1 has no writers
Instance 0: x: 1000, y: 10
Instance 2: x: 3000, y: 10
Instance 0: x: 1000, y: 11
Instance 2: x: 3000, y: 11
Instance 0: x: 1000, y: 12
Instance 2: x: 3000, y: 12
Instance 0: x: 1000, y: 13
Instance 2: x: 3000, y: 13
Instance 0: x: 1000, y: 14
Instance 2: x: 3000, y: 14
Instance 2 disposed
Instance 0: x: 1000, y: 15
Instance 0 has no writers  