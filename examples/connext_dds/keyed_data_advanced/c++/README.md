# Example Code: Keyed Data -- Advanced

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

```sh
# Do not forget to replace the path separator to "\" on Windows.
keys_publisher.exe  <domain_id> <samples_to_send>
keys_subscriber.exe <domain_id> <sleep_periods>
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples for the publisher and
    sleep periods for the subscriber. A value of '0' instructs the application
    to run forever; this is the default.

## Publisher Output

```plaintext
----DW1 registering instance 0
DW1 write; code: 0, x: 1, y: 0
DW2 write; code: 1, x: 2, y: -1000
DW1 write; code: 0, x: 1, y: 1
DW2 write; code: 1, x: 2, y: -1001
DW1 write; code: 0, x: 1, y: 2
DW2 write; code: 1, x: 2, y: -1002
DW1 write; code: 0, x: 1, y: 3
DW2 write; code: 1, x: 2, y: -1003
----DW1 registering instance 1
----DW1 registering instance 2
DW1 write; code: 0, x: 1, y: 4
DW1 write; code: 1, x: 1, y: 1004
DW1 write; code: 2, x: 1, y: 2004
DW2 write; code: 1, x: 2, y: -1004
DW1 write; code: 0, x: 1, y: 5
DW1 write; code: 1, x: 1, y: 1005
DW1 write; code: 2, x: 1, y: 2005
DW2 write; code: 1, x: 2, y: -1005
DW1 write; code: 0, x: 1, y: 6
DW1 write; code: 1, x: 1, y: 1006
DW1 write; code: 2, x: 1, y: 2006
DW2 write; code: 1, x: 2, y: -1006
DW1 write; code: 0, x: 1, y: 7
DW1 write; code: 1, x: 1, y: 1007
DW1 write; code: 2, x: 1, y: 2007
DW2 write; code: 1, x: 2, y: -1007
----DW1 disposing instance 1
DW1 write; code: 0, x: 1, y: 8
DW1 write; code: 2, x: 1, y: 2008
DW2 write; code: 1, x: 2, y: -1008
DW1 write; code: 0, x: 1, y: 9
DW1 write; code: 2, x: 1, y: 2009
DW2 write; code: 1, x: 2, y: -1009
----DW1 unregistering instance 1
DW1 write; code: 0, x: 1, y: 10
DW1 write; code: 2, x: 1, y: 2010
DW2 write; code: 1, x: 2, y: -1010
DW1 write; code: 0, x: 1, y: 11
DW1 write; code: 2, x: 1, y: 2011
DW2 write; code: 1, x: 2, y: -1011
----DW1 unregistering instance 2
----DW1 re-registering instance 1
DW1 write; code: 0, x: 1, y: 12
DW1 write; code: 1, x: 1, y: 1012
DW2 write; code: 1, x: 2, y: -1012
DW1 write; code: 0, x: 1, y: 13
DW1 write; code: 1, x: 1, y: 1013
DW2 write; code: 1, x: 2, y: -1013
DW1 write; code: 0, x: 1, y: 14
DW1 write; code: 1, x: 1, y: 1014
DW2 write; code: 1, x: 2, y: -1014
DW1 write; code: 0, x: 1, y: 15
DW1 write; code: 1, x: 1, y: 1015
DW2 write; code: 1, x: 2, y: -1015
----DW1 re-registering instance 2
DW1 write; code: 0, x: 1, y: 16
DW1 write; code: 1, x: 1, y: 1016
DW1 write; code: 2, x: 1, y: 2016
----DW2 disposing instance 1
DW1 write; code: 0, x: 1, y: 17
DW1 write; code: 1, x: 1, y: 1017
DW1 write; code: 2, x: 1, y: 2017
```

## Subscriber Output

```plaintext
New instance found; code = 0
New instance found; code = 1
code: 1, x: 2, y: -1000
code: 1, x: 2, y: -1001
code: 1, x: 2, y: -1002
code: 1, x: 2, y: -1003
code: 1, x: 1, y: 1004
New instance found; code = 2
code: 2, x: 1, y: 2004
code: 1, x: 1, y: 1005
code: 2, x: 1, y: 2005
code: 1, x: 1, y: 1006
code: 2, x: 1, y: 2006
code: 1, x: 1, y: 1007
code: 2, x: 1, y: 2007
Instance disposed; code = 1
code: 2, x: 1, y: 2008
code: 2, x: 1, y: 2009
code: 2, x: 1, y: 2010
Found reborn instance; code = 1
code: 1, x: 2, y: -1010
code: 2, x: 1, y: 2011
code: 1, x: 2, y: -1011
Instance has no writers; code = 2
code: 1, x: 1, y: 1012
code: 1, x: 1, y: 1013
code: 1, x: 1, y: 1014
code: 1, x: 1, y: 1015
code: 1, x: 1, y: 1016
Found writer for instance; code = 2
code: 2, x: 1, y: 2016
code: 1, x: 1, y: 1017
code: 2, x: 1, y: 2017
Instance has no writers; code = 0
Instance has no writers; code = 2
Instance has no writers; code = 1
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
