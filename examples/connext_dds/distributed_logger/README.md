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

To learn more about the *Distributed Logger*, refer to the Connext DDS
API online documentation. (Eg.
[Modern C++ Distributed Logger](https://community.rti.com/static/documentation/connext-dds/7.2.0/doc/api/connext_dds/distributed_logger/api_cpp2/index.html)).
