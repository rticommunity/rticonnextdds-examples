Applies to RTI Data Distribution Service 4.5x and above. The following example has been written in Java. C and C# versions will be posted at a later date.

Purpose
=======
We are going to create participants, publishers, subscribers, writers, and readers with different QoS settings. The QoS settings will be obtained from XML QoS profile libraries. There will be two XML profiles files: a file with the same name as the default RTI XML profiles file and an XML profiles file whose name is user-created. Both files modify the default middleware QoS settings.

The questions that this example tries to answer include:

    * What should the XML file look like?
    * How do I tell the participant factory which XML files it should use to find profiles?

Before we dive into this example, let's first understand the terminology that the QoS profiles API uses.

A profile is a collection of one or more of the following QoS'es: participant_qos, publisher_qos, subscriber_qos, writer_qos, and reader_qos. Within a profile, there is at most one participant_qos, at most one publisher_qos, etc. Each profile has a name.
A default profile is the name of the profile that is used whenever NULL is passed into a function as the profile_name argument.
To perform a function with_profile() is to use the appropriate QoS in that profile to perform that function.

In this example, the QoS'es of the publishers and writers are set using a more straightforward method, and those of the subscribers and readers are set using more complicated methods for illustration purposes.

PARTICIPANT:
------------
This example changes the participant_name to "RTI Profiles Example C++"  

This is loaded from the "volatile_profile" QoS profile that is loaded automatically from the USER_QOS_PROFILES.xml.  This is used as the default QoS profile by the application because the attribute is_default_qos is set to true:
is_default_qos="true"

Note that the USER_QOS_PROFILES.xml file is only loaded if it is located in the current working directory.

PUBLISHER AND DATAWRITERS:
--------------------------
The publisher's QoS profiles reside in NDDS_QOS_PROFILES.xml.

Notice that since this XML file falls under the category of <Application directory>/NDDS_QOS_PROFILES.xml (see User's Manual 15.2), it is loaded automatically when the first domain participant is created, and you don't have to write code to load it.

Using straightforward create_publisher/datawriter_with_profile() calls, we create two publishers with one writer each:

    * writer1: transient local durability
    * writer2: volatile durability (from the default profile)

The volatile DataWriter uses the "volatile_profile" QoS profile, that is loaded automatically from the USER_QOS_PROFILES.xml file.  This is used as the default QoS profile by the application because the attribute is_default_qos is set to true:
is_default_qos="true"

Note that the USER_QOS_PROFILES.xml file is only loaded if it is located in the current working directory.

The transient local DataWriter uses the "transient_local_profile" from the my_custom_qos_profiles.xml file that is loaded by the DomainParticipantFactory.  Because this is not set to be the default QoS profile, the application explicitly uses it by calling create_datawriter_with_profile().


Both writers write continuously.

SUBSCRIBER AND DATAREADERS:
---------------------------
The subscriber's QoS profiles reside in NDDS_QOS_PROFILES.xml.

This subscriber creates two DataReaders, one with volatile QoS settings, and one with transient local QoS settings.  

The volatile reader uses the "volatile_profile" QoS profile, that is loaded automatically from the USER_QOS_PROFILES.xml file.  This is used as the default QoS profile by the application because the attribute is_default_qos is set to true:
is_default_qos="true"

Note that the USER_QOS_PROFILES.xml file is only loaded if it is located in the current working directory.

The transient local DataReader uses the "transient_local_profile" from the my_custom_qos_profiles.xml file that is loaded by the DomainParticipantFactory.  Because this is not set to be the default QoS profile, the application explicitly uses it by calling create_datareader_with_profile().

The volatile DataReader will receive the data from both of the DataWriters.


Building Java Example
=====================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32jdk or i86Linux2.6gcc4.4.3jdk). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems run:

rtiddsgen -language Java -example i86Win32jdk profiles.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

rtiddsgen -language Java -example i86Linux2.6gcc4.4.3jdk profiles.idl

You will see messages that look like this:

File C:\local\profiles\java\profilesSubscriber.java already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.
File C:\local\profiles\java\profilesPublisher.java already exists and 
will not be replaced with updated content. If you would like to get a new file 
with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has 
not been replaced, which is fine since all the source files for the example are 
already provided.

Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

On Windows systems run:

javac -classpath .;%NDDSHOME%\class\nddsjava.jar profiles.java profilesSeq.java profilesTypeSupport.java profilesTypeCode.java profilesDataReader.java profilesDataWriter.java profilesSubscriber.java profilesPublisher.java

On Unix systems (including Linux and MacOS X):

javac -classpath .:$NDDSHOME/class/nddsjava.jar profiles.java profilesSeq.java profilesTypeSupport.java profilesTypeCode.java profilesDataReader.java profilesDataWriter.java profilesSubscriber.java waitsetsPublisher.java

After building the example, put USER_QOS_PROFILES.xml and my_custom_qos_profiles.xml in the directory you will run your application from.


Running Java Example
====================
Before running, make sure that the native Java libraries on which RTI Connext
depends are in your environment path (or library path). To add Java libraries 
to your environment...

On Windows systems run: 
set PATH=%NDDSHOME%\lib\i86Win32jdk;%PATH%

On Unix systems except MacOS X (assuming you are using Bash) run:
export LD_LIBRARY_PATH=$NDDSHOME/lib/<platform_name>jdk:$LD_LIBRARY_PATH

On MacOSX (assuming your are using Bash) run:
export DYLD_LIBRARY_PATH=$NDDSHOME/lib/<platform_name>jdk:$DYLD_LIBRARY_PATH

Then, in two separate command prompt windows for the publisher and subscriber. 
Run the following commands from the example directory (this is necessary to 
ensure the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

java -cp .;%NDDSHOME%\class\nddsjava.jar profilesPublisher  <domain_id> <samples_to_send>
java -cp .;%NDDSHOME%\class\nddsjava.jar profilesSubscriber <domain_id> <sleep_periods>

On Unix systems (including Linux and MacOS X) run:

java -cp .:$NDDSHOME/class/nddsjava.jar profilesPublisher  <domain_id> <samples_to_send>
java -cp .:$NDDSHOME/class/nddsjava.jar profilesSubscriber <domain_id> <sleep_periods>


The applications accept two arguments:

   1. The <domain_id>. Both applications must use the same domain ID in order to 
   communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.


Publisher Output
================

Writing profiles, count 0
Writing profile_name = volatile_profile x = 0
Writing profile_name = transient_local_profile x = 0
Writing profiles, count 1
Writing profile_name = volatile_profile x = 1
Writing profile_name = transient_local_profile x = 1
Writing profiles, count 2
Writing profile_name = volatile_profile x = 2
Writing profile_name = transient_local_profile x = 2
Writing profiles, count 3
Writing profile_name = volatile_profile x = 3
Writing profile_name = transient_local_profile x = 3

Subscriber Output
=================
Notice that the volatile DataReader receives both transient local and volatile data, because the QoS is request/offered, and volatile QoS matches with the higher transient local level of service.

profiles subscriber sleeping for 4 sec...
TransientLocalProfile listener received

profiles subscriber sleeping for 4 sec...
======================================

Received:
    profile_name: transient_local_profile
    x: 0

profiles subscriber sleeping for 4 sec...
========================================
VolatileProfile listener received

======================================

Received:
    profile_name: volatile_profile
    x: 1

========================================
VolatileProfile listener received

======================================

Received:
    profile_name: transient_local_profile
    x: 1

