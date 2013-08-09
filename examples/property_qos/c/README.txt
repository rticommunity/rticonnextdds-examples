Relevant RTI DDS versions: 4.2e, 4.3

Purpose
=======
The Property QosPolicy is used to specify name/value pairs of data and attach them to an entity, such as a DomainParticipant.

With the Property QosPolicy, you can avoid the process of creating an entity disabled, changing its QoS settings, and then enabling it.

A common use case is to configure transport properties. The Property QosPolicy may also be used to configure Durable Writer History and Durable Reader State.


The attached example code shows how to use the Property QosPolicy to modify the send and receive socket buffer transport properties. It shows this three ways:

1. Using the XML QoS file 

2. Setting the Property QosPolicy in code 
3. Changing these values in code without the Property QosPolicy (the code refers to this as the "classic" method), so you can see the difference.

Building
=======
Make sure you are using one of the relevant RTI Data Distribution Service versions, as specified at the top of the Solution.

Before compiling or running the example, make sure the environment variable NDDSHOME is set to the directory where your version of RTI Data Distribution Service is installed.

Run rtiddsgen with the -example option and the target architecture of your choice (for example, i86Win32VS2005). The RTI Data Distribution Service Getting Started Guide describes this process in detail. Follow the same procedure to generate the code and build the examples. Do not use the -replace option.

After running rtiddsgen like this...

C:\local\property_qos\c> rtiddsgen -language C -example i86Win32VS2005 numbers.idl

...you will see messages that look like this:

File C:\local\property_qos\c\numbers_subscriber.c already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.
File C:\local\property_qos\c\numbers_publisher.c already exists and will not be replaced with updated content. If you would like to get a new file with the new content, either remove this file or supply -replace option.

This is normal and is only informing you that the subscriber/publisher code has not been replaced, which is fine since all the source files for the example are already provided.

Running
=======
In two separate command prompt windows for the publisher and subscriber, navigate to the objs/<arch> directory and run these commands:

Windows systems:

    * numbers_publisher.exe <domain#> 4
    * numbers_subscriber.exe <domain#> 4

UNIX systems:

    * ./numbers_publisher <domain#> 4
    * ./numbers_subscriber <domain#> 4

The applications accept two arguments:

   1. The <domain #>. Both applications must use the same domain # in order to communicate. The default is 0.
   2. How long the examples should run, measured in samples for the publisher and sleep periods for the subscriber. A value of '0' instructs the application to run forever; this is the default.

While generating the output below, we used values that would capture the most interesting behavior.

Publisher Output
=============
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65507
New UDPv4 receive socket buffer size is: 65507
Writing numbers, count 0
Writing numbers, count 1
Writing numbers, count 2
Writing numbers, count 3

Subscriber Output
==============
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