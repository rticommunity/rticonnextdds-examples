===========================================
 Example Code -- MultiChannel
===========================================

Building Java Example
=====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2010 or i86Linux2.6gcc4.6.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems run:

rtiddsgen -language Java -example i86Win32VS2010 market_data.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language Java -example i86Linux2.6gcc4.6.3 market_data.idl

You will see messages that look like this:

File C:\local\MultiChannel\java\market_dataSubscriber.java already exists and 
 will not be replaced with updated content. If you would like to get a new file
 with the new content, either remove this file or supply -replace option.
File C:\local\MultiChannel\java\market_dataPublisher.java already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.
File C:\local\MultiChannel\java\USER_QOS_PROFILES.xml already exists and will 
not be replaced with updated content. If you would like to get a new file with 
the new content, either remove this file or supply -replace option.

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

    java -cp .;%NDDSHOME%\lib\java\nddsjava.jar market_dataPublisher <domain#> 400
    java -cp .;%NDDSHOME%\lib\java\nddsjava.jar market_dataSubscriber <domain#> 10

On Unix systems (including Linux and MacOS X) run:
    java -cp .:$NDDSHOME/lib/java/nddsjava.jar market_dataPublisher <domain#> 400
    java -cp .:$NDDSHOME/lib/java/nddsjava.jar market_dataSubscriber <domain#> 10

   The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.
   
While generating the output below, we used values that would capture the most 
interesting behavior.

Wireshark Output
================
<See multichannel.pcap. Observe that data samples are being sent on the wire in 
 only the channels that are necessary for delivering samples that pass the 
 reader's filter.>
<In contrast, singlechannel.pcap shows the wire traffic when multichannel is not
 used and when the reader specifies a multicast.receive_address of 239.255.0.1.
 Observe that all data samples are being sent on the wire, i.e. reader-side
 filtering, since there is only one channel.>

market_data Publisher Output
===========================
<nothing>

market_data Subscriber Output
============================
filter is Symbol MATCH 'A'

   Symbol: "A"
   Price: 26.000000

   Symbol: "A"
   Price: 52.000000

   Symbol: "A"
   Price: 78.000000

   Symbol: "A"
   Price: 104.000000
changed filter to Symbol MATCH 'AD'

   Symbol: "A"
   Price: 130.000000

   Symbol: "D"
   Price: 133.000000

   Symbol: "A"
   Price: 156.000000

   Symbol: "D"
   Price: 159.000000

   Symbol: "A"
   Price: 182.000000

   Symbol: "D"
   Price: 185.000000

   Symbol: "A"
   Price: 208.000000

   Symbol: "D"
   Price: 211.000000

   Symbol: "A"
   Price: 234.000000

   Symbol: "D"
   Price: 237.000000
changed filter to Symbol MATCH 'D'

   Symbol: "D"
   Price: 263.000000

   Symbol: "D"
   Price: 289.000000

   Symbol: "D"
   Price: 315.000000

   Symbol: "D"
   Price: 341.000000

   Symbol: "D"
   Price: 367.000000

   Symbol: "D"
   Price: 393.000000