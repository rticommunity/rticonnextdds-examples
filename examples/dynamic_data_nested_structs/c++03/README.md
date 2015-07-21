# Example Code: Nested Struct in Dynamic Data

## Building the C++03 example
The example is contained in the *dynamic_data_nested_struct_example.cxx* file.
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
rtiddsgen -platform <platform_name> -language C++03 -create makefiles Foo.idl
```

Once you have run the application, modify the generated makefile and
set the `COMMONSOURCES` and `EXEC` variables to:
```
COMMONSOURCES =
EXEC          = dynamic_data_nested_struct_example
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

## Running the example
Run the following command from the example directory to execute the application.

On *UNIX* systems:
```
./objs/<arch_name>/dynamic_data_nested_struct_example
```

On *Windows* Systems:
```
objs\<arch_name>\dynamic_data_nested_struct_example
```

## Output
```
Connext Dynamic Data Nested Struct Example
--------------------------------------------
Data Type
-----------
struct InnerStruct {
  double x;
  //@ID 0
  double y;
  //@ID 1
}; //@Extensibility EXTENSIBLE_EXTENSIBILITY
struct OuterStruct {
  InnerStruct inner;
  //@ID 0
}; //@Extensibility EXTENSIBLE_EXTENSIBILITY

Setting the initial values of struct
--------------------------------------
        inner:
           x: 3.141590
           y: 2.718280

+ copied struct from outer_data
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


loan/unloan API
-----------------
+ loan member called
        x: 3.141590
        y: 2.718280

+ setting new values to loaned_data
+ current outer_data value
        inner:
           x: 1.000000
           y: 0.000010
```
