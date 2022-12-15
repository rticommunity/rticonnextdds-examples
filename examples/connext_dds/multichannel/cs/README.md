# Example Code: MultiChannel

If you haven't used the RTI Connext C# API before, first check the
[Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html).

## Building the Example :wrench:

First use **rtiddsgen** to generate the C# type and project files from
`MarketData.idl`:

```sh
<install dir>/bin/rtiddsgen -language c# -example net5 MarketData.idl
```

Where `<install dir>` refers to your RTI Connext installation.

You will see messages that look like:

```plaintext
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/MarketDataSubscriber.cs
WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not be
overwritten : /some/path/MarketDataPublisher.cs
```

This is normal and is only informing you that the subscriber/publisher code has
not been replaced, which is fine since all the source files for the example are
already provided.

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
changed filter to Symbol MATCH 'A,D'

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
