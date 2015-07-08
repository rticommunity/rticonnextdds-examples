# Example Code: Nested Struct in Dynamic Data

## Building Java example
The example is contained in the *DynamicDataNestedStruct.java* file. Before
compiling or running the example, make sure the environment variable `NDDSHOME`
is set to the directory where your version of *RTI Connext* is installed.

Before compiling in Java, make sure that the desired version of the *javac*
compiler is in your `PATH` environment variable.

In *Windows* systems run:
```
javac -classpath .;%NDDSHOME%\lib\java\nddsjava.jar DynamicDataNestedStruct.java
```

On *UNIX* systems (including Linux and MacOS X) run:
```
javac -classpath .:$NDDSHOME/lib/java/nddsjava.jar DynamicDataNestedStruct.java
```

## Running Java Example
On *Windows* Systems:
```
java -cp .;%NDDSHOME%\lib\java\nddsjava.jar DynamicDataNestedStruct
```

On *UNIX* systems:
```
java -cp .:$NDDSHOME/lib/java/nddsjava.jar DynamicDataNestedStruct
```

Running the program produces the following output:
```
 Connext Dynamic Data Nested Struct Example
--------------------------------------------
 Data Types
------------------
struct InnerStruct {
   double x;
   double y;
}; //@Extensibility EXTENSIBLE_EXTENSIBILITY
struct OuterStruct {
   InnerStruct inner;
}; //@Extensibility EXTENSIBLE_EXTENSIBILITY


 get/set_complex_member API
------------------
Setting the initial values of struct with set_complex_member()

   inner:
      x: 3.141590
      y: 2.718280

 + get_complex_member() called

 + inner struct value

   x: 3.141590
   y: 2.718280

 + setting new values to inner struct

 + current outter struct value

   inner:
      x: 3.141590
      y: 2.718280


 bind/unbind API
------------------

 + bind complex member called

   x: 3.141590
   y: 2.718280

 + setting new values to inner struct

   x: 1.000000
   y: 0.000010

 + current outter struct value

   inner:
      x: 1.000000
      y: 0.000010
```
