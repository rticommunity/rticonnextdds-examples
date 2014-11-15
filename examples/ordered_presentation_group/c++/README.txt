===========================================
 Example Code -- Ordered Presentation Group
===========================================

Building C++ Example
==================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2008 or i86Linux2.6gcc4.4.5). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2005) run:

rtiddsgen -language C++ -example i86Win32VS2008 ordered_group.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language C++ -example i86Linux2.6gcc4.4.5 ordered_group.idl

You will see messages that look like this:


After running rtiddsgen like this...

File C:\local\ordered_presentation_group\c++\ordered_group_subscriber.cxx 
already exists and will not be replaced with updated content. If you would like
to get a new file with the new content, either remove this file or supply 
-replace option.
File C:\local\ordered_presentation_group\c++\ordered_group_publisher.cxx already
exists and will not be replaced with updated content. If you would like to get 
a new file with the new content, either remove this file or supply -replace 
option.
File C:\local\ordered_presentation_group\c++\USER_QOS_PROFILES.xml already 
exists and will not be replaced with updated content. If you would like to get 
a new file with the new content, either remove this file or supply -replace 
option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Running C++ Example
=================
In two separate command prompt windows for the publisher and subscriber. Run
the following commands from the example directory (this is necessary to ensure
the application loads the QoS defined in USER_QOS_PROFILES.xml):

Windows systems:

    * objs\<arch_name>\ordered_group_publisher.exe <domain_id> <samples_to_send>
    * objs\<arch_name>\ordered_group_subscriber.exe <domain_id> <sleep_periods>

UNIX systems:

    * ./objs/<arch_name>/ordered_group_publisher <domain_id> <samples_to_send>
    * ./objs/<arch_name>/ordered_group_subscriber <domain_id> <sleep_periods>

The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.
   
While generating the output below, we used values that would capture the most 
interesting behavior. 
This ouput it is from: 
    objs\<arch_name>\ordered_group_publisher.exe <domain_id> 10
    objs\<arch_name>\ordered_group_subscriber.exe <domain_id> 3

Publisher Output
================
Writing ordered_group, count 0
Writing ordered_group, count 1
Writing ordered_group, count 2
Writing ordered_group, count 3
Writing ordered_group, count 4
Writing ordered_group, count 5
Writing ordered_group, count 6
Writing ordered_group, count 7
Writing ordered_group, count 8
Writing ordered_group, count 9

Subscriber Output
=================
ordered_group subscriber sleeping for 4 sec...                        
    
   message: "First sample, Topic 1 sent by DataWriter number 1"       
    
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
   
   message: "First sample, Topic 2 sent by DataWriter number 2"       
   
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
   
   message: "First sample, Topic 3 sent by DataWriter number 3"       
    
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
   
   message: "First sample, Topic 1 sent by DataWriter number 1"       
    
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
   
   message: "First sample, Topic 2 sent by DataWriter number 2"       
   
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
   
   message: "First sample, Topic 3 sent by DataWriter number 3"       
   
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
ordered_group subscriber sleeping for 4 sec...                        
   
   message: "First sample, Topic 1 sent by DataWriter number 1"       
   
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
   
   message: "First sample, Topic 2 sent by DataWriter number 2"       
   
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
   
   message: "First sample, Topic 3 sent by DataWriter number 3"       
   
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
  
   message: "First sample, Topic 1 sent by DataWriter number 1"       
  
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
  
   message: "First sample, Topic 2 sent by DataWriter number 2"       
   
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
   
   message: "First sample, Topic 3 sent by DataWriter number 3"       
  
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
  
   message: "First sample, Topic 1 sent by DataWriter number 1"       
  
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
 
   message: "First sample, Topic 2 sent by DataWriter number 2"       
   
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
  
   message: "First sample, Topic 3 sent by DataWriter number 3"       
  
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
   
   message: "First sample, Topic 1 sent by DataWriter number 1"       
   
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
  
   message: "First sample, Topic 2 sent by DataWriter number 2"       
   
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
  
   message: "First sample, Topic 3 sent by DataWriter number 3"       
  
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
ordered_group subscriber sleeping for 4 sec...                        
   
   message: "First sample, Topic 1 sent by DataWriter number 1"       
  
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
  
   message: "First sample, Topic 2 sent by DataWriter number 2"       
  
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
 
   message: "First sample, Topic 3 sent by DataWriter number 3"       
  
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
   
   message: "First sample, Topic 1 sent by DataWriter number 1"       
  
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
 
   message: "First sample, Topic 2 sent by DataWriter number 2"       
 
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
 
   message: "First sample, Topic 3 sent by DataWriter number 3"       
   
   message: "Second sample, Topic 3 sent by DataWriter number 3"      
  
   message: "First sample, Topic 1 sent by DataWriter number 1"       
  
   message: "Second sample, Topic 1 sent by DataWriter number 1"      
   
   message: "First sample, Topic 2 sent by DataWriter number 2"       
  
   message: "Second sample, Topic 2 sent by DataWriter number 2"      
  
   message: "First sample, Topic 3 sent by DataWriter number 3"       
   
   message: "Second sample, Topic 3 sent by DataWriter number 3"      