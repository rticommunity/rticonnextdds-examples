# Example Code: Hello Simple


## Concept
*Connext* is network middleware for real-time distributed applications. It
provides the communications service that programmers need to distribute
time-critical data between embedded and / or enterprise devices or nodes.
*Connext* uses a *publish-subscribe* communications model to make
data-distribution efficient and robust.


## Example Description
This a example about how to write a simple application using *RTI Connext DDS*
as it is explained in the [Getting Started](https://community.rti.com/rti-doc/510/ndds.5.1.0/doc/pdf/RTI_CoreLibrariesAndUtilities_GettingStarted.pdf)
manual. Please refer to it for a more detailed explanation.

There is one application for a publisher and another for a subscriber (server).
The publisher reads the standard input and publishes it using the
*Builtin Type String*. On the other side, every sample received by the
subscriber is written into the standard output.
