# Dynamic Data API: access to complex member example

## Concept
*Dynamic Data API* allows to create topic samples in a programmatically manner
without defining an IDL in compile time. Analogously to the IDL based approach,
the *Dynamic Data API* provide methods to get/set complex members in dynamic
data samples.

In this example, we show how to access structs nested in a dynamic data object
to get/set their values.

## Example description
In this example, we use the two *Dynamic Data APIs* provided by *RTI Connext* to
access to sample members that are not one of the primitive types.

The first API the get/set API, copies the value of a *Dynamic Data* complex
member into a new *Dynamic Data* object. As a consequence, any modification on
the newly created object will not affect the original member.

The second API, the bind/unbind API, allows to reference of a complex member
sing a new *Dynamic Data* object. This binding is a reference to the original
nested member, so any modification on this object will affect the original
member.

The data types used for our examples are the following ones:
```
struct InnerStruct {
   double x;
   double y;
};

struct OuterStruct {
   InnerStruct inner;
};
```
