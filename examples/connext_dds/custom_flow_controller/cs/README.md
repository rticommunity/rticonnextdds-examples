# Example Code: Custom Flowcontroller

If you haven't used the RTI Connext C# API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/6.1.2/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the C# type and project files from
`FlowControllerExample.idl`:

```sh
<install dir>/bin/rtiddsgen -language c# -platform net5 -create typefiles -create makefiles FlowControllerExample.idl
```

Where `<install dir>` refers to your RTI Connext installation.

You will see messages that look like:

```plaintext
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/FlowControllerExample.csproj
```

This is normal and is only informing you that some of the files that **rtiddsgen**
can generate were already available in the repository.

Then build it using the dotnet CLI:

```sh
dotnet build
```

## Running C# Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
dotnet run -- --pub
dotnet run -- --sub
```

You can update the flow controller token-bucket period (in milliseconds) used
by the publisher application in the command line:

```sh
dotnet run -- --pub --period 50
```

For the full list of arguments:

```sh
dotnet run -- -h
```

## Publisher Output

```plaintext
Writing x=0
Writing x=1
Writing x=2
Writing x=3
Writing x=4
Writing x=5
Writing x=6
Writing x=7
Writing x=8
Writing x=9
Writing x=10
Writing x=11
Writing x=12
Writing x=13
Writing x=14
Writing x=15
Writing x=16
Writing x=17
Writing x=18
Writing x=19
Writing x=20
Writing x=21
Writing x=22
Writing x=23
Writing x=24
Writing x=25
Writing x=26
Writing x=27
Writing x=28
Writing x=29
Writing x=30
Writing x=31
Writing x=32
Writing x=33
Writing x=34
Writing x=35
Writing x=36
Writing x=37
Writing x=38
Writing x=39
```

## Subscriber Output

```plaintext
At t=2.685928s, got x=0
At t=2.781702s, got x=1
At t=2.897418s, got x=2
At t=3.013025s, got x=3
At t=3.126543s, got x=4
At t=3.23995s, got x=5
At t=3.356158s, got x=6
At t=3.473951s, got x=7
At t=3.590605s, got x=8
At t=3.707113s, got x=9
At t=3.824015s, got x=10
At t=3.940767s, got x=11
At t=4.057632s, got x=12
At t=4.174062s, got x=13
At t=4.29126s, got x=14
At t=4.392365s, got x=15
At t=4.507466s, got x=16
At t=4.624523s, got x=17
At t=4.7415s, got x=18
At t=4.858407s, got x=19
At t=4.974957s, got x=20
At t=5.091741s, got x=21
At t=5.199797s, got x=22
At t=5.312822s, got x=23
At t=5.426081s, got x=24
At t=5.542166s, got x=25
At t=5.657693s, got x=26
At t=5.773644s, got x=27
At t=5.886495s, got x=28
At t=5.999547s, got x=29
At t=6.111968s, got x=30
At t=6.215294s, got x=31
At t=6.32946s, got x=32
At t=6.445314s, got x=33
At t=6.562069s, got x=34
```
