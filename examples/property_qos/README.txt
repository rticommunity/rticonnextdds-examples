=====================================
Example Code - Using the Property QoS
=====================================

Concept
-------
The Property QosPolicy is used to specify name/value pairs of data and attach
them to an entity, such as a DomainParticipant. 

Some values can only be set through properties, such as the properties that
enable and configure the Monitor library. Other values can be set in multiple
ways, such as built-in transport properties, which can be enabled with 
properties, or by creating TransportProperty structures. 

For the second case, setting the properties using the Property QoS is much 
easier.  An example of the steps required to change the transport properties 
without the Property QoS:

  1. Change the factory entity's QoS to create its children disabled
  2. Create the entity 
  3. Get its transport properties
  4. Change its transport properties
  5. Enable the entity

Example Description
-------------------
The attached example code shows how to use the Property QosPolicy to modify the
send and receive socket buffer transport properties. It shows this three ways:
  1. Using the XML QoS file to set the Property QoS policy
  2. Setting the Property QoS policy in code.
  3. Changing these values in code without the Property QoS Policy (the code 
     refers to this as the "classic" method), so you can see the difference.


Note that the C# example is slightly different, because it does not set the 
transport properties in code.  This cannot be done in C# using the transport 
APIs, and must be done through the Property QoS Policy in code or XML.
