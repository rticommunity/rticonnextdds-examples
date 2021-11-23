# Example Code: FlatData and ZeroCopy examples

## Explore the Example

### CameraImage.idl

`CameraImage.idl` defines four different versions of `CameraImage`, with
different annotations for each mode.

### CameraImage_publisher.cxx and CameraImage_subscriber.cxx

`CameraImage_publisher.cxx` and `CameraImage_subscriber.cxx` implement each mode
in a separate function:

-   `CameraImage_publisher.cxx`:

    - `publisher_flat()`
    - `publisher_zero_copy()`
    - `publisher_flat_zero_copy()`
    - `publisher_plain()`

-   `CameraImage_subscriber.cxx`:

    - `subscriber_flat()`
    - `subscriber_zero_copy()`
    - `subscriber_flat_zero_copy()`
    - `subscriber_plain()`

These functions are very similar but have a few differences specific to each
feature:

-   **Language binding**: `*_flat` and `*_flat_zero_copy` use the FlatData
    language binding, with a distinct way to create and access the data sample.
    `*_zero_copy` and `*_plain` use the regular (`PLAIN`) language binding.

-   **Sample lifecycle**: `*_flat`, `*_zero_copy` and `*_flat_zero_copy` use
    DataWriter-managed samples, obtained with the `get_loan` function. The
    application doesn't delete these samples.

### Common.hpp

`Common.hpp` implements functionality used by both applications:

- Waiting for discovery
- Functions to populate and print the data
- Application options

### QoS Settings

The FlatData profile used in this example inherits from the builtin
`StrictReliable.LargeData` profile. This builtin profile sets up the middleware
for sending large data by using Asynchronous Pubisher, a default Flow Controller
and other optimizations. In the FlatData profile, the *Durability QoS* of the
DataWriter is set to volatile to enable managed samples to be reused when they
are acknowledged. The *DataRepresentation QoS* is set to *Extended CDR version
2* at both the DataWriter and the DataReader as FlatData language binding is
only supported with this data representation. The tranport settings of the
participant are also tuned to ensure reduced latency while sending large data.

## Building the Example :wrench:

To build this example, first run CMake to generate the corresponding build
files. We recommend you use a separate directory to store all the generated
files (e.g., ./build).

```bash
mkdir build
cd build
cmake ..
```

Once you have run CMake, you will find a number of new files in your build
directory (the list of generated files will depend on the specific CMake
Generator). To build the example, run CMake as follows:

```sh
cmake --build .
```

**Note**: if you are using a multi-configuration generator, such as Visual
Studio solutions, you can specify the configuration mode to build as follows:

```sh
cmake --build . --config Release|Debug
```

Alternatively, you can use directly the generated infrastructure (e.g.,
Makefiles or Visual Studio Solutions) to build the example. If you generated
Makefiles in the configuration process, run make to build the example. Likewise,
if you generated a Visual Studio solution, open the solution and follow the
regular build process.

## Running the Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

On *Windows* systems run:

```sh
CameraImage_publisher.exe -d <domain_id> -m <mode> -s <sample_count> -e <exec_time> -n <ip_addres>
CameraImage_subscriber.exe -d <domain_id> -m <mode> -n <ip_address> --display_samples
```

On *UNIX* systems run:

```sh
./CameraImage_publisher -d <domain_id> -m <mode> -s <sample_count> -e <exec_time> -n <ip_addres>
./CameraImage_subscriber -d <domain_id> -m <mode> -n <ip_address> --display_samples
```

The application has the following arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  Publisher subscriber/mode.
    1.  publisher_flat
    2.  publisher_zero_copy
    3.  publisher_flat_zero_copy
    4.  publisher_plain

3.  (Publisher only) How long the examples should run, measured in samples
(-s option). The default is infinite.

4.  (Publisher only) The `<exec_time>` in seconds.

5.  (Subscriber only) If the `--display_samples` option is given, the
subscriber will print the sample data.

6.  When a device has multiple interfaces, it is recommended to use `-n` when
running on separate nodes. This ensures that data is sent over one network
interface only.

## Publisher Output

*CameraImage_publisher* waits for the subscriber and then prints the average
one-way latency every 4 seconds:

```plaintext
Running publisher_zero_copy
Waiting for the subscriber application
Discovery complete
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 81 microseconds
Average end-to-end latency: 82 microseconds
```

## Subscriber Output

*CameraImage_subscriber* waits for the publisher and responds to each ping with
a pong message:

```plaintext
Running subscriber_zero_copy
Waiting for the publisher application
Discovery complete
```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify the
default behavior:

-   `-DCMAKE_BUILD_TYPE` -- specifies the build mode. Valid values are Release
    and Debug. See the [CMake documentation for more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

-   `-DBUILD_SHARED_LIBS` -- specifies the link mode. Valid values are ON for
    dynamic linking and OFF for static linking. See [CMake documentation for
    more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

-   `-G` -- CMake generator. The generator is the native build system to use
    build the source code. All the valid values are described described in the
    CMake documentation [CMake Generators
    Section.](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

For example, to build a example in Debug/Static mode run CMake as follows:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON .. -G "Visual Studio 15 2017" -A x64
```

### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform.If you installed Connext DDS in a custom location, you
can use the CONNEXTDDS_DIR variable to indicate the path to your RTI Connext DDS
installation folder. For example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, If you installed libraries for multiple target architecture on your system
(i.e., you installed more than one target rtipkg), you can use the
CONNEXTDDS_ARCH variable to indicate the architecture of the specific libraries
you want to link against. For example:

```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```

### CMake Build Infrastructure

The CMakeListst.txt script that builds this example uses a generic CMake
function called connextdds_add_example that defines all the necessary constructs
to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Copy the USER_QOS_PROFILES.xml file into the directory where the publisher
    and subscriber executables are generated.

You will find the definition of connextdds_add_example, along with detailed
documentation, in
[resources/cmake/ConnextDdsAddExample.cmake](../../../../resources/cmake/ConnextDdsAddExample.cmake).

For a more comprehensive example on how to build an RTI Connext DDS application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which includes
a comprehensive CMakeLists.txt script with all the steps and instructions
described in detail.
