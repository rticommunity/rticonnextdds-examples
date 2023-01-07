# Example Code: Persistence Service and Library API

## Persistence Service

In many applications, we need to store data samples so they can be delivered to
subscribing applications that join the system at a later time or to avoid
receiving old data each time we re-join to the same publisher. Or maybe we need
to keep the data available even once our publishing application has terminated.
In *RTI Connext DDS* this behavior can be achieved using *Persistence Service*.

*Persistence Service* is a *Connext* application that saves data samples to
transient or permanent storage, so they can be delivered to subscribing
applications that join the system at a later time - even if the publishing
application has already terminated.

When configured to run in `PERSISTENT` mode, *Persistence Service* uses the
filesystem. For each persistent topic, it collects all the data written by
the corresponding persistent *DataWriters* and stores them into persistent
storage. When configured to run in `TRANSIENT` mode, *Persistence Service*
stores the data in memory.

## Example Description

This example includes publisher and subscriber applications provided to send
and receive simple integers under the `hello_world` type.

The `PersistenceServiceConfig.xml` file persists the `Example hello_world`
topic published by the provided publisher and subscriber. You can adjust the
domain ID used by passing the `-domainId` argument to the *Persistence Service*
executable or `-domain_id` argument to the provided publisher and subscriber.

The example also includes a demonstration of how to use the *Persistence Service*
Library API for encapsulating the functionality of *Persistence Service* within
your own application. The Library API is supported for C.

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

**Note**: If you are using a multi-configuration generator, such as Visual
Studio solutions, you can specify the configuration mode to build as follows:

```sh
cmake --build . --config Release|Debug
```

Alternatively, you can use directly the generated infrastructure (e.g.,
Makefiles or Visual Studio Solutions) to build the example. If you generated
Makefiles in the configuration process, run make to build the example. Likewise,
if you generated a Visual Studio solution, open the solution and follow the
regular build process.

## Running the example

Before follwing the steps in this example make sure that you have set the
`NDDSHOME` environment variable to point to the top level directory where
*RTI Connext DDS* is installed.

1.  Run *Persistence Service* from the top level directory for this example
    that contains the `PersistenceServiceConfig.xml`. You can choose from
    `defaultTransient` or `defaultPersistent` depending on the kind of scenario
    you are running. You can also control on which domain ID *Persistence Service*
    runs by using the `-domainId` argument.

    For this step we use the `defaultTransient` configuration while running on
    domain ID 70.

    ```sh
    $NDDSHOME/bin/rtipersistenceservice -cfgName defaultTransient
    -cfgFile PersistenceServiceConfig.xml -domainId 70
    ```

    If everything goes well you should see:

    ```sh
    RTI Persistence Service started
    ```

    Note: If you want to use the `defaultPersistent` configuration you need to
    create a directory named `MyDirectory` in the your present working directory.
    The files containing the samples will be located in there. If you want to
    change the location you can use the tag `<directory>` under `<filesystem>`.

2.  Run the publisher from the `build` directory you created when compiling the
    example. The publisher and subscriber have the `-domain_id` and `-persistent`
    arguments to control the domain ID and the type of *Persistence Service*
    scenario you are running.

    For this step we use the opt for using `-persistent 0` to match step 2 of
    *Persistence Service* while running on domain ID 70.

    ```sh
    ./hello_world_publisher -domain_id 70 -persistent 0 -sample_count 10
    ```

    Note: The `-sample_count` argument controls how many samples the publisher
    will publish.

    If everything goes well you should see:

    ```sh
    Writing hello_world, count 0
    Writing hello_world, count 1
    Writing hello_world, count 2
    Writing hello_world, count 3
    Writing hello_world, count 4
    ```

3.  Run the subscriber from the `build` directory you created when compiling the
    example. The publisher and subscriber have the `-domain_id` and `-persistent`
    arguments to control the domain ID and the type of *Persistence Service*
    scenario you are running.

    For this step we use the `defaultTransient` configuration while running on
    domain ID 70.

    ```sh
    ./hello_world_subscriber -domain_id 70 -persistent 0
    ```

    If everything goes well you should see:

    ```sh
    [data: 0]
    [data: 1]
    [data: 2]
    [data: 3]
    [data: 4]
    ```

4.  Stop the publisher by pressing CTRL-C.

5.  Start a new subscriber and verify that it receives samples from *Persistence
    Service* in the absence of the original publisher.

    ```sh
    ./hello_world_subscriber -domain_id 70 -persistent 0
    ```

    If everything goes well you should see:

    ```sh
    [data: 0]
    [data: 1]
    [data: 2]
    [data: 3]
    [data: 4]
    ```

6.  Stop RTI Persistence Service and the two subscribers by pressing CTRL-C.

7.  Restart RTI Persistence Service. This time we use the `defaultPersistent`
    configuration to persist data to the disk.

    ```sh
    $NDDSHOME/bin/rtipersistenceservice -cfgName defaultPersistent
    -cfgFile PersistenceServiceConfig.xml -domainId 70
    ```

8.  Repeat steps 2 to 6 but this time use `-persistent 1` when starting the
    publisher or subscriber application.

9.  For the last step, we will be using the sample application that demonstrates
    the usage of *Persistence Service* Library API. The `PersistenceServiceLibraryAPI`
    application is created when this example is compiled.

    It takes the `-domain_id` and `-persistent` arguments like the publisher and
    subscriber applications.

    Run `PersistenceServiceLibraryAPI` from the top level directory for this example
    that contains the `PersistenceServiceConfig.xml`

    Rerun steps 2 to 6 for each invocation of `PersistenceServiceLibraryAPI` -
    one with `-persistent 0` that corresponds to `defaultTransient`, and one with
    `-persistent 1` that corresponds to `defaultPersistent`.

    ```sh
    ./build/PersistenceServiceLibraryAPI -domain_id 70 -persistent <0|1>
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
