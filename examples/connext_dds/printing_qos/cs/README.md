# Example Code: Qos Printing Example

If you haven't used the RTI Connext C# API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Concept

Using the Qos printing APIs, the Qos being used by a DDS entity can be printed.
This functionality is useful in many scenarios, one of these being when it is not
clear which Qos is being used by an entity.

In this example we define the DataWriterQos in the USER_QOS_PROFILES.xml file to
have a durability kind of Volatile. The code obtains this Qos from the XML file,
but modifies the durability kind to TransientLocal before creating the DataWriter.

To somebody unfamiliar with the order of Qos precedence, it may not be clear what
durability kind the created DataWriter will be using. We print the Qos in use by
the DataWriter, which shows that it is using TransientLocal durability (as set
in the code).

## Building the Example :wrench:

First use **rtiddsgen** to generate the C# type and project files from
`QosPrintingExample.idl`:

```sh
<install dir>/bin/rtiddsgen -language c# -platform net5 -create typefiles -create makefiles QosPrintingExample.idl
```

Where `<install dir>` refers to your RTI Connext installation.

Then build it using the dotnet CLI:

```sh
dotnet build
```

## Running the Example
Run the following command from the example directory (this is necessary to ensure
the application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
dotnet run
```
