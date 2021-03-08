# Example Code: Instance Statistics

If you haven't used the RTI Connext C# API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/getting_started_guide/csharp/before_csharp.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the C# type and project files from
`InstanceStatisticsExample.idl`:

```sh
<install dir>/bin/rtiddsgen -language c# -platform net5 -create typefiles -create makefiles InstanceStatisticsExample.idl
```

Where `<install dir>` refers to your RTI Connext installation.

Then build it using the dotnet CLI:

```sh
dotnet build
```

## Running the Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
dotnet run -- --pub -domain-id <domain_id> --sample-count <samples_to_send>
dotnet run -- --sub -domain-id <domain_id> --sample-count <samples_to_receive>
```

For the full list of arguments:

```sh
dotnet run -- -h
```
