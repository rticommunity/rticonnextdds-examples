===============================
Example Code - Builtin Topics
===============================

Purpose 
======
PUBLISHER: 
This publisher example shows how to attach listeners to Builtin Topics. Each participant has a builtin subscriber with three topics. These subscriptions receive data when the participant gets information about another participant, datareader or datawriter. 

Listening to these topics has several uses. For instance, we can see discovery information, read user_data, or check the typecodes other entities are using. 

This example shows how user_data can be used for security. We check user_data fields for discovered participants and datareaders. If the participant user_data matches an authorization string, we allow all datareaders from that participant read access. Otherwise, we check the user_data from individual datareaders to see if it is authorized. 

This exposes two points. First, even though the builtin topics correspond to a hierarchy of participants -> datawriters/datareaders, there is no hierarchy of listener callbacks. If, as in this example, we listen for participant and datareader information, both listeners will get the on_data_available() callbacks. If we want to track relationships between discovered participants and readers/writers, we may do so by keys. 

Second, the callbacks for participant discovery are guaranteed to fire before the callbacks for readers or writers that are children of that participant. 


SUBSCRIBER: 
This shows how to set the user_data qos fields for participants and data readers. To do this, we first get the relevant default qos, then add our data as an DDS_OctetSeq. These Octets are opaque to DDS, and will not undergo any conversions during transmission. 

This process is identical for topics, publishers and subscribers, except for the specific field names. The topic qos field is topic_data, while the publisher and subscriber fields are called group_data. 

Building
========

Before compiling or running the example, make sure the environment variable NDDSHOME is set to the directory where your version of RTI Data Distribution Service is installed.

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32VS2005). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\Builtin_Topics\c++> rtiddsgen -language C++ -example i86Win32VS2005 msg.idl

...you will see messages that look like this:

File C:\local\Builtin_Topics\c++\msg_subscriber.cxx already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File C:\local\Builtin_Topics\c++\msg_publisher.cxx already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has not been replaced, which is fine since all the source files for the example are already provided.

Running
======
C/C++:
In two separate command prompt windows for the publisher and subscriber, navigate to the objs/<arch> directory and run these commands:

Windows systems:
	•	msg_publisher.exe <domain#> 3
	•	msg_subscriber.exe <domain#> 5 password foo
UNIX systems:
	•	./msg_publisher <domain#> 3
	•	./msg_subscriber <domain#> 5 password foo
Java:
Before running, make sure that the desired version of the java compiler is in your PATH environment variable.

In two separate command prompt windows for the publisher and subscriber, run these commands:
	•	java -cp .\;%nddshome%\class\nddsjava.jar msgPublisher <domain#> 3
	•	java -cp .\;%nddshome%\class\nddsjava.jar msgSubscriber <domain#> 5 password foo
The applications accept up to four arguments:
	1.	The <domain #>. Both applications must use the same domain # in order to communicate. The default is 0.
	2.	How long the examples should run, measured in samples for the publisher and sleep periods for the subscriber. A value of '0' instructs the application to run forever; this is the default.
	3.	(subscriber only) The participant authorization string. This is checked against the authorization that the publisher is expecting. The default is "password".
	4.	(subscriber only) The reader authorization string. If and only if the reader's participant didn't get authorized, then this value is checked against the authorization that the publisher is expecting. The default is "Reader_Auth".
While generating the output below, we used values that would capture the most interesting behavior.

Publisher Output (Note: the C++ version has some additional output. See the README for details.)
============
Built-in Reader: found participant 
    key->'c0a8a7d713d40001000001c1'
    user_data->'password'
Built-in Reader: found subscriber 
    participant_key->'c0a8a7d713d40001000001c1'
    user_data->'foo'
Writing msg, count 0
Writing msg, count 1
Writing msg, count 2

Subscriber Output
=============
   x: 0
   x: 1
   x: 2
