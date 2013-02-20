===========================================
 Example Code -- Using Typecodes
===========================================

Concept
-------
Type schemas -- the names and definitions of a type and its fields -- are 
represented by TypeCode objects. A type code value consists of a type code kind 
and a list of members. Type codes unambiguously match type representations and 
provide a more reliable test than comparing the string type names.

The TypeCode class, modeled after the corresponding CORBA API, provides access 
to typecode information. For details on the available operations for the 
TypeCode class, see the API Reference HTML documentation, which is available for 
all supported programming languages (select Modules, DDS API Reference, Topic 
Module, Type Support, Type Code Support in the corresponding reference manual). 

In addition to being used locally, serialized type codes are typically published
automatically during discovery as part of the built-in topics for publications 
and subscriptions. This allows applications to publish or subscribe to topics of
arbitrary types. This functionality is useful for generic system monitoring 
tools like the rtiddsspy debug tool.

Note: Type codes are not cached by Connext upon receipt and are therefore not 
available from the built-in data returned by the DataWriter's 
get_matched_subscription_data() operation or the DataReader's 
get_matched_publication_data() operation. 

If your data type has an especially complex type code, you may need to increase 
the value of the type_code_max_serialized_length field in the 
DomainParticipant's DOMAIN_PARTICIPANT_RESOURCE_LIMITS QosPolicy 
(DDS Extension). Or, to prevent the propagation of type codes altogether, you 
can set this value to zero (0). Be aware that some features of monitoring tools,
as well as some features of the middleware itself 
(such as ContentFilteredTopics) will not work correctly if you disable TypeCode 
propagation.

 
Example Description
-------------------
The Topic type in this example is complex to illustrate the power of type codes.
The default value for resource_limits.type_code_max_serialized_length is  
2048 bytes --  an insufficient size to hold the type codes of a complex 
type. We need to increase resource_limits.type_code_max_serialized_length so 
that built-in topics on the receiver side get the correct type code information
of our example topic type, which has a size of 3070 bytes. Note that resource 
limits need to be changed both in the Publisher and Subscriber application's 
DomainParticipant QoS.

The subscriber has no user data readers. Our purpose is to show how to 
manipulate metadata describing the message type. To do this, we listen for 
built-in topic data describing published topics. When we get information about 
new topic types, we print out information about the type using the TypeCode 
APIs.
