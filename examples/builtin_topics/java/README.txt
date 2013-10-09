============================================================================
 Example Code -- Builtin Topics
============================================================================


Building Java Builtin Topic Example
===================================
Make sure you are using one of the relevant RTI Data Distribution Service versions, as specified at the top of the Solution.

Before compiling or running the example, make sure the environment variable NDDSHOME is set to the directory where your version of RTI Data Distribution Service is installed.

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32jdk). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\Builtin_Topics\java> rtiddsgen -language Java -example i86Win32jdk msg.idl

...you will see messages that look like this:

File C:\local\Builtin_Topics\java\msgSubscriber.java already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File C:\local\Builtin_Topics\java\msgPublisher.java already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has not been replaced, which is fine since all the source files for the example are already provided.

Before compiling in Java, make sure that the desired version of the javac compiler is in your PATH environment variable.

Running
=======
Before running, make sure that the desired version of the java compiler is in your PATH environment variable.

In two separate command prompt windows for the publisher and subscriber, run these commands:

    * java -cp .\;%nddshome%\class\nddsjava.jar msgPublisher <domain#> 3
    * java -cp .\;%nddshome%\class\nddsjava.jar msgSubscriber <domain#> 5 password foo

The applications accept up to four arguments:

   1. The <domain #>. Both applications must use the same domain # in order to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher and sleep periods for the subscriber. A value of '0' instructs the application to run forever; this is the default.
   3. (subscriber only) The participant authorization string. This is checked against the authorization that the publisher is expecting. The default is "password".
   4. (subscriber only) The reader authorization string. If and only if the reader's participant didn't get authorized, then this value is checked against the authorization that the publisher is expecting. The default is "Reader_Auth".

While generating the output below, we used values that would capture the most interesting behavior.

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
