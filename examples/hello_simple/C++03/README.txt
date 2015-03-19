============================================================
 Example Code -- Hello Simple
============================================================

Building C++03 Example
======================
The example is contained in the Hello_simple_publisher.cxx and
Hello_simple_subscirber.cxx files. Before compiling or running the example, make
sure the environment variable NDDSHOME is set to the directory where you
installed RTI Connext DDS.

The accompanying makefile makefile_Foo_x64Linux2.6gcc4.1.1 can be used to
compile the application for the architecture x64Linux2.6gcc4.1.1.

To generate a makefile for any other architecture, you can search and
replace the architecture on the makefile, or use rtiddsgen to generate
a makefile specific for it. To use rtiddsgen to generate a makefile,
create a sample idl file called Foo.idl, with the following contents:

struct foo {
    long a;
};

and run

rtiddsgen -example <platform_name> -language C++03 Foo.idl

Once you have run the application, modify the generated makefile. Remove the
CDRSOURCE variable and set the COMMONSOURCES and EXEC variables to:

COMMONSOURCES =
EXEC          = HelloWorld_subscriber HelloWorld_publisher

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

Running C++03 Example
=====================

To run this example on Windows, type the following in two different command
shells, either on the same machine or on different machines:

  > objs\<architecture>\HelloSubscriber.exe
  > objs\<architecture>\HelloPublisher.exe

To run this example on a UNIX-based system, type the following in two
different command shells, either on the same machine or on different machines:

  > objs/<architecture>/HelloSubscriber
  > objs/<architecture>/HelloPublisher

For more information, please consult your "Getting Started Guide."
