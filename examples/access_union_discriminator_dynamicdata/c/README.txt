============================================================
 Example Code -- Access Union Discriminator In Dynamic Data
============================================================

Building C Example
==================
The example is contained in the UnionExample.c file. Before compiling
or running the example, make sure the environment variable NDDSHOME is
set to the directory where you installed RTI Connext DDS.

The accompanying makefiles makefile_Foo_x64Darwin10gcc4.2.1 and
makefile_Foo_i86Linux2.6gcc4.1.1 can be used to compile the application
for x64Darwin10gcc4.2.1 and i86Linux2.6gcc4.1.1, respectively.

To generate a makefile for any other architecture, you can search and 
replce the architecture on the makefile, or use rtiddsgen to generate
a makefile specifily for it. To use rtiddsgen to generate a makefile,
create a sample idl file called Foo.idl, with the following contents:

struct foo {
    long a;
};

and run 

rtiddsgen -example <platform_name> -language C Foo.idl

Once you have run the application, modify the generated makefile and 
set the COMMONSOURCES and EXEC variables to:

COMMONSOURCES =
EXEC          = UnionExample

# Remove Foo.h from objs/<arch_name>/%.o, for instance:
objs/i86Linux2.6gcc4.1.1/%.o : %.c

Finally, remove the the generated files you will not need. 

rm Foo* USER_QOS_PROFILES.xml

If you are running Windows, follow the same process to generate a Visual
Studio Project 

Now that you have a makefile compatible with your platform 
(e.g., i86Linux2.6gcc4.1.1) , run make to build your example.

make -f makefile_Foo_i86Linux2.6gcc4.1.1

Running C Example
=================

Run the following command from the example directory to execute
the application.

On UNIX systems:

./objs/<arch_name>/UnionExample

On Windows Systems:

objs\<arch_name>\UnionExample
