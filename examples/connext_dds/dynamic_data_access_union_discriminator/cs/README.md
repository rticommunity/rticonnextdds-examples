# Example Code: Access Union Discriminator In Dynamic Data

:warning: **Note**: This example uses the **Legacy .NET API**. Until it is
updated, you can learn more about the **new Connext DDS C# API** in the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building C# Example

The example is contained in the *dynamic_data_union_example.cs* file. Before
compiling or running the example, make sure the environment variable `NDDSHOME`
is set to the directory where you installed *RTI Connext DDS*.

The accompanying Visual Studio projects called *Hello-i86Win32dotnet4.0.sln* and
*Hello-i86Win32VS2010.sln* can be used to compile the application for *Visual
Studio 2010*. Whether the NDDS version is higher or equal than 5.2.0, you have
to use Hello-i86Win32VS2010.sln, else you have to use
*Hello-i86Win32dotnet4.0.sln*.

If you are using this method, build the solution in *Visual Studio 2010* and
continue reading *Running C# Example* paragraph.

## Creating a new project

If the architecture or the *Visual Studio* version are different, you need to
create a new *Visual Studio* Project. For this task, you can use *rtiddsgen* to
generate it. To use *rtiddsgen* to generate a new project, create a sample idl
file called *Foo.idl*, with the following contents:

```c
struct foo {
    long a;
};
```

and run (assuming you want to generate an example for *i86Win32dotnet4.0*):

```sh
rtiddsgen -language C# -example i86Win32dotnet4.0 -ppDisable Foo.idl
```

Once you have run the application, you will need to modify the generated
solution and add the file *dynamic_data_union_example.cs*. Following the next
steps to get your solution ready to use.

Modify the next file names:

- Foo-csharp.sln -> dynamic_data_union_example.sln
- Foo_publisher-csharp.csproj -> dynamic_data_union_example.csproj

Modify (in text format) dynamic_data_union_example.sln and change

- Foo_publisher-csharp.csproj -> dynamic_data_union_example.csproj

Modify (in text format) dynamic_data_union_example.csproj and change these
lines:

```xml
<ItemGroup>
    <Compile Include="foo_publisher.cs" />
</ItemGroup>
```

for:

```xml
<ItemGroup>
    <Compile Include="dynamic_data_union_example.cs" />
</ItemGroup>
```

Finally, remove all generated files you will not need:

```sh
del Foo* USER_QOS_PROFILES.xml
```

You will have a new *Visual Studio* solution where you can build this project.

Open this solution with Visual Studio and delete manually *Foo_type* and
*Foo_subscriber-csharp* projects.

You can modify the name of the executable, clicking right button over the
project and push in properties. in the new panel, modify `Application/Assembly
name`. This name will be you executable name.

## Running C# Example

Run the following command from the example directory to execute the application.

If you have used the accompanying Visual Studio Project:

```sh
bin\<build_type>-VS2010\dynamic_data_union_example.exe
```

If you have generated a new Visual Studio Project:

```sh
bin\<build_type>-<VSVersion>\Foo_publisher.exe
```
