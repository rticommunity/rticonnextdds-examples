===============================================================
Dynamic Data API, access to complex member example
===============================================================


Building the Java example
===============================================

Before building the example make sure that the NDDSHOME environment variable is set and its pointing to a valid RTI Connext location.

This example is only composed by an unique file called "DynamicDataNestedStruct.java". In order to build it on a makefile based system, you can run this command: 

$ make -f makefilei86Linux2.6jdk ARCH=<arch_name>

Running the example
===================
You can run the program using the following command: 

$ make -f makefilei86Linux2.6jdk DynamicDataNestedStruct

Running the program produces the following output:

================= BEGIN OUTPUT ==========================
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

================= END OUTPUT ==========================

