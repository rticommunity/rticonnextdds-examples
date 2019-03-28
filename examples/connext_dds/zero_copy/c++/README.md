# Example Code to demonstrate the usage of zero copy

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
Makefiles in the configuration process, run make to build the example.
Likewise, if you generated a Visual Studio solution, open the solution and
follow the regular build process.

## Running the Example

To run the example applications you can use the generated makefile. In one
terminal situated in the example directory, run:

### Publisher application

For x64Linux3gcc4.8.2, run:

```sh
./zero_copy_publisher [options]
```

Where [options]:

```
    -h                  Shows this page
    -d  [domain_id]     Sets the domain id **Default**: 0
    -fc [frame count]   Sets the total number of frames to be mapped in the shared memory queue **Default**: frame rate
    -fr [frame rate]    Sets the rate at which frames are written **Default**: 60fps
    -sc [sample count]  Sets the number of frames to send **Default**: 1200
    -k  [key]               Sets the key for shared memory segment **Default**: obtained automatically
    -s  [buffer size]   Sets payload size of the frame in bytes **Default**: 1048576 (1MB)
    -rc [dr count]      Expected number of DataReaders that will receive frames **Default**: 1
    -v                  Displays the checksum for each frame
```

### Subscriber application

For x64Linux3gcc4.8.2, run:

```sh
./zero_copy_subscriber [options]
```

Where [options]:

```
    -h                  Shows this page
    -d  [domain_id]     Sets the domain id **Default**: 0
    -sc [sample count]  Sets the number of frames to receive **Default**: 1200
    -s  [buffer size]   Sets the payload size of the frame in bytes **Default**: 1048576 (1MB)
    -v                  Displays checksum and computed latency of each received frame
```

## Publisher Output

```
Running using:
  Domain ID:  0
  Frame Count in SHMEM segment: 60
  Frame Rate: 60 fps
  Sample Count: 1200
  SHMEM Key: 1371756096
  Frame size: 1048576 bytes
  Expected DataReader Count: 1
Waiting until 1DataReaders are discovered
Going to send 1200frames at 60 fps
Writen 60 frames
Writen 120 frames
Writen 180 frames
Writen 240 frames
```

## Subscriber Output

```
Running using:
  Domain ID:  0
  Sample Count: 1200
  Frame size: 1048576 bytes
ZeroCopy subscriber waiting to receive samples...
Average latency: 249 microseconds
Average latency: 225 microseconds
Average latency: 223 microseconds
Average latency: 219 microseconds
```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify
the default behavior:

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
    Section.](https://cmake.org/cmake/help/v3.13/manual/cmake-generators.7.html)

For example, to build a example in Debug/Static mode run CMake as follows:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON ..
```

### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform.If you installed Connext DDS in a custom location, you
can use the CONNEXTDDS_DIR variable to indicate the path to your RTI Connext
DDS installation folder. For example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, If you installed libraries for multiple target architecture on your
system (i.e., you installed more than one target rtipkg), you can use the
CONNEXTDDS_ARCH variable to indicate the architecture of the specific libraries
you want to link against. For example:

```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```

### CMake Build Infrastructure

The CMakeListst.txt script that builds this example uses a generic CMake
function called connextdds_add_example that defines all the necessary
constructs to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Copy the USER_QOS_PROFILES.xml file into the directory where the publisher
    and subscriber executables are generated.

You will find the definition of connextdds_add_example, along with detailed
documentation, in
[rescources/cmake/ConnextDdsAddExample.cmake](../../../../rescources/cmake/ConnextDdsAddExample.cmake).

For a more comprehensive example on how to build an RTI Connext DDS application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which
includes a comprehensive CMakeLists.txt script with all the steps and
instructions described in detail.
