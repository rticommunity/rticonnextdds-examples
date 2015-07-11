===========================================
 Example Code -- Custom Flowcontroller
===========================================

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

rtiddsgen -language C# -example i86Win32VS2010 -ppDisable cfc.idl

Note: If you are using Visual Studio Express add the -express option to the 
command, i.e.,

rtiddsgen -language C# -example i86Win32VS2010 -express -ppDisable cfc.idl

...you will see messages that look like this:

File C:\local\Custom_Flowcontroller\cs\cfc_subscriber.cs already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.
File C:\local\Custom_Flowcontroller\cs\cfc_publisher.cs already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.
File C:\local\Custom_Flowcontroller\cs\USER_QOS_PROFILES.xml already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Rtiddsgen generates two solutions for Visual Studio C++ and C#, that you will 
use to build the types and the C# example, respectively. First open  
async_type-dotnet4.0.sln and build the solution. Once you've done that, open
cfc_example-csharp.sln and build the C# example.

Running C# Example
==================
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

bin\<build_type>-VS2010\cfc_publisher.exe  <domain_id> <samples_to_send>
bin\<build_type>-VS2010\cfc_subscriber.exe <domain_id> <sleep_periods>

The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.

While generating the output below, we used values that would capture the most 
interesting behavior.
This ouput it is from: 
    bin\<build_type>-VS2010\cfc_publisher.exe <domain_id> 4
    bin\<build_type>-VS2010\cfc_subscriber.exe <domain_id> 3

Publisher Output
================
Writing cfc, sample 0
Writing cfc, sample 1
Writing cfc, sample 2
Writing cfc, sample 3
Writing cfc, sample 4
Writing cfc, sample 5
Writing cfc, sample 6
Writing cfc, sample 7
Writing cfc, sample 8
Writing cfc, sample 9
Writing cfc, sample 10
Writing cfc, sample 11
Writing cfc, sample 12
Writing cfc, sample 13
Writing cfc, sample 14
Writing cfc, sample 15
Writing cfc, sample 16
Writing cfc, sample 17
Writing cfc, sample 18
Writing cfc, sample 19
Writing cfc, sample 20
Writing cfc, sample 21
Writing cfc, sample 22
Writing cfc, sample 23
Writing cfc, sample 24
Writing cfc, sample 25
Writing cfc, sample 26
Writing cfc, sample 27
Writing cfc, sample 28
Writing cfc, sample 29
Writing cfc, sample 30
Writing cfc, sample 31
Writing cfc, sample 32
Writing cfc, sample 33
Writing cfc, sample 34
Writing cfc, sample 35
Writing cfc, sample 36
Writing cfc, sample 37
Writing cfc, sample 38
Writing cfc, sample 39

Subscriber Output
=================
@ t=1.50s, got x = 10
@ t=1.61s, got x = 11
@ t=1.72s, got x = 12
@ t=1.83s, got x = 13
@ t=2.06s, got x = 14
@ t=2.61s, got x = 15
@ t=2.72s, got x = 16
@ t=2.83s, got x = 17
@ t=2.94s, got x = 18
@ t=3.05s, got x = 19
@ t=3.17s, got x = 20
@ t=3.28s, got x = 21
@ t=3.39s, got x = 22
@ t=3.50s, got x = 23
@ t=3.61s, got x = 24
@ t=3.73s, got x = 25