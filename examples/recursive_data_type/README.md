# Example Code: Recursive Data Type

## Concept
Some data-types are recursive in the sense that they contain members elements
of the same data-type as the containing structure. A classic example is a Tree.

A Tree is traditionally defined as a type that contains a Node (data) and a
list of children who themselves can be Tree structures. A simple example
in C would be:

```
/* C definition of a Tree structure */
struct Node {
    long data;
};

struct Tree {
    Node node;
    struct Tree *children; // NULL terminated list of children
};
```

Representing these kinds of *recursive data-types* in IDL such that they can
be sent using DDS presents challenges.

While *recursive data-types* are not officially supported by RTI Connext DDS
it is possible to use them in RTI Connext DDS 5.2, as long as you program in
least in C or C++. In future releases the support will be expanded to Java and C#.

Using *recursive data-types* requires taking advantage of some advanced 
features of `rtiddsgen`. This example illustrates the approach.

## Example Description

The simplest mapping of the above Tree to IDL would be to define a Tree as:

```
// IDL definition of a Tree structure. This will not compile...
struct Node {
    long data;
};

struct Tree {
    Node node;
    sequence<Tree> children; 
};
```

If you try this and run `rtiddsgen` you will encounter several problems:
1) `rtiddsgen` may fail as it is unable to resolve the recursive dependency on the type
  getting into an infinite recursion that overflows the stack.
            
2) `rtiddsgen` may fail as it is unable to compute the "TypeCode" for the data-type
        
3) `rtiddsgen` may fail as it is unable to compute the "max serialized size" for the data-type
        
4) The generated header files may give compile-time errors due to them referencing types
   before they are fully defined
   
To overcome these problems use the following techniques:
        
(1) Annotate the recursive type member with the annotation `//@resolve-name false`
    This prevents `rtiddsgen` from trying to resolve the type and getting into the infinite
    recursion
                
(2) Run `rtiddsgen` with the option `-notypecode`  
    This option prevents it use the recursive type inside an unbounded sequence and run
    `rtiddsgen` with the option  trying to compute the typecode.
        
(3) Run `rtiddsgen` with the option `-unboundedSupport` this disables the
    computation of the max serialized size as being unlimited there would be no maximum size.
    Note that this option is currently only available in C and C++.
        
(4) Declare the recursive type with the annotation `//@Optional`
    this causes `rtiddsgen` to generate a reference/pointer avoiding the self-reference compile-time
    issue.
   
Applying these techniques the resulting IDL is:

```
// IDL file: recursiveType.idl
struct Node {
    long data;
};

struct Tree {
    Node node;
    sequence<Tree> children; //@Optional
                             //@resolve-name false
};
```

And the `rtiddsgen` command called to generate the data-types and marshaling code is:
```
rtiddsgen RecursiveType.idl -ppDisable -language C++ -update typefiles -notypecode -unboundedSupport
```
As mentioned in Connext DDS 5.2 this techniques are only usable in C and C++.




      
