# Example Code: Routing Service Transformation

Below there are the instructions to build and run this example. All the commands
and syntax used assume a Unix-based system. If you run this example in a
different architecture, please adapt the commands accordingly.

## Building the Example :wrench:

In order to build this example, you need to define the variable `CONNEXTDDS_DIR`
You can do so by exporting them manually, by sourcing the `rtisetenv` script for
your architecture, or by passing them to the `cmake` command as arguments:

```bash
mkdir build
cd build
cmake -DCONNEXTDDS_DIR=<Connext DDS Directory> \     # If not exported
      -DBUILD_SHARED_LIBS=ON \
      -DCMAKE_BUILD_TYPE=Debug|Release ..
cmake --build .
```

**Note**: You do not need to define `CONNEXTDDS_ARCH` if you only have one
architecture target installed in your system.

**Note**: when compiling on a Windows 64-bit machine you will need to add the
`-A x64` parameter to the call to CMake. See
[Customizing the Build](#customizing-the-build) for more details.

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

Upon success it will create in the build directory:

-   A shared library file that represents the *Transformation* plug-in.

-   A publisher application  executable with name
    `SensorAttributesCollectionPublisher`.

-   A subscriber application executable with name `SensorDataSubscriber`.

## Running the Example

To run this example you will need an instance of the publisher application, and
instance of the subscriber application, and an instance of *RoutingService*.

To run *RoutingService*, you will need first to set up your environment as
follows:

```sh
export RTI_LD_LIBRARY_PATH=$NDDSHOME/lib/<ARCH>
```

where `<ARCH>` shall be replaced with the target architecture you used to build
the example in the previous step.

1.  Run the publisher application on domain 0. You should observe the following
    output:

    ```bash
    ./SensorAttributesCollectionPublisher 0

    Writing SensorAttributesCollection, count 0
    [sensor_array: {[id: 0, value: nan, is_active: 1], [id: 0, value: inf, is_active: 1],  [id: 0, value: inf, is_active: 1], [id: 0, value: inf, is_active: 1]}]
    Writing SensorAttributesCollection, count 1
    [sensor_array: {[id: 1, value: 0, is_active: 1
    ...
    ```

2.  Run the subscriber application on domain 0. You should observe that no data
    is received and output will look like this:

    ```bash
    ./SensorDataSubscriber 0

    SensorData subscriber sleeping for 4 sec...
    SensorData subscriber sleeping for 4 sec...
    SensorData subscriber sleeping for 4 sec...
    ...
    ```

3.  Now run *RTI Routing Service* to provide communication from the publisher
    application to the subscriber application. Run the following command from
    the example build directory:

    On *Windows*:

    ```sh
    %NDDSHOME%\bin\rtiroutingservice ^
           -cfgFile ..\RsStructArrayTransf.xml ^
           -cfgName RsStructArrayTransf
    ```

    On *Linux*:

    ```sh
    $NDDSHOME/bin/rtiroutingservice \
           -cfgFile ../RsStructArrayTransf.xml \
           -cfgName RsStructArrayTransf
    ```

    You should see how the subscriber application now receives samples with the
    following output:

    ```plaintext
    SensorData subscriber sleeping for 4 sec...
    [id: {1, 1, 1, 1}, value: {0, 1, 2, 3}, is_active: {1, 1, 1, 1}]
    SensorData subscriber sleeping for 4 sec...
    [id: {2, 2, 2, 2}, value: {0, 0.5, 1, 1.5}, is_active: {1, 1, 1, 1}]
    ...
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
