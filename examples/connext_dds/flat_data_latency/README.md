# Example Code: FlatData and ZeroCopy examples

## Concept

Connext 6 introduces two features - *Zero Copy Transfer over Shared Memory* and
*FlatData Language Binding*. These features enable the middleware to reduce the
number of copies involved in sending a sample, resulting in lower latencies. In
the case of *Zero Copy Transfer over Shared Memory*, a fixed minimal latency is
guaranteed regardless of the size of the sample.

To learn more about these two features, refer to the
[User's Manual](https://community.rti.com/static/documentation/connext-dds/current/doc/manuals/connext_dds/html_files/RTI_ConnextDDS_CoreLibraries_UsersManual/index.htm#UsersManual/SendingLargeData.htm%3FTocPath%3DPart%25203%253A%2520Advanced%2520Concepts%7C22.%2520Sending%2520Large%2520Data%7C_____0) and the FlatData section of the
[API Reference](https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp2/group__RTIFlatDataModule.html).

## Example Description

This example demonstrates the benefits of *Zero Copy Transfer over Shared
Memory* and *FlatData Language Binding* using a large-data ping/pong latency
application. The applications are written in C++11 using the modern C++ API.
However, the traditional C++ API supports both features as well.

*CameraImage_publisher* publishes the ping topic and subscribes to the pong
topic; *CameraImage_subscriber* subscribes to the ping topic and publishes the
pong topic. Both topics use the same type, `CameraImage`,
containing a 25MB array.

Each application can be run in 4 different modes by using the `-mode`
command-line parameter:

* Mode 1 - FlatData
* Mode 2 - Zero Copy
* Mode 3 - FlatData + Zero Copy
* Mode 4 - Regular DDS application

You can choose the same or a different mode for each application.

The applications can run on the same or different nodes, although the benefits
of the Zero Copy Transfer over Shared Memory can be observed only when running
on the same node.

The example application allows one-to-one tests and is not prepared to run
multiple subscribers and/or publisher.
