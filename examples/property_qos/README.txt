Example Code - Using the Property QoSPolicy

Purpose 
=======
The Property QosPolicy is used to specify name/value pairs of data and attach them to an entity, such as a DomainParticipant. 

With the Property QosPolicy, you can avoid the process of creating an entity disabled, changing its QoS settings, and then enabling it.

A common use case is to configure transport properties. The Property QosPolicy may also be used to configure Durable Writer History and Durable Reader State.

The attached example code shows how to use the Property QosPolicy to modify the send and receive socket buffer transport properties. It shows this three ways:
1. Using the XML QoS file 
2. Setting the Property QosPolicy in code 
3. Changing these values in code without the Property QosPolicy (the code refers to this as the "classic" method), so you can see the difference.


Publisher Output
=============
Ok, send_socket_buffer_size....modified
Ok, recv_socket_buffer_size....modified
New UDPv4 send socket buffer size is: 65535 
New UDPv4 receive socket buffer size is: 65535 
Writing numbers, count 0
Writing numbers, count 1
Writing numbers, count 2
Writing numbers, count 3

Subscriber Output
==============
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