Applies to RTI Data Distribution Service 4.4d and above. The following example has been written in C.

Purpose
=======
We are going to create participants, publishers, subscribers, writers, and readers with different QoS settings. The QoS settings will be obtained from XML QoS profile libraries. There will be two XML profiles files: a file with the same name as the default RTI XML profiles file and an XML profiles file whose name is user-created. Both files modify the default middleware QoS settings.

Some of the questions that this example tries to answer include:

    * What should the XML file look like?
    * How do I tell the participant factory which XML files it should use to find profiles?
    * What's the difference between a (default) profile and a (default) QoS?
    * What are the differences between set_default_profile(), set_qos_with_profile(), and set_default_<entity>_qos_with_profile()?
    * What if a reader has no default profile? Does it inherit the subscriber's default profile?
    * Can a reader's QoS come from a different profile than its subscriber's QoS?

Before we dive into this example, let's first understand the terminology that the QoS profiles API uses.

A profile is a collection of one or more of the following QoS'es: participant_qos, publisher_qos, subscriber_qos, writer_qos, and reader_qos. Within a profile, there is at most one participant_qos, at most one publisher_qos, etc. Each profile has a name.
A default profile is the name of the profile that is used whenever NULL is passed into a function as the profile_name argument.
To perform a function with_profile() is to use the appropriate QoS in that profile to perform that function.

A QoS is one of the aforementioned <entity>_qos'es. So a profile can contain up to five QoS'es, one for each type of entity.
A default QoS is the collection of QoS policies that defines one of the DDS_<entity>_QOS_DEFAULT special values.

In this example, the QoS'es of the publishers and writers are set using a more straightforward method, and those of the subscribers and readers are set using more complicated methods for illustration purposes.

PARTICIPANT:
As a parenthetical, the Transport Properties example in section 'Setting Builtin Transport Properties of the Default Transport Instance' of the User's Manual is reproduced in its context in myReaderProfiles.xml, with the values replaced by the default values of 9216.

PUBLISHER:
The publisher's QoS profiles reside in NDDS_QOS_PROFILES.xml.
Notice that since this XML file falls under the category of <Application directory>/NDDS_QOS_PROFILES.xml (see User's Manual section 'How to Load XML-Specified QoS Setting'), it is loaded automatically when the first domain participant is created, and you don't have to write code to load it.

Using straightforward create_publisher/datawriter_with_profile() calls, we create two publishers with one writer each:

    * writer1: partition A, exclusive, writes odd samples.
    * writer2: partition B, shared, writes even samples.

Both writers write continuously.

SUBSCRIBER:
The subscriber's QoS profiles reside in myReaderProfiles.xml.
To tell the participant factory to use this XML file to find profiles, we add a URL group to the url_profile.
Using various API calls relating to defaults, we create two subscribers and three readers:

    * subscriber1: partition A
          o reader1: middleware default QoS (DDS_Subscriber_set_default_profile() by itself has no effect on reader1 if reader1 is created with DDS_DATAREADER_QOS_DEFAULT)
          o reader2: time-based filtered, exclusive
    * subscriber2: partition A
          o reader3: non-filtered exclusive

subscriber1 illustrates the relationship between set_default_profile() and set_default_datareader_qos_with_profile():
set_default_profile() sets the default value of the profile_name parameter, which enables you to pass a profile_name value of NULL into set_default_datareader_qos_with_profile(), which uses the reader_qos in the default profile to define DDS_DATAREADER_QOS_DEFAULT.

Another point is that reader2's default QoS comes from a different profile from subscriber1's default QoS, so we see that a reader's default QoS is completely independent from its subscriber's default QoS, whether the two come from the same profile or not.

The goal of subscriber2 is to illustrate that subscriber1 cannot set the default QoS for other subscribers. Only the domain participant can do that.
The creation of reader3 points out that using set_qos_with_profile() is not always possible because there are certain QoS policies that cannot be changed once the reader has been created.

reader1 attempts and fails to read the first half of writer1's samples.
readers 2 and 3 successfully read the second half of writer1's samples, with reader2 reading every other sample.

More detailed comments can be found in the code.

Building
========
After building the example, put NDDS_QOS_PROFILES.xml and myReaderProfiles.xml in the objs\<arch> directory.

Make sure you are using one of the relevant RTI Data Distribution Service versions, as specified at the top of the Solution.

Before compiling or running the example, make sure the environment variable NDDSHOME is set to the directory where your version of RTI Data Distribution Service is installed.

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32VS2005). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\QoS_Profiles\c> rtiddsgen -language C -example i86Win32VS2005 profiles.idl

...you will see messages that look like this:

File C:\local\QoS_Profiles\c\profiles_subscriber.c already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File C:\local\QoS_Profiles\c\profiles_publisher.c already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has not been replaced, which is fine since all the source files for the example are already provided.

Running
=======
In two separate command prompt windows for the publisher and subscriber, navigate to the objs\<arch> directory and run these commands:

Windows systems:

    * profiles_publisher.exe <domain #> 20
    * profiles_subscriber.exe <domain #> 8

UNIX systems:

    * ./profiles_publisher <domain #> 20
    * ./profiles_subscriber <domain #> 8

The two arguments are the following:

   1. The <domain #>. Both applications must use the same domain # in order to communicate. The default is 0.
   2. How long the examples should run, measured in samples. A value of '0' instructs the application to run forever; this is the default.

While generating the output below, we used values that would capture the most interesting behavior.

Publisher Output
=============
writer1: writing x = 1
writer2: writing x = 2
writer1: writing x = 3
writer2: writing x = 4
writer1: writing x = 5
writer2: writing x = 6
writer1: writing x = 7
writer2: writing x = 8
writer1: writing x = 9
writer2: writing x = 10
writer1: writing x = 11
writer2: writing x = 12
writer1: writing x = 13
writer2: writing x = 14
writer1: writing x = 15
writer2: writing x = 16
writer1: writing x = 17
writer2: writing x = 18
writer1: writing x = 19
writer2: writing x = 20

Subscriber Output
==============
reader1 sleeping for 1 sec...
reader 8388608: requested incompatible QoS
reader1 sleeping for 1 sec...
reader1 sleeping for 1 sec...
reader1 sleeping for 1 sec...
readers 2 and 3 sleeping for 1 sec...
reader 8388609:

   code: 0
   x: 11
reader 8388610:

   code: 0
   x: 11
readers 2 and 3 sleeping for 1 sec...
reader 8388610:

   code: 0
   x: 13
readers 2 and 3 sleeping for 1 sec...
reader 8388609:

   code: 0
   x: 15
reader 8388610:

   code: 0
   x: 15
readers 2 and 3 sleeping for 1 sec...
reader 8388610:

   code: 0
   x: 17