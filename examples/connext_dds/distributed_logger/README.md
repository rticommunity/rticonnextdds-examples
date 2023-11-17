# Example Code: Distributed Logger

## Concept

RTI Distributed Logger is an API that allows publishing log messages to a DDS Topic.
This will allow your distributed application to send log messages that can be monitored
using tools such as RTI Spy or RTI Admin Console.

It is also possible to use the API directly, creating your own logging infrastructure.

## Example Description

Similar examples are available in other programming languages in the
`rti_workspace/<version>/examples/distributed_logger/<language>/`
folder created under your home directory when RTI Connext is installed.

This example shows a simple application running Distributed Logger to
log several messages using different configurations.

The log messages can then be visualized using RTI Tools such as RTI Spy
or RTI Admin Console.
For example, you may find RTI Spy in your RTI Connext DDS installation
and run it from a terminal as follows:

```
cd rti_connext_dds-7.2.0/bin
./rtiddsspy -printSample
```

Once the Distributed Logger example is running in a different terminal, you should start
seeing state information along with the logged messages on RTI Spy:

```
RTI Connext DDS Spy built with DDS version: 7.2.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtiddsspy is listening for data, press CTRL+C to stop it.

[...]

09:02:38 New data          from 192.168.1.228   : topic="rti/distlog" type="com::rti::dl::LogMessage"
hostAndAppId:
   rtps_host_id: 16867916
   rtps_app_id: 1940525435
level: 400
category: ""
message: "This is a warning message"
messageId: 1

[...]
```

To learn more about the *Distributed Logger*, refer to the Connext DDS
API online documentation. (Eg.
[Modern C++ Distributed Logger](https://community.rti.com/static/documentation/connext-dds/7.2.0/doc/api/connext_dds/distributed_logger/api_cpp2/index.html)).
