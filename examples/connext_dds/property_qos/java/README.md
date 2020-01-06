# Example code: Using Property QoS

## Building Java Example

Before compiling or running the example, make sure the environment variable
NDDSHOME is set to the directory where your version of RTI Connext is installed.

Run rtiddsgen with the -example option and the target architecture of your
choice (e.g., i86Win32VS2010). The RTI Connext Core Libraries and Utilities
Getting Started Guide describes this process in detail.

Follow the same procedure to generate the code and build the examples. Do not
use the -replace option.

On Windows systems (assuming you want to generate an example for i86Win32VS2010)
run:

```sh
rtiddsgen -language Java -example i86Win32VS2010 numbers.idl
```

On UNIX systems (assuming you want to generate an example for
i86Linux2.6gcc4.4.3) run:

```sh
rtiddsgen -language Java -example i86Linux2.6gcc4.4.3 numbers.idl
```

...you will see messages that look like this:

```plaintext
File C:\local\property_qos\java\numbers_subscriber.java already exists and will
not be replaced with updated content. If you would like to get a new file with
the new content, either remove this file or supply -replace option.
File C:\local\property_qos\java\numbers_publisher.java already exists and will
not be replaced with updated content. If you would like to get a new file with
the new content, either remove this file or supply -replace option.
File C:\local\property_qos\java\USER_QOS_PROFILES.xml already exists and will
not be replaced with updated content. If you would like to get a new file with
the new content, either remove this file or supply -replace option.
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

Before compiling in Java, make sure that the desired version of the javac
compiler is in your PATH environment variable.

On Windows systems run:

```sh
javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar *.java
```

On Unix systems (including Linux and MacOS X):

```sh
javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar *.java
```

## Running Java Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in USER_QOS_PROFILES.xml):

On Windows systems run:

```sh
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar numbersPublisher  <domain_id> <samples_to_send>
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar numbersSubscriber <domain_id> <sleep_periods>
```

On Unix systems (including Linux and MacOS X) run:

```sh
java -cp .:$NDDSHOME/lib/java/nddsjava.jar numbersPublisher  <domain_id> <samples_to_send>
java -cp .:$NDDSHOME/lib/java/nddsjava.jar numbersSubscriber <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1.  The <domain_id>. Both applications must use the same domain id in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples for the publisher and
    sleep periods for the subscriber. A value of '0' instructs the application
    to run forever; this is the default.

## Publisher Output

```plaintext
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65507
New UDPv4 receive socket buffer size is: 65507
Writing numbers, count 0
Writing numbers, count 1
Writing numbers, count 2
Writing numbers, count 3
```

## Subscriber Output

```plaintext
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65507
New UDPv4 receive socket buffer size is: 65507
numbers subscriber sleeping for 4 sec...
   number: 500
   halfNumber: 250.000000
numbers subscriber sleeping for 4 sec...
   number: 250
   halfNumber: 125.000000
numbers subscriber sleeping for 4 sec...
   number: 125
   halfNumber: 62.500000
numbers subscriber sleeping for 4 sec...
```
