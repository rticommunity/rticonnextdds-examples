# Example Code: Persistent Storage

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

**Note: If you are using *TimesTen*, you may need run the command prompt as
administrator. Otherwise, none of the scenarios will not run correctly.**

**For TimesTen users**: In addition, you need to uncomment some lines in the
*USER_QOS_PROFILES.xml* and *persistence_service_configuration.xml* to load the
correct driver (we assuming you have installed *TimesTen 11.2.1*). Refer to
those documents for further details.

-   Durable Writer History scenario

    ```sh
    # Do not forget to replace the path separator to "\" on Windows.
    hello_world_subscriber
    hello_world_publisher -sample_count 5 -initial_value 0 -dwh 1
    hello_world_subscriber
    hello_world_publisher -sample_count 5 -initial_value 5 -dwh 1
    ```

-   Durable Reader State Scenario

    ```sh
    # Do not forget to replace the path separator to "\" on Windows.
    hello_world_subscriber.exe -drs 1
    hello_world_publisher.exe -sample_count 5 -initial_value 0 -sleep 60
    stop hello_world_subscriber
    hello_world_subscriber.exe
    hello_world_subscriber.exe -drs 1
    ```

-   Persistence Service Scenario

    ```sh
    # Do not forget to replace the path separator to "\" and
    # "$NDDSHOME" by "%NDDSHOME%" on Windows.

    # Run persistence service (in the same folder that there are
    # persistence_service_configuration.xml):
    "$NDDSHOME/bin/rtipersistenceservice" -cfgFile persistence_service_configuration.xml -cfgName <persistence_service_database|persistence_service_filesystem>

    hello_world_subscriber
    hello_world_publisher -sample_count 5 -initial_value 0
    hello_world_subscriber
    ```

The applications accepts different arguments:

```plaintext
hello_world_subscriber:
    -domainId <domain ID> (default: 0)
    -sample_count <sample_count> (default: infinite => 0)
    -drs <1|0> Enable/Disable durable reader state (default: 0)

hello_world_publisher:
    -domainId <domain ID> (default: 0)
    -sample_count <number of published samples> (default: infinite)
    -initial_value <first sample value> (default: 0)
    -sleep <sleep time in seconds before finishing> (default: 0)
    -dwh <1|0> Enable/Disable durable writer history (default: 0)
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
