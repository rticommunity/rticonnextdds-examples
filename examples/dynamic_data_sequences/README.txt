========================================================
Example Code - Dynamic Data: Accessing Sequence Members
========================================================

Concept
=======
Dynamic Data provides an easy way to access the members of a data sample. Such 
members can be primitive data types or complex data types such as arrays or 
sequences.

While the access to certain members is done using a name, the access in multiple
cardinality data types (such as arrays or sequences) are done using a index 
based access.

Primitive type sequences such as DDS_Long and DDS_Double sequences can be 
usually accessed using dedicated methods such as get_<type>_array() and 
get_<type>_seq(). However, such approach is not possible for non-primitive data
(such as user defined types).

Dynamic data allows accessing to the elements of arrays and sequences using 
a 1-based index access. For this purpose the use of methods such 
get_complex_member() or bind_complex_member() are required. 

Example description
===================

This example shows how to access to elements in array/sequence based dynamic 
data types.

For this example we define the following types:


struct SimpleStruct {
    long a_member;
};

struct TypeWithSequence {
    sequence<MAX_LEN,SimpleStruct> sequence_member;
};

In this example first we fill the 'sequence_member' sequence with data 
(write_data function). After that, we access to each of the elements in the 
sequence (read_data function).

The same concepts shown in this example works for arrays and primitive 
sequences.
