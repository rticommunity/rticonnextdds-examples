===========================================
 Example Code -- Content Filtered Topic
===========================================

Concept
-------
Unions in IDL are mapped to structs in C and C++, so that Connext will not 
have to dynamically allocate memory for unions containing variable-length 
fields such as strings or sequences. To be efficient, the entire struct 
(or class in C++/CLI) is not sent when the union is published. Instead, 
Connext uses the discriminator field of the struct to decide what field 
in the struct is actually sent on the wire.

This example 
  - creates the type code of for a union 
  - creates a DynamicData instance
  - sets one of the field of the union
  - shows how to retrieve the discriminator 
