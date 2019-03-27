# FlatData and ZeroCopy examples

## Concept

Connext 6 introduces two features - *Zero Copy Transfer over Shared Memory* and *FlatData Language Binding*. These features enable the middleware to reduce the number of copies involved in sending a sample, resulting in lower latencies. In the case of *Zero Copy Transfer over Shared Memory*, a fixed minimal latency is guaranteed regardless of the size of the sample. 

To learn more about these two features, refer to the 
[User's Manual](https://community.rti.com/static/documentation/connext-dds/current/doc/manuals/connext_dds/html_files/RTI_ConnextDDS_CoreLibraries_UsersManual/index.htm#UsersManual/SendingLargeData.htm%3FTocPath%3DPart%25203%253A%2520Advanced%2520Concepts%7C22.%2520Sending%2520Large%2520Data%7C_____0) and the FlatData section of the 
[API Reference](https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp2/group__RTIFlatDataModule.html).

## Example Description

This example demonstrates the benefits of *Zero Copy Transfer over Shared Memory* and *FlatData Language Binding* using a large-data ping/pong latency application. The applications are written in C++11 using the modern C++ API. However, the traditional C++ API supports both features as well.


*CameraImage_publisher* publishes the ping topic and subscribes to the pong topic; *CameraImage_subscriber* subscribes to the ping topic and publishes the pong topic. Both topics use the same type, `CameraImage`,
containing a 25MB array.

Each application can be run in 4 different modes by using the `-mode` command-line parameter:

*  Mode 1 - FlatData
*  Mode 2 - Zero Copy
*  Mode 3 - FlatData + Zero Copy
*  Mode 4 - Regular DDS application

You can choose the same or a different mode for each application.

The applications can run on the same or different nodes, although the benefits of the Zero Copy Transfer over Shared Memory can be observed only when running on the same node.


The example application allows one-to-one tests and is not prepared to run multiple subscribers and/or publisher.

## Compiling the example

Use rtiddsgen to generate a makefile for your machine's architecture and compiler. For example:

```
rtiddsgen -language C++11 -example x64Darwin17clang9.0 CameraImage.idl
```

You may get messages saying that some files already exist and will not be replaced. You can safely ignore those messages, since all the source files for this example are already provided.

Compile the Publisher and Subscriber apps:

```
make -f <makefile for your platform>
```

The executables *CameraImage_publisher* and *CameraImage_subscriber* are located under **objs/\<architecture\>**

## Running the example

### Usage

```
Usage: ./objs/x64Darwin17clang9.0/CameraImage_[publisher|subscriber] [options]

Options:
 -domainId    <domain ID>    Domain ID
 -mode        <1,2,3,4>      Publisher modes
                                 1. publisher_flat
                                 2. publisher_zero_copy
                                 3. publisher_flat_zero_copy
                                 4. publisher_plain
 -sampleCount <sample count> Sample count
                             Default: -1 (infinite)
 -executionTime <sec>        Execution time in seconds
                             Default: 30
 -nic         <IP address>   Use the nic specified by <ipaddr> to send
                             Default: automatic
 -help                       Displays this information

```

When a device has multiple interfaces, it is recommended to use `-nic` when running on separate nodes. This ensures that data is sent over one network interface only.

## Output

*CameraImage_publisher* waits for the subscriber and then prints the average one-way latency every 4 seconds:

```
Running publisher_zero_copy
Waiting for the subscriber application
Discovery complete
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 82 microseconds
```

*CameraImage_subscriber* waits for the publisher and responds to each ping with a pong message:

```
Running subscriber_zero_copy
Waiting for the publisher application
Discovery complete
```

## Explore the code

`CameraImage.idl` defines four different versions of `CameraImage`, with different annotations for each mode.

`CameraImage_publisher.cxx` and `CameraImage_subscriber.cxx` implement each mode in a separate function:

`CameraImage_publisher.cxx`:
- `publisher_flat()`
- `publisher_zero_copy()`
- `publisher_flat_zero_copy()`
- `publisher_plain()`

`CameraImage_subscriber.cxx`:
- `subscriber_flat()`
- `subscriber_zero_copy()`
- `subscriber_flat_zero_copy()`
- `subscriber_plain()`

These functions are very similar but have a few differences specific to each feature:
- **Language binding**: `*_flat` and `*_flat_zero_copy` use the FlatData language binding, with a distinct way to create and access the data sample.

	`*_zero_copy` and `*_plain` use the regular (`PLAIN`) language binding.
- **Sample lifecycle**: `*_flat`, `*_zero_copy` and `*_flat_zero_copy` use DataWriter-managed samples, obtained with the `get_loan` function. The application doesn't delete these samples.

`Common.hpp` implements functionality used by both applications:
- Waiting for discovery
- Functions to populate and print the data
- Application options

### QoS Settings

The FlatData profile used in this example inherits from the builtin StrictReliable.LargeData profile. This builtin profile sets up the middleware for sending large data by using Asynchronous Pubisher, a default Flow Controller and other optimizations. In the FlatData profile, the *Durability QoS* of the DataWriter is set to volatile to enable managed samples to be reused when they are acknowledged. The *DataRepresentation QoS* is set to *Extended CDR version 2* at both the DataWriter and the DataReader as FlatData language binding is only supported with this data representation. The tranport settings of the participant are also tuned to ensure reduced latency while sending large data.

