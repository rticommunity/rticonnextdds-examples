# Example Code: Time Based Filters

:warning: **Note**: This example uses the **Legacy .NET API**. Until it is
updated, you can learn more about the **new Connext DDS C# API** in the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building C# Example

Before compiling or running the example, make sure the environment variable
`NDDSHOME` is set to the directory where your version of *RTI Connext* is
installed.

Run *rtiddsgen* with the `-example` option and the target architecture of your
choice (e.g., *x64Win64VS2013*). The *RTI Connext Core Libraries and Utilities
Getting Started Guide* describes this process in detail. Follow the same
procedure to generate the code and build the examples. **Do not use the
`-replace` option.** Assuming you want to generate an example for
*x64Win64VS2013* run:

```sh
rtiddsgen -language c# -example x64Win64VS2013 -ppDisable tbf.idl
```

**Note**: If you are using *Visual Studio Express* add the `-express` option to
the command, i.e.:

```sh
rtiddsgen -language c# -example x64Win64VS2013 -express -ppDisable tbf.idc
```

You will see messages that look like:

```plaintext
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/tbf_subscriber.cs
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/tbf_publisher.cs
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

*rtiddsgen* generates one solution with three *Visual Studio* projects. The
project *tbf_type-dotnet4.0.vcxproj* contains the types of the example and it
should be compiled with *Visual Studio C++*. The other two projects contain the
C# publisher and subscriber example and should be compiled with *Visual Studio
C#*.

## Running C# Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
bin\<build_type>\tbf_publisher.exe  <domain_id> <samples_to_send>
bin\<build_type>\tbf_subscriber.exe <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples for the publisher and
    sleep periods for the subscriber. A value of '0' instructs the application
    to run forever; this is the default.
