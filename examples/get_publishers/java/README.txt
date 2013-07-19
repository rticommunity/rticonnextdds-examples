===========================================
 Example Code -- get_publishers
===========================================

Building JAVA Example
====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2005jdk or i86Linux2.6gcc4.4.3jdk). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems (assuming you want to generate an example for 
i86Win32VS2005) run:

rtiddsgen -language Java -example i86Win32VS2005jdk Foo.idl

File FooSubscriber.java already exists and will not be replaced with updated content. 

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

To compile run 

gmake -f makefile_Foo_<arch>

Running JAVA Example
===================
Run the following command

gmake -f makefile_Foo_<arch> FooSubscriber
