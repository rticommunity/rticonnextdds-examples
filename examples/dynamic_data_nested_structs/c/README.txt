===============================================================
 Example Code -- Nested Struct in Dynamic Data
===============================================================

Building the C example
======================

The example is contained in the dynamic_data_nested_struct_example.c file. 
Before compiling or running the example, make sure the environment variable 
NDDSHOME is set to the directory where you installed RTI Connext DDS.

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

rtiddsgen -example <platform_name> -language C Foo.idl

Once you have run the application, modify the generated makefile and 
set the COMMONSOURCES and EXEC variables to:

COMMONSOURCES =
EXEC          = dynamic_data_nested_struct_example

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

Running the C example
=====================
This example consist in only one executable that is stored in the 
"objs/<arch_name>/" directory.For instance, if your architecture is 
i86Linux3.xgcc4.6.3, you have to run the following command:

$ objs/i86Linux3.xgcc4.6.3/dynamic_data_nested_struct_example

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
----------------------------
Setting the initial values of outer_data with set_complex_member()

   inner:
      x: 3.141590
      y: 2.718280

 + get_complex_member() called

 + inner_data value

   x: 3.141590
   y: 2.718280

 + setting new values to inner_data

   x: 1.000000
   y: 0.000010

 + current outer_data value

   inner:
      x: 3.141590
      y: 2.718280


 bind/unbind API
------------------
Creating a new dynamic data called bounded_data

 + binding bounded_data to outer_data's inner_struct

   x: 3.141590
   y: 2.718280

 + setting new values to bounded_data

   x: 1.000000
   y: 0.000010

 + current outer_data value

   inner:
      x: 1.000000
      y: 0.000010

================= END OUTPUT ==========================

