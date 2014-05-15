===========================================
 Example Code -- MultiChannel
===========================================

Building C Example
==================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2005 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2005) run:

    rtiddsgen -language C -example i86Win32VS2005 market_data.idl

...you will see messages that look like this:

File C:\.\market_data_subscriber.c already exists and will not be replaced
 with updated content. If you would like to get a new file with the new
 content, either remove this file or supply -replace option.
File C:\.\market_data_publisher.c already exists and will not be replaced
 with updated content. If you would like to get a new file with the new
 content, either remove this file or supply -replace option.
File C:\.\USER_QOS_PROFILES.xml already exists and will not be replaced
 with updated content. If you would like to get a new file with the new 
 content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code 
has not been replaced, which is fine since all the source files for the example
are already provided.

Use the generated makefile of visual studio project to compile your application

Running C Example
=================
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in USER_QOS_PROFILES.xml):

Windows systems:

    * objs\<arch>\market_data_publisher.exe <domain#> 400
    * objs\<arch>\market_data_subscriber.exe <domain#> 10    

UNIX systems:

    * objs/<arch>/market_data_publisher.exe <domain#> 400
    * objs/<arch>/market_data_subscriber.exe <domain#> 10    

The applications accept two arguments:

   1. The <domain #>. Both applications must use the same domain # in order to 
   communicate. The default is 0.
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