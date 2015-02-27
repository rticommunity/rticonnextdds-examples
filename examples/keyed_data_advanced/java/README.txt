===========================================
 Example Code -- Keyed data: Advanced
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


File C:\local\keyed_data_advanced\java\keysSubscriber.java already exists and will
not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.
File C:\local\keyed_data_advanced\java\keysPublisher.java already exists and will 
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

java -cp .;%NDDSHOME%\lib\java\nddsjava.jar keysPublisher  <domain_id> 18
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar keysSubscriber <domain_id> 20

On Unix systems (including Linux and MacOS X) run:

java -cp .:$NDDSHOME/lib/java/nddsjava.jar keysPublisher  <domain_id> 18
java -cp .:$NDDSHOME/lib/java/nddsjava.jar keysSubscriber <domain_id> 20

The applications accept two arguments:
   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.
   While generating the output below, we used values that would capture the most 
   interesting behavior.


Publisher Output
============
----DW1 registering instance 0
DW1 write; code: 0, x: 1, y: 0
DW2 write; code: 1, x: 2, y: -1000
DW1 write; code: 0, x: 1, y: 1
DW2 write; code: 1, x: 2, y: -1001
DW1 write; code: 0, x: 1, y: 2
DW2 write; code: 1, x: 2, y: -1002
DW1 write; code: 0, x: 1, y: 3
DW2 write; code: 1, x: 2, y: -1003
----DW1 registering instance 1
----DW1 registering instance 2
DW1 write; code: 0, x: 1, y: 4
DW1 write; code: 1, x: 1, y: 1004
DW1 write; code: 2, x: 1, y: 2004
DW2 write; code: 1, x: 2, y: -1004
DW1 write; code: 0, x: 1, y: 5
DW1 write; code: 1, x: 1, y: 1005
DW1 write; code: 2, x: 1, y: 2005
DW2 write; code: 1, x: 2, y: -1005
DW1 write; code: 0, x: 1, y: 6
DW1 write; code: 1, x: 1, y: 1006
DW1 write; code: 2, x: 1, y: 2006
DW2 write; code: 1, x: 2, y: -1006
DW1 write; code: 0, x: 1, y: 7
DW1 write; code: 1, x: 1, y: 1007
DW1 write; code: 2, x: 1, y: 2007
DW2 write; code: 1, x: 2, y: -1007
----DW1 disposing instance 1
DW1 write; code: 0, x: 1, y: 8
DW1 write; code: 2, x: 1, y: 2008
DW2 write; code: 1, x: 2, y: -1008
DW1 write; code: 0, x: 1, y: 9
DW1 write; code: 2, x: 1, y: 2009
DW2 write; code: 1, x: 2, y: -1009
----DW1 unregistering instance 1
DW1 write; code: 0, x: 1, y: 10
DW1 write; code: 2, x: 1, y: 2010
DW2 write; code: 1, x: 2, y: -1010
DW1 write; code: 0, x: 1, y: 11
DW1 write; code: 2, x: 1, y: 2011
DW2 write; code: 1, x: 2, y: -1011
----DW1 unregistering instance 2
----DW1 re-registering instance 1
DW1 write; code: 0, x: 1, y: 12
DW1 write; code: 1, x: 1, y: 1012
DW2 write; code: 1, x: 2, y: -1012
DW1 write; code: 0, x: 1, y: 13
DW1 write; code: 1, x: 1, y: 1013
DW2 write; code: 1, x: 2, y: -1013
DW1 write; code: 0, x: 1, y: 14
DW1 write; code: 1, x: 1, y: 1014
DW2 write; code: 1, x: 2, y: -1014
DW1 write; code: 0, x: 1, y: 15
DW1 write; code: 1, x: 1, y: 1015
DW2 write; code: 1, x: 2, y: -1015
----DW1 re-registering instance 2
DW1 write; code: 0, x: 1, y: 16
DW1 write; code: 1, x: 1, y: 1016
DW1 write; code: 2, x: 1, y: 2016
----DW2 disposing instance 1
DW1 write; code: 0, x: 1, y: 17
DW1 write; code: 1, x: 1, y: 1017
DW1 write; code: 2, x: 1, y: 2017

Subscriber Output
=============
New instance found; code = 0
New instance found; code = 1
code: 1, x: 2, y: -1000
code: 1, x: 2, y: -1001
code: 1, x: 2, y: -1002
code: 1, x: 2, y: -1003
code: 1, x: 1, y: 1004
New instance found; code = 2
code: 2, x: 1, y: 2004
code: 1, x: 1, y: 1005
code: 2, x: 1, y: 2005
code: 1, x: 1, y: 1006
code: 2, x: 1, y: 2006
code: 1, x: 1, y: 1007
code: 2, x: 1, y: 2007
Instance disposed; code = 1
code: 2, x: 1, y: 2008
code: 2, x: 1, y: 2009
code: 2, x: 1, y: 2010
Found reborn instance; code = 1
code: 1, x: 2, y: -1010
code: 2, x: 1, y: 2011
code: 1, x: 2, y: -1011
Instance has no writers; code = 2
code: 1, x: 1, y: 1012
code: 1, x: 1, y: 1013
code: 1, x: 1, y: 1014
code: 1, x: 1, y: 1015
code: 1, x: 1, y: 1016
Found writer for instance; code = 2
code: 2, x: 1, y: 2016
code: 1, x: 1, y: 1017
code: 2, x: 1, y: 2017
Instance has no writers; code = 0
Instance has no writers; code = 2
Instance has no writers; code = 1