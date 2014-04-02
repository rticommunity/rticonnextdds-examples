===========================================
 Example Code -- Using_Qos_profiles
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

rtiddsgen -language C -example i86Win32VS2005 profiles.idl

...you will see messages that look like this:

File C:\local\using_qos_profiles\c\profiles_subscriber.c already exists and will 
not be replaced with updated content. If you would like to get a new file with the 
new content, either remove this file or supply -replace option.
File C:\local\using_qos_profiles\c\profiles_publisher.c already exists and will not 
be replaced with updated content. If you would like to get a new file with the new 
content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Running C Example
=================
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in USER_QOS_PROFILES.xml and 
my_custom_qos_profiles.xml):


On Windows systems run:

objs\<arch_name>\profiles_publisher.exe <domain #> 20
objs\<arch_name>\profiles_subscriber.exe <domain #> 8

UNIX systems:

./objs/<arch_name>/profiles_publisher  <domain_id> 20
./objs/<arch_name>/profiles_subscriber <domain_id> 8

The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples. A value of '0' 
   instructs the application to run forever; this is the default.
