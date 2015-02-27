============================================================================
 Example Code -- Builtin Topics
============================================================================


Building Java Builtin Topic Example
===================================
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your 
choice (e.g., i86Win32VS2005 or i86Linux2.6gcc4.4.3). The RTI Connext Core 
Libraries and Utilities Getting Started Guide describes this process in detail. 
Follow the same procedure to generate the code and build the examples. Do not 
use the -replace option.

On Windows systems run:

    rtiddsgen -language Java -example i86Win32VS2010 msg.idl

On UNIX systems (assuming you want to generate an example for 
i86Linux2.6gcc4.4.3) run:

    rtiddsgen -language Java -example i86Linux2.6gcc4.4.3 msg.idl

You will see messages that look like this:

...you will see messages that look like this:

File C:\local\Builtin_Topics\java\msgSubscriber.java already exists and will not
be replaced with updated content. If you would like to get a new file with the 
new content, either remove this file or supply -replace option.
File C:\local\Builtin_Topics\java\msgPublisher.java already exists and will not
be replaced with updated content. If you would like to get a new file with the 
new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.


Before compiling in Java, make sure that the desired version of the javac 
compiler is in your PATH environment variable.

On Windows systems run:

javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar *.java

On Unix systems (including Linux and MacOS X):

javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar *.java

Running
=======
In two separate command prompt windows for the publisher and subscriber. 
Run the following commands from the example directory (this is necessary to 
ensure the application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

java -cp .;%NDDSHOME%\lib\java\nddsjava.jar msgPublisher  <domain_id> <samples_to_send>
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar msgSubscriber <domain_id> <sleep_periods> <participant_auth> <reader_auth>

On Unix systems (including Linux and MacOS X) run:

java -cp .:$NDDSHOME/lib/java/nddsjava.jar msgPublisher  <domain_id> <samples_to_send>
java -cp .:$NDDSHOME/lib/java/nddsjava.jar msgSubscriber <domain_id> <sleep_periods> <participant_auth> <reader_auth>


The applications accept up to two arguments (four to subscriber):

   1. The <domain_id>. Both applications must use the same domain id in order 
   to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher 
   and sleep periods for the subscriber. A value of '0' instructs the 
   application to run forever; this is the default.
   3. (subscriber only) The participant authorization string. This is checked 
   against the authorization that the publisher is expecting. The default is 
   "password".
   4. (subscriber only) The reader authorization string. If and only if the 
   reader's participant didn't get authorized, then this value is checked 
   against the authorization that the publisher is expecting. The default is 
   "Reader_Auth".

While generating the output below, we used values that would capture the most
interesting behavior.

Publisher Output
============
Writing msg, count 0
Built-in Reader: found participant 
	key->'[168453134, 2044, 1, 449]'
	user_data->'password'
instance_handle: InstanceHandle_t[10,10,100,14,0,0,7,-4,0,0,0,1,0,0,1,-63]
Built-in Reader: found subscriber 
	participant_key->'[168453134, 2044, 1, 449]'
	key->'[168453134, 2044, 1, -2147483644]'
	user_data->'foo'
instance_handle: InstanceHandle_t[10,10,100,14,0,0,7,-4,0,0,0,1,-128,0,0,4]
publication_matched, current count = 1
Writing msg, count 1
Writing msg, count 2
publication_matched, current count = 0

Subscriber Output
=============
Using participant id 0
Received:
    x: 1

Received:
    x: 2
