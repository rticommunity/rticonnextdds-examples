===========================================
 Example Code -- get_publishers
===========================================

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

rtiddsgen -language C++ -example i86Win32VS2005 Foo.idl

File Foo_subscriber.cxx already exists and willnot be replaced with updated content. 

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

To compile run: 

gmake -f makefile_Foo_<arch>

Running C++ Example
===================
Run the following command

On Windows systems run:

objs\<arch_name>\Foo_publisher.exe

UNIX systems:

./objs/<arch_name>/Foo_publisher  <domain_id>   <samples_to_send>
