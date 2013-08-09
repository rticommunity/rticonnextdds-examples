=====================================Example Code - Using the Property QoS
=====================================

Concept
-------
The Property QosPolicy is used to specify name/value pairs of data and attach them to an entity, such as a DomainParticipant. 

Some values can only be set through properties, such as the properties that enable and configure the Monitor library.  Other values can be set in multiple ways, such as built-in transport properties, which can be enabled with properties, or by creating TransportProperty structures. 

For the second case, setting the properties using the Property QoS is much easier.  An example of the steps required to change the transport properties without the Property QoS:
1. Change the factory entity's QoS to create its children disabled
2. Create the entity 
2. Get its transport properties
3. Change its transport properties
4. Enable the entity

The attached example code shows how to use the Property QosPolicy to modify the send and receive socket buffer transport properties. It shows this three ways:
1. Using the XML QoS file to set the Property QoS policy
2. Setting the Property QoS policy in code 
3. Changing these values in code without the Property QoS Policy (the code refers to this as the "classic" method), so you can see the difference.

Publisher Output
----------------
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65535 
New UDPv4 receive socket buffer size is: 65535 
Writing numbers, count 0
Writing numbers, count 1
Writing numbers, count 2
Writing numbers, count 3

Subscriber Output
-----------------
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65535 
New UDPv4 receive socket buffer size is: 65535 
numbers subscriber sleeping for 4 sec...
 number: 500
 halfNumber: 250.000000
numbers subscriber sleeping for 4 sec...
 number: 250
 halfNumber: 125.000000
numbers subscriber sleeping for 4 sec...
 number: 125
 halfNumber: 62.500000
numbers subscriber sleeping for 4 sec..