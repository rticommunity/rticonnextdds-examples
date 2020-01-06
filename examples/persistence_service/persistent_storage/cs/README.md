# Example Code: Persistent Storage

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
rtiddsgen -language c# -example x64Win64VS2013 -ppDisable hello_world.idl
```

**Note**: If you are using *Visual Studio Express* add the `-express` option to
the command, i.e.:

```sh
rtiddsgen -language c# -example x64Win64VS2013 -express -ppDisable hello_world.idc
```

You will see messages that look like:

```plaintext
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/hello_world_subscriber.cs
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/hello_world_publisher.cs
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

*rtiddsgen* generates one solution with three *Visual Studio* projects. The
project *hello_world_type-dotnet4.0.vcxproj* contains the types of the example
and it should be compiled with *Visual Studio C++*. The other two projects
contain the C# publisher and subscriber example and should be compiled with
*Visual Studio C#*.

## Running C# Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

**Note: If you are using *TimesTen*, you may need run the command prompt as
administrator. Otherwise, none of the scenarios will not run correctly.**

**For TimesTen users**: In addition, you need to uncomment some lines in the
*USER_QOS_PROFILES.xml* and *persistence_service_configuration.xml* to load the
correct driver (we assuming you have installed *TimesTen 11.2.1*). Refer to
those documents for further details.

-   Durable Writer History scenario

    ```sh
    bin\<build_type>-VS2010\hello_world_subscriber.exe
    bin\<build_type>-VS2010\hello_world_publisher.exe -sample_count 5 -initial_value 0 -dwh 1
    bin\<build_type>-VS2010\hello_world_subscriber.exe
    bin\<build_type>-VS2010\hello_world_publisher.exe -sample_count 5 -initial_value 5 -dwh 1
    ```

-   Durable Reader State Scenario

    ```sh
    bin\<build_type>-VS2010\hello_world_subscriber.exe -drs 1
    bin\<build_type>-VS2010\hello_world_publisher.exe -sample_count 5 -initial_value 0 -sleep 60
    stop hello_world_subscriber
    bin\<build_type>-VS2010\hello_world_subscriber.exe
    bin\<build_type>-VS2010\hello_world_subscriber.exe -drs 1
    ```

-   Persistence Service Scenario

    ```sh
    # Run persistence service (in the same folder that there are
    # persistence_service_configuration.xml):
    %NDDSHOME%\bin\rtipersistenceservice.bat -cfgFile persistence_service_configuration.xml -cfgName <persistence_service_database|persistence_service_filesystem>

    bin\<build_type>-VS2010\hello_world_subscriber.exe
    bin\<build_type>-VS2010\hello_world_publisher.exe -sample_count 5 -initial_value 0
    bin\<build_type>-VS2010\hello_world_subscriber.exe
    ```

The applications accepts different arguments:

```plaintext
hello_world_subscriber:
    -domainId <domain ID> (default: 0)
    -sample_count <sample_count> (default: infinite => 0)
    -drs <1|0> Enable/Disable durable reader state (default: 0)

hello_world_publisher:
    -domainId <domain ID> (default: 0)
    -sample_count <number of published samples> (default: infinite)
    -initial_value <first sample value> (default: 0)
    -sleep <sleep time in seconds before finishing> (default: 0)
    -dwh <1|0> Enable/Disable durable writer history (default: 0)
```
