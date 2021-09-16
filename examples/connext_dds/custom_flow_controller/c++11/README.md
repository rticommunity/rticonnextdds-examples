# Example Code: Custom Flowcontroller

## Building the Example :wrench:

To build this example, first run CMake to generate the corresponding build
files. We recommend you use a separate directory to store all the generated
files (e.g., ./build).

```sh
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
cfc_publisher.exe  -d <domain_id> -s <samples_to_send>
cfc_subscriber.exe -d <domain_id> -s <samples_to_receive>
```

On *UNIX* systems run:

```sh
./cfc_publisher  -d <domain_id> -s <samples_to_send>
./cfc_publisher  -d <domain_id> -s <samples_to_receive>
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples. The default is
    infinite.

## Publisher Output

```plaintext
Writing cfc, sample 0
Writing cfc, sample 1
Writing cfc, sample 2
Writing cfc, sample 3
Writing cfc, sample 4
Writing cfc, sample 5
Writing cfc, sample 6
Writing cfc, sample 7
Writing cfc, sample 8
Writing cfc, sample 9
Writing cfc, sample 10
Writing cfc, sample 11
Writing cfc, sample 12
Writing cfc, sample 13
Writing cfc, sample 14
Writing cfc, sample 15
Writing cfc, sample 16
Writing cfc, sample 17
Writing cfc, sample 18
Writing cfc, sample 19
Writing cfc, sample 20
Writing cfc, sample 21
Writing cfc, sample 22
Writing cfc, sample 23
Writing cfc, sample 24
Writing cfc, sample 25
Writing cfc, sample 26
Writing cfc, sample 27
Writing cfc, sample 28
Writing cfc, sample 29
```

## Subscriber Output

```plaintext
@ t=0.032710s, got x = 0
@ t=0.032841s, got x = 1
@ t=0.033193s, got x = 2
@ t=0.033250s, got x = 3
@ t=0.033609s, got x = 4
@ t=0.033661s, got x = 5
@ t=0.033793s, got x = 6
@ t=0.033848s, got x = 7
@ t=0.034092s, got x = 8
@ t=0.034146s, got x = 9
@ t=0.035744s, got x = 10
@ t=0.035796s, got x = 11
@ t=0.036038s, got x = 12
@ t=0.036092s, got x = 13
@ t=0.036389s, got x = 14
@ t=0.036464s, got x = 15
@ t=0.036760s, got x = 16
@ t=0.036837s, got x = 17
@ t=0.037029s, got x = 18
@ t=0.037117s, got x = 19
@ t=0.038716s, got x = 20
@ t=0.038770s, got x = 21
@ t=0.038981s, got x = 22
@ t=0.039077s, got x = 23
@ t=0.039297s, got x = 24
@ t=0.039404s, got x = 25
@ t=0.039906s, got x = 26
@ t=0.039999s, got x = 27
@ t=0.040254s, got x = 28
@ t=0.040313s, got x = 29
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
