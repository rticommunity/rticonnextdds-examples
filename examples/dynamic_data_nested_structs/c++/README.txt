===============================================================
Dynamic Data API, access to complex member example
===============================================================


Building the C++ example
===============================================

The example is contained in the NestedStructExample.cxx file. Before compiling
or running the example, make sure the environment variable NDDSHOME is
set to the directory where you installed RTI Connext DDS.

The accompanying makefiles makefile_Foo_x64Linux3.xgcc4.6.3 and
makefile_Foo_i86Linux3.xgcc4.6.3 can be used to compile the application
for makefile_Foo_x64Linux3.xgcc4.6.3 and makefile_Foo_i86Linux3.xgcc4.6.3, 
respectively.

To generate a makefile for any other architecture, you can search and 
replace the architecture on the makefile, or use rtiddsgen to generate
a makefile specific for it. To use rtiddsgen to generate a makefile,
create a sample idl file called Foo.idl, with the following contents:

struct foo {
    long a;
};

and run 

rtiddsgen -example <platform_name> -language C++ Foo.idl

Once you have run the application, modify the generated makefile and 
set the COMMONSOURCES and EXEC variables to:

COMMONSOURCES =
EXEC          = NestedStructExample

# Remove Foo.h from objs/<arch_name>/%.o, for instance:
objs/i86Linux3.xgcc4.6.3/%.o : %.c

Finally, remove all generated files you will not need. 

rm Foo* USER_QOS_PROFILES.xml

If you are running Windows, follow the same process to generate a Visual
Studio Project 

Now that you have a makefile compatible with your platform 
(e.g., i86Linux3.xgcc4.6.3) , run make to build your example.

make -f makefile_Foo_i86Linux3.xgcc4.6.3

For Windows systems, you will have a new Visual Studio project where you can
build this solution.

Running the example
===================
Run the following command from the example directory to execute
the application.

On UNIX systems:

./objs/<arch_name>/NestedStructExample

On Windows Systems:

objs\<arch_name>\NestedStructExample

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

