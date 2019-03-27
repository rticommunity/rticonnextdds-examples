# Example Code: Flat Data API

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

This example shows how to create, write, and read FlatData samples for a
mutable type. *CameraImage_publisher* and *CameraImage_subscriber* are simple
applications using the RTI FlatData language binding.
