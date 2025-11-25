# Example Description

This example includes publisher and subscriber applications provided to send
and receive simple integers under the `hello_world` type.

The `PersistenceServiceConfig.xml` file persists the `Example hello_world`
topic published by the provided publisher and subscriber. You can adjust the
domain ID used by passing the `-domainId` argument to the *Persistence Service*
executable or `-domain_id` argument to the provided publisher and subscriber.

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

Before following the steps in this example make sure that you have set the
`NDDSHOME` environment variable to point to the top level directory where
*RTI Connext* is installed.

1.  Run *Persistence Service* by using either the `defaultTransient` or
    `defaultPersistent` configuration, depending on the kind of scenario you
    are running. You can also control on which domain ID *Persistence Service*
    runs by using the `-domainId` argument.

    For this step we use the `defaultTransient` configuration while running on
    domain ID 70.

    ```sh
    $NDDSHOME/bin/rtipersistenceservice -cfgName defaultTransient
    -cfgFile PersistenceServiceConfig.xml -domainId 70
    ```

    You should see the output:

    ```sh
    RTI Persistence Service started
    ```

    **Note:** If you want to use the `defaultPersistent` configuration you need to
    create a directory named `MyDirectory` in the your present working directory.
    The files containing the samples will be located in there. If you want to
    change the location you can use the tag `<directory>` under `<filesystem>`.
    If you already had the `MyDirectory` directory, make sure you clean its
    contents to avoid receiving samples from your previous runs.

2.  Run the publisher from the `build` directory you created when compiling the
    example. The publisher and subscriber have the `-domain_id` and `-persistent`
    arguments to control the domain ID and the type of *Persistence Service*
    scenario you are running.

    For this step we use the opt for using `-persistent 0` to match step 2 of
    *Persistence Service* while running on domain ID 70.

    ```sh
    ./hello_world_publisher -domain_id 70 -persistent 0 -sample_count 10
    ```

    **Note**: The `-sample_count` argument controls how many samples the publisher
    will publish.

    You should see the output:

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

    You should see the output:

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

    You should see the output:

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

9.  Restart RTI Persistence Service with the `defaultPersistent` configuration.

    ```sh
    $NDDSHOME/bin/rtipersistenceservice -cfgName defaultPersistent
    -cfgFile PersistenceServiceConfig.xml -domainId 70
    ```

    Start a subscriber and verify that it receives the persisted samples from
    RTI Persistence Service:

    ```sh
    ./hello_world_subscriber -domain_id 70 -persistent 1
    ```

    You should see the output:

    ```sh
    [data: 0]
    [data: 1]
    [data: 2]
    [data: 3]
    [data: 4]
    ```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
Solutions on Windows). You can use the following CMake variables to modify the
default behavior:

-   `-DCMAKE_BUILD_TYPE` - specifies the build mode. Valid values are `Release`
    and `Debug`. See the [CMake documentation for more details
    (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html).

-   `-DBUILD_SHARED_LIBS` - specifies the link mode. Valid values are `ON` for
    dynamic linking and `OFF` for static linking. See [CMake documentation for
    more details
    (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html).

-   `-G` - CMake generator. The generator is the native build system used to
    build the source code. All the valid values are described in the CMake
    documentation for [CMake
    Generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html).

For example, to build an example in Debug/Dynamic mode run CMake as follows:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON .. -G "Visual Studio 15 2017" -A x64
```

### Configuring Connext Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
installation and the Connext architecture based on the default settings
for your host platform. If you installed Connext in a custom location, you
can use the `CONNEXTDDS_DIR` variable to indicate the path to your RTI Connext
installation folder. For example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, if you installed libraries for multiple target architectures on your system
(i.e., you installed more than one target `.rtipkg` file), you can use the
`CONNEXTDDS_ARCH` variable to indicate the architecture of the specific libraries
you want to link against. For example:

```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```

### Cross-compilation

When you need to cross-compile the example, the above
command will not work, the assigned compiler won't be the cross-compiler and
errors may happen when linking against the cross-compiled Connext binaries.
To fix this, you have to create a file with the architecture name and call
CMake with a specific flag called ``-DCMAKE_TOOLCHAIN_FILE``.
An example of the file to create with the toolchain settings (e.g. for an
ARM architectures):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(toolchain_path "<path to>/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian")
set(CMAKE_C_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-g++")
```

Then you can call CMake like this:

```bash
    cmake -DCONNEXTDDS_DIR=<connext dir> -DCMAKE_TOOLCHAIN_FILE=<toolchain file created above>
            -DCONNEXTDDS_ARCH=<connext architecture> ..
```

### CMake Build Infrastructure

The `CMakeListst.txt` script that builds this example uses a generic CMake
function called `connextdds_add_example` that defines all the necessary constructs
to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Copy the `USER_QOS_PROFILES.xml` file into the directory where the publisher
    and subscriber executables are generated.

You will find the definition of `connextdds_add_example`, along with detailed
documentation, in
[resources/cmake/rticonnextdds-cmake-utils/cmake/Modules/ConnextDdsAddExample.cmake
](https://github.com/rticommunity/rticonnextdds-cmake-utils/blob/main/cmake/Modules/ConnextDdsAddExample.cmake).

For a more comprehensive example on how to build an RTI Connext application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which includes
a comprehensive `CMakeLists.txt` script with all the steps and instructions
described in detail.


## Troubleshooting

### Compilation fails accessing struct field

If the code compilation fails with errors such as "reference to non-static member
function must be called" for code such as `my_sample.my_field = value` or
`value = my_sample.my_field` this means that the rtiddsgen version you are using
doesn't have the IDL4 C++ mapping enabled by default.

To fix it, upgrade your Connext version to 7.6+ or check out the branch for the
Connext version you're using, e.g.

```sh
git checkout release/7.3.0
```
