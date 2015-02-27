===================================
 Example Code -- Builtin Topics
===================================

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

rtiddsgen -language C# -example i86Win32VS2010 -ppDisable msg.idl

Note: If you are using Visual Studio Express add the -express option to the 
command, i.e.,

rtiddsgen -language C# -example i86Win32VS2010 -express -ppDisable msg.idl

...you will see messages that look like this:

File C:\local\Builtin_Topics\cs\msg_subscriber.cs already exists and will not be
replaced with updated content. If you would like to get a new file with the new
content, either remove this file or supply -replace option.
File C:\local\Builtin_Topics\cs\msg_publisher.cs already exists and will not be
replaced with updated content. If you would like to get a new file with the new
content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Rtiddsgen generates two solutions for Visual Studio C++ and C#, that you will 
use to build the types and the C# example, respectively. First open  
msg_type-dotnet4.0.sln and build the solution. Once you've done that, open
msg_example-csharp.sln and build the C# example.

Running C# Example
==================
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

bin\<build_type>-VS2010\msg_publisher.exe  <domain_id> <samples_to_send>
bin\<build_type>-VS2010\msg_subscriber.exe <domain_id> <sleep_periods> <participant_auth> <reader_auth>

The applications accept up to two arguments (four to subscriber):

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.
   3. (subscriber only) The participant authorization string. This is checked 
   against the authorization that the publisher is expecting. The default is 
   "password".
   4. (subscriber only) The reader authorization string. If and only if the 
   reader's participant didn't get authorized, then this value is checked 
   against the authorization that the publisher is expecting. The default is 
   "Reader_Auth".

While generating the output below, we used values that would capture the most 
interesting behavior.

Publisher Output
================
Built-in Reader: found participant
        key->'1673909612'
        user_data->'password'
instance_handle: 9E02460A.A80A0000.1000000.C1010000
Built-in Reader: found subscriber
        participant_key->'1673909612'
        key-> '-473574231'
        user_data-> 'Reader_Auth'
instance_handle: 9E02460A.A80A0000.1000000.4000080
Writing msg, count 0
Writing msg, count 1

Subscriber Output
=================
   x: 0
   x: 1
   x: 2