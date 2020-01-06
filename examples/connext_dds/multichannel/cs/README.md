# Example Code: MultiChannel

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
rtiddsgen -language c# -example x64Win64VS2013 -ppDisable market_data.idl
```

**Note**: If you are using *Visual Studio Express* add the `-express` option to
the command, i.e.:

```sh
rtiddsgen -language c# -example x64Win64VS2013 -express -ppDisable market_data.idc
```

You will see messages that look like:

```plaintext
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/market_data_subscriber.cs
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/market_data_publisher.cs
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

*rtiddsgen* generates one solution with three *Visual Studio* projects. The
project *market_data_type-dotnet4.0.vcxproj* contains the types of the example
and it should be compiled with *Visual Studio C++*. The other two projects
contain the C# publisher and subscriber example and should be compiled with
*Visual Studio C#*.

## Running C# Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

```sh
bin\<build_type>\market_data_publisher.exe  <domain_id> <samples_to_send>
bin\<build_type>\market_data_subscriber.exe <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples for the publisher and
    sleep periods for the subscriber. A value of '0' instructs the application
    to run forever; this is the default.

## Wireshark Output

-   **multichannel.pcap**: Observe that data samples are being sent on the wire
    in only the channels that are necessary for delivering samples that pass the
    reader's filter.

-   **singlechannel.pcap**: Shows the wire traffic when multichannel is not used
    and when the reader specifies a `multicast.receive_address` of
    `239.255.0.1`. Observe that all data samples are being sent on the wire,
    i.e. reader-side filtering, since there is only one channel.

## Publisher Output

Nothing

## Subscriber Output

```plaintext
filter is Symbol MATCH 'A'

   Symbol: "A"
   Price: 26.000000

   Symbol: "A"
   Price: 52.000000

   Symbol: "A"
   Price: 78.000000

   Symbol: "A"
   Price: 104.000000
changed filter to Symbol MATCH 'AD'

   Symbol: "A"
   Price: 130.000000

   Symbol: "D"
   Price: 133.000000

   Symbol: "A"
   Price: 156.000000

   Symbol: "D"
   Price: 159.000000

   Symbol: "A"
   Price: 182.000000

   Symbol: "D"
   Price: 185.000000

   Symbol: "A"
   Price: 208.000000

   Symbol: "D"
   Price: 211.000000

   Symbol: "A"
   Price: 234.000000

   Symbol: "D"
   Price: 237.000000
changed filter to Symbol MATCH 'D'

   Symbol: "D"
   Price: 263.000000

   Symbol: "D"
   Price: 289.000000

   Symbol: "D"
   Price: 315.000000

   Symbol: "D"
   Price: 341.000000

   Symbol: "D"
   Price: 367.000000

   Symbol: "D"
   Price: 393.000000
```
