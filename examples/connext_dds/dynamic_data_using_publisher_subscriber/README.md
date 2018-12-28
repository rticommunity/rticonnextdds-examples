# Dynamic Data API: publish/subscribe with Dynamic Data

## Concept
*Dynamic Data* provides an easy way to access the members of a data sample.
Such members can be primitive data types or complex data types such as
arrays or sequences.

While the access to certain members is done by name, the access in multiple
cardinality data types—such as arrays or sequences—is done by index.
Primitive type sequences (e.g., *DDS_Long* and *DDS_Double sequences*) can
be usually accessed using dedicated methods such as `get_<type>_array()`
and `get_<type>_seq()`. However, that approach is not possible for
non-primitive data (e.g., user defined types).

Using *Dynamic Data* you can access elements of arrays and sequences
using a 1-based index access using the `get_complex_member()` and
`bind_complex_member()` methods.

## Example Description
This example shows how to publish and subscribe Shapes using the *Dynamic
Data API*. In particular, application that operate on the "Square" topic
through *Dynamic Data* calls.

This example can be run along with *RTI Shapes Demo* as explained in the
README file of each specific language. The *Shapes.idl* contains the
following data structure:
```
struct ShapeType {
 string<128> color; //@key
 long x;
 long y;
 long shapesize;
};
```
