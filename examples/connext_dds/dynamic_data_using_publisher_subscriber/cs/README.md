# Example code: Using Dynamic Data to publish and subscribe

## Building C# Example
Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where your version of *RTI Connext* is
installed.

Run *rtiddsgen* with the `-example` option and the target architecture of your
choice (e.g., *i86Win32VS2010*). The *RTI Connext Core Libraries and Utilities
Getting Started Guide* describes this process in detail. Follow the same
procedure to generate the code and build the examples. **Do not use the
`-replace` option.** Assuming you want to generate an example for
*i86Win32VS2010* run:
```
rtiddsgen -language C# -example i86Win32VS2010 -ppDisable Shapes.idl
```

**Note**: If you are using *Visual Studio Express* add the `-express` option to
the command, i.e.,
```
rtiddsgen -language C# -example i86Win32VS2010 -express -ppDisable Shapes.idl
```

You will see messages that look like this:
```
File C:\...\cs\Shapes_subscriber.cs already exists and will not
be replaced with updated content. If you would like to get a new file with the
new content, either remove this file or supply -replace option.
File C:\...\cs\Shapes_publisher.cs already exists and will not
be replaced with updated content. If you would like to get a new file with the
new content, either remove this file or supply -replace option.
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

*rtiddsgen* generates two solutions for *Visual Studio C++* and *C#*, that you
will use to build the types and the C# example, respectively. First open
*Shapes_type-dotnet4.0.sln* and build the solution. Once you've done that, open
*Shapes_example-csharp.sln* and build the C# example.

## Running C# Example
### Publisher/Subscriber in command prompt
In two separate command prompt windows for the publisher and subscriber.

On *Windows* systems run:
```
bin\<build_type>-VS2010\Shapes_publisher.exe <domain_id> <sample #>
bin\<build_type>-VS2010\Shapes_subscriber.exe <domain_id> <sample #>
```

The applications accept up to two arguments:

1. The `<domain_id>`. Both applications must use the same domain ID in order
to communicate. The default is 0.
2. How long the examples should run, measured in samples. A value of '0'
instructs the application to run forever; this is the default.

### Interconnection between this example and *Shapes Demo* tool
First of all, remember that you have to work in the same domain both in
*Shape Demo* and your command prompt.

1. Publish using this example and subscribe using *Shapes Demo*:
    1. Open a new *Shapes Demo* window.
    2. Create a new Square subscriber with default options.
    3. Run the publisher in a new command prompt like explained before (use the
    `<sample #>` = 0 option to run an indefinitely).

    You will see a Square moving from left to right. Also, its size will
    increase until it reaches a maximum size. When the max size or the end of
    the canvas are reached, it restarts.

2. Subscribe using this example and publish using *Shapes Demo*:
    1. Open a new command prompt and create a new subscriber like explained
    before.
    2. Create a publisher (Square) in *Shapes Demo*.

    You will see the full description of the data published by *Shapes Demo* in
    your command prompt.
