===========================================
 Example Code -- Using Sequences
===========================================

Concept
-------
A sequence is an ordered collection of elements of the same type. The type of a 
sequence containing elements of type "Foo" (whether "Foo" is one of your types
or a built-in Connext type) is typically called "FooSeq".

In all APIs except Java, FooSeq contains deep copies of Foo elements; in Java, 
which does not provide direct support for deep copy semantics, FooSeq contains 
references to Foo objects. In Java, sequences implement the java.util.List 
interface, and thus support all of the collection APIs and idioms familiar to 
Java programmers.

A sequence is logically composed of three things: an array of elements, a 
maximum number of elements that the array may contain (i.e., its allocated 
size), and a logical length indicating how many of the allocated elements are 
valid. The length may vary dynamically between 0 and the maximum (inclusive); 
it is not permissible to access an element at an index greater than or equal 
to the length.

A sequence may either "own" the memory associated with it, or it may "borrow" 
that memory. 

- If a sequence owns its own memory, then the sequence itself will allocate the 
its memory and is permitted to grow and shrink that memory 
(i.e., change its maximum) dynamically.
- You can also loan a sequence of memory using the sequence-specific operations 
loan_contiguous() or loan_discontiguous(). This is useful if you want Connext 
to copy the received data samples directly into data structures allocated in 
user space.

A sequence may be declared as bounded or unbounded. A sequence's bound is the
maximum number of elements that the sequence can contain at any one time. 
The bound is very important because it allows Connext to preallocate buffers to
hold serialized and deserialized samples of your types; these buffers are used 
when communicating with other nodes in your distributed system. If a sequence 
had no bound, Connext would not know how large to allocate its buffers and 
would therefore have to allocate them on the fly as individual samples were 
read and written -- severely impacting the latency and determinism of your 
application. Therefore, Connext supports only bounded sequences; any unbounded 
sequences found in an IDL file will be given a default bound of 100 elements.

Example description
-------------------

In this example, we define a data type containing a sequence of shorts in an  
IDL. Using this IDL file we generate a publisher and a subscriber application 
that manage two instances of that data type. 

Our goal is to illustrate how sequences can "own" the memory associated with it,
or "borrow" that memory. The sequence of the first instance, owner_instance, 
owns its own memory, whereas in the second instance, borrower_instance, we use 
loan_contiguous() to loan a buffer of shorts previously allocated. We also 
illustrate how to use the sequences created, changing the sequence's length 
every time we send a sample of each instance, assigning a random value to each
sequence's elements.
