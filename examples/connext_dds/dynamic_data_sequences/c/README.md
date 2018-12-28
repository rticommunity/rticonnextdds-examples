# Example: Sequences in Dynamic Data

## Building the C example
The example is contained in the *dynamic_data_sequences.c* file.
Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where you installed *RTI Connext DDS*.

The accompanying makefiles *makefile_Foo_i86Linux3gcc4.8.2* and
*makefile_Foo_x64Linux3gcc4.8.2* can be used to compile the application
for *i86Linux3gcc4.8.2* and *x64Linux3gcc4.8.2*, respectively.

To generate a makefile for any other architecture, you can search and
replace the architecture on the makefile, or use *rtiddsgen* to generate
a makefile specific for it. Regarding the latter, create a temporal IDL file
named *Foo.idl* (it can be empty) and run:
```
rtiddsgen -platform <platform_name> -language C -create makefiles Foo.idl
```

Once you have run the application, modify the generated makefile and
set the `COMMONSOURCES` and `EXEC` variables to:
```
COMMONSOURCES =
EXEC          = dynamic_data_sequences
```

Remove `Foo.hpp` from `objs/$(TARGET_ARCH)/%.o`:
```
objs/$(TARGET_ARCH)/%.o : %.cxx
```

If you are running *Windows*, follow the same process to generate a *Visual
Studio Project*.

Now that you have a makefile compatible with your platform
(e.g., *i86Linux3gcc4.8.2*), run `make` to build your example.
```
make -f makefile_Foo_i86Linux3gcc4.8.2
```

For *Windows* systems, you will have a new *Visual Studio* project where you can
build this solution.

To test the example using the bind API instead of using the get API, uncomment
the following line in the source file:
```
#define USE_BIND_API
```

## Running the example
Run the following command from the example directory to execute the application.

On *UNIX* systems:
```
./objs/<arch_name>/dynamic_data_sequences
```

On *Windows* Systems:
```
objs\<arch_name>\dynamic_data_sequences
```
