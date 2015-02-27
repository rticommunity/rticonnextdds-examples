===========================================
 Example Code -- get_publishers
===========================================

Building Java Example
=====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2005 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems run:

rtiddsgen -language Java -example i86Win32VS2010 Foo.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language Java -example i86Linux2.6gcc4.4.3 Foo.idl

You will see messages that look like this:

File C:\local\get_publishers\java\FooPublisher.java already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.
File C:\local\get_publishers\java\FooPublisher.java already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

On Windows systems run:

javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar Foo.java FooSeq.java FooTypeSupport.java FooTypeCode.java FooDataReader.java FooDataWriter.java FooSubscriber.java FooPublisher.java

On Unix systems (including Linux and MacOS X):

javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar Foo.java FooSeq.java FooTypeSupport.java FooTypeCode.java FooDataReader.java FooDataWriter.java FooSubscriber.java FooPublisher.java

Running Java Example
====================
Run the following command from the example directory (this is necessary to 
ensure the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

java -cp .;%NDDSHOME%\lib\java\nddsjava.jar FooPublisher 

On Unix systems (including Linux and MacOS X) run:

java -cp .:$NDDSHOME/lib/java/nddsjava.jar FooPublisher 
