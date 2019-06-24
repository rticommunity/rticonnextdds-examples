# Example Code: FlatData and ZeroCopy examples

## Explore C++11 Example

### CameraImage.idl

`CameraImage.idl` defines four different versions of `CameraImage`, with
different annotations for each mode.

### CameraImage_publisher.cxx and CameraImage_subscriber.cxx

`CameraImage_publisher.cxx` and `CameraImage_subscriber.cxx` implement each
mode in a separate function:

* `CameraImage_publisher.cxx`:
  * `publisher_flat()`
  * `publisher_zero_copy()`
  * `publisher_flat_zero_copy()`
  * `publisher_plain()`

* `CameraImage_subscriber.cxx`:
  * `subscriber_flat()`
  * `subscriber_zero_copy()`
  * `subscriber_flat_zero_copy()`
  * `subscriber_plain()`

These functions are very similar but have a few differences specific to each
feature:

* **Language binding**: `*_flat` and `*_flat_zero_copy` use the FlatData
  language binding, with a distinct way to create and access the data sample.
   `*_zero_copy` and `*_plain` use the regular (`PLAIN`) language binding.
* **Sample lifecycle**: `*_flat`, `*_zero_copy` and `*_flat_zero_copy` use
  DataWriter-managed samples, obtained with the `get_loan` function. The
  application doesn't delete these samples.

### Common.hpp

`Common.hpp` implements functionality used by both applications:

* Waiting for discovery
* Functions to populate and print the data
* Application options

### QoS Settings

The FlatData profile used in this example inherits from the builtin
`StrictReliable.LargeData` profile. This builtin profile sets up the middleware
for sending large data by using Asynchronous Pubisher, a default Flow
Controller and other optimizations. In the FlatData profile, the *Durability
QoS* of the DataWriter is set to volatile to enable managed samples to be
reused when they are acknowledged. The *DataRepresentation QoS* is set to
*Extended CDR version 2* at both the DataWriter and the DataReader as FlatData
language binding is only supported with this data representation. The tranport
settings of the participant are also tuned to ensure reduced latency while
sending large data.

## Building C++11 Example

Use rtiddsgen to generate a makefile for your machine's architecture and
compiler. For example:

```bash
rtiddsgen -language C++11 -additionalHeaderFile Common.hpp -example <your_target_architecture> CameraImage.idl
```

You may get messages saying that some files already exist and will not be
replaced. You can safely ignore those messages, since all the source files for
this example are already provided.

Compile the Publisher and Subscriber apps:

```bash
make -f <makefile_for_your_target_architecture>
```

The executables *CameraImage_publisher* and *CameraImage_subscriber* are
located under `objs/\<architecture\>`.

## Running C++11 Example

### Usage

```plain
Usage: ./objs/<archictecture>/CameraImage_[publisher|subscriber] [options]

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

When a device has multiple interfaces, it is recommended to use `-nic` when
running on separate nodes. This ensures that data is sent over one network
interface only.

### Output

*CameraImage_publisher* waits for the subscriber and then prints the average
one-way latency every 4 seconds:

```plain
Running publisher_zero_copy
Waiting for the subscriber application
Discovery complete
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 82 microseconds
```

*CameraImage_subscriber* waits for the publisher and responds to each ping
with a pong message:

```plain
Running subscriber_zero_copy
Waiting for the publisher application
Discovery complete
```
