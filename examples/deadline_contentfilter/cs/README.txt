=================================================
 Example Code -- Deadline Content Filter Example
=================================================

Building C# Example
===================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2010). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2010) run:

rtiddsgen -language C# -example i86Win32VS2010 -ppDisable deadline_contentfilter.idl

Note: If you are using Visual Studio Express add the -express option to the 
command, i.e.,

rtiddsgen -language C# -example i86Win32VS2010 -express -ppDisable deadline_contentfilter.idl

...you will see messages that look like this:

File C:\local\deadline_contentfilter\cs\deadline_contentfilter_subscriber.cxx 
already exists and will not be replaced with updated content. If you would like 
to get a new file with the new content, either remove this file or 
supply -replace option.
File C:\local\deadline_contentfilter\cs\deadline_contentfilter_publisher.cxx 
already exists and will not be replaced with updated content. If you would like
to get a new file with the new content, either remove this file or 
supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Rtiddsgen generates two solutions for Visual Studio C++ and C#, that you will 
use to build the types and the C# example, respectively. First open  
deadline_contentfilter_type-dotnet4.0.sln and build the solution. Once you've 
done that, open deadline_contentfilter_example-csharp.sln and build the C# 
example.

Running C# Example
==================
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

bin\<build_type>-VS2010\deadline_contentfilter_publisher.exe  <domain_id> <samples_to_send>
bin\<build_type>-VS2010\deadline_contentfilter_subscriber.exe <domain_id> <sleep_periods>

The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.

While generating the output below, we used values that would capture the most 
interesting behavior.
This ouput it is from: 
    bin\<build_type>-VS2010\cfc_publisher.exe <domain_id> 13
    bin\<build_type>-VS2010\cfc_subscriber.exe <domain_id> 15

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