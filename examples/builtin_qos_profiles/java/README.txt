=========================================
Example code: Using Built-in QoS Profiles
=========================================

Building Java Example
=====================
Make sure you are using one of the relevant RTI Data Distribution Service versions, as specified at the top of the Solution.

Before compiling or running the example, make sure the environment variable NDDSHOME is set to the directory where your version of RTI Data Distribution Service is installed.

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32jdk). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\property_qos\java> rtiddsgen -language Java -example i86Win32jdk profiles.idl

...you will see messages that look like this:

File C:\local\builtin_qos_profiles\java\profilesSubscriber.java already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File C:\local\builtin_qos_profiles\java\profilesPublisher.java already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has not been replaced, which is fine since all the source files for the example are already provided.

Before compiling in Java, make sure that the desired version of the javac compiler is in your PATH environment variable.

Running Java Example
====================
Before running, make sure that the desired version of the java compiler is in your PATH environment variable.

In two separate command prompt windows for the publisher and subscriber, run these commands:

    * java -cp .\;%nddshome%\class\nddsjava.jar profilesPublisher <domain#> 4
    * java -cp .\;%nddshome%\class\nddsjava.jar profilesSubscriber <domain#> 4

The applications accept two arguments:

   1. The <domain #>. Both applications must use the same domain # in order to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher and sleep periods for the subscriber. A value of '0' instructs the application to run forever; this is the default.
