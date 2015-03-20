# Example Code: Hello Simple

## Building C++03 Example
The example is contained in the *Hello_simple_publisher.cxx* and
*Hello_simple_subscirber.cxx* files. Before compiling or running it, make
sure the environment variable `NDDSHOME` is set to the directory where you
installed *RTI Connext DDS*.

### Unix
The accompanying makefile *makefile_Foo_x64Linux2.6gcc4.1.1* can be used to
compile the application for the architecture *x64Linux2.6gcc4.1.1*.

To generate a makefile for any other architecture, you can search and
replace the architecture on the makefile, or use *rtiddsgen*. To use *rtiddsgen*
to generate a makefile, create a sample idl file called *Foo.idl*, with the
following contents:

``` idl
struct foo {
    long a;
};
```

and run

 ``` shell
rtiddsgen -example <platform_name> -language C++03 Foo.idl
```

Once you have run the application, modify the generated makefile to fix the
`COMMONSOURCES` and `EXEC` variables and remove `Foo.hpp` from
`objs/<arch_name>/%.o`.

``` makefile
COMMONSOURCES =
EXEC          = HelloWorld_subscriber HelloWorld_publisher

objs/$(TARGET_ARCH)/%.o : %.cxx
    $(COMPILER) $(COMPILER_FLAGS)  -o $@ $(DEFINES) $(INCLUDES) -c $<
```

Finally, remove all generated files you will not need and run *make* command.
``` shell
rm Foo* USER_QOS_PROFILES.xml
make -f makefile_Foo_i86Linux2.6gcc4.1.1
```

### Windows
Follow the same process as *Unix* using *rtiddsgen*. In the case of
*Windows* platforms like *i86Win32VS2010*, it will generate
*Visual Studio Project*.


## Running C++03 Example
To run this example on *Windows*, type the following in two different command
shells, either on the same machine or on different machines:

``` batch
objs\<architecture>\HelloSubscriber.exe
objs\<architecture>\HelloPublisher.exe
```

To run this example on a *UNIX-based* system, type the following in two
different command shells, either on the same machine or on different machines:

``` batch
objs/<architecture>/HelloSubscriber
objs/<architecture>/HelloPublisher
```
