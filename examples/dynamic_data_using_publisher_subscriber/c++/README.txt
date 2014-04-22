===========================================================
Example code: Using Dynamic Data to publish and subscribe
===========================================================

Building C++ Example
====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2005 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2005) run:

rtiddsgen -language C++ -example i86Win32VS2005 Shapes.idl

...you will see messages that look like this:

File C:\local\QoS_Profiles\c++\Shapes_subscriber.cxx already exists and will not 
be replaced with updated content. If you would like to get a new file with the 
new content, either remove this file or supply -replace option.
File C:\local\QoS_Profiles\c++\Shapes_publisher.cxx already exists and will not 
be replaced with updated content. If you would like to get a new file with the 
new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Running C++ Example
===================
--------------------------------------------------------------------------------
                PUBLISH/SUBSCRIBE IN COMMAND PROMPT
--------------------------------------------------------------------------------
In two separate command prompt windows for the publisher and subscriber.

On Windows systems run:

objs\<arch_name>\Shapes_publisher.exe <domain_id> <sample #>
objs\<arch_name>\Shapes_subscriber.exe <domain_id> <sample #>

UNIX systems:

./objs/<arch_name>/Shapes_publisher  <domain_id> <sample #>
./objs/<arch_name>/Shapes_subscriber <domain_id> <sample #>

The applications accept up to two arguments:

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples. A value of '0' 
   instructs the application to run forever; this is the default.

--------------------------------------------------------------------------------
         INTERCONNECTION BETWEEN THIS EXAMPLE AND 'SHAPES DEMO' TOOL
--------------------------------------------------------------------------------

First of all, remember that you have to work in the same domain both in
'Shape Demo' and your command prompt.

A. Publish using this example and subscribe using 'Shapes Demo': 
    1. Open a new 'Shapes Demo' window.
    2. Create a new Square subscriber with default options.
    3. Run the publisher in a new command prompt like explained before (use the
    <sample #> = 0 option to run an indefinitely).
    
    You will see a Square moving from left to right. Also, its size will 
    increase until it reaches a maximum size. When the max size or the end of 
    the canvas are reached, it restarts.
    
 B. Subscribe using this example and publish using 'Shapes Demo':
    1. Open a new command prompt and create a new subscriber like explained 
    before.
    2. Create a publisher (Square) in 'Shapes Demo'.
       
    You will see the full description of the data published by 'Shapes Demo' in 
    your command prompt.
