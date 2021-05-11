# Example code: Using Dynamic Data to publish and subscribe

If you haven't used the RTI Connext C# API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the C# type and project files from
`ShapeType.idl`.

```sh
<install dir>/bin/rtiddsgen -language c# -platform net5 -create typefiles -create makefiles ShapeType.idl
```

Where `<install dir>` refers to your RTI Connext installation.

You will see messages that look like:

```plaintext
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/PartitionsExample.csproj
```

This is normal and is only informing you that some of the files that
**rtiddsgen** can generate were already available in the repository.

Generating code is not necessary to use a DynamicType and
DynamicData, but this example demonstrates several ways to obtain a DynamicType,
including from the generated code.

Then build it using the dotnet CLI:

```sh
dotnet build
```

The project file is configured to build a .NET 5 application. To build for
a different target, change `<TargetFramework>net5.0</TargetFramework>` in
the `.csproj` file.

## Running the Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
dotnet run -- --pub
dotnet run -- --sub
```

You can change the way the DynamicType is created with the `--type-source`
argument, for example:

```sh
dotnet run -- --pub --type-source idl
```

The following type sources are available:

-   `build`, to build it in code (default)
-   `extended`, to build a version of the type with extra fields
-   `idl`, to obtain it from the code generated from IDL by rtiddsgen
-   `xml`, to load it dynamically from `Shapes.xml`. To generate this file,
run the following command:

```sh
<install dir>/bin/rtiddsgen -convertToXml Shapes.idl
```

The implementation of each of these options is in `ShapeTypeHelper.cs`.

For the full list of arguments:

```sh
dotnet run -- -h
```
