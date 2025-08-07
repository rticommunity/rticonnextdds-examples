# Example Code: Routing Service Monitoring

## Building the Example :wrench:

In order to build this example, you need to define the variables `CONNEXTDDS_DIR`
and `CONNEXTDDS_ARCH`. You can do so by exporting them manually, by sourcing
the `rtisetenv` script for your architecture, or by passing them to the `cmake`
command as arguments:

```bash
mkdir build
cd build
cmake -DCONNEXTDDS_DIR=<Connext DDS Directory> \     # If not exported
      -DCONNEXTDDS_ARCH=<Connext DDS Architecture> \ # If not exported
      -DBUILD_SHARED_LIBS=ON|OFF \
      -DCMAKE_BUILD_TYPE=Debug|Release ..
cmake --build .
```

**Note**: You do not need to define `CONNEXTDDS_ARCH` if you only have one
architecture target installed in your system.

Instead of using `cmake --build`, you can directly use the generated
infrastructure (e.g., Makefiles or Visual Studio Solutions) to build the example.
If you generated Makefiles in the configuration process, run make to build the
example. Likewise, if you generated a Visual Studio solution, open the solution
and follow the regular build process.

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

## Running the Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

On *Windows* systems run:

```sh
RoutingServiceMonitoring_subscriber.exe <domain_id> <sample_count>
```

On *UNIX* systems run:

```sh
./RoutingServiceMonitoring_subscriber <domain_id> <sample_count>
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.
2.  The `<sample_count>`: number of samples to be received. The default is infinite.

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
cmake -DCONNEXTDDS_ARCH=<Connext DDS Architecture>..
```

### CMake Build Infrastructure

The `CMakeLists.txt` script that builds this example uses two generic CMake
functions called `connextdds_rtiddsgen_run` and `connextdds_add_application`
that defines all the necessary constructs to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Subscriber application.

3.  Copy the `USER_QOS_PROFILES.xml` file into the directory where the subscriber
executable is generated.

You will find the definition of `connextdds_add_application`, along with detailed
documentation, in
[resources/cmake/rticonnextdds-cmake-utils/cmake/Modules/ConnextDdsAddExample.cmake
](https://github.com/rticommunity/rticonnextdds-cmake-utils/blob/main/cmake/Modules/ConnextDdsAddExample.cmake).

You will find the definition of `connextdds_rtiddsgen_run`, along with detailed
documentation, in
[resources/cmake/ConnextDdsCodegen.cmake](../../../../resources/cmake/ConnextDdsCodegen.cmake).

For a more comprehensive example on how to build an RTI Connext DDS application
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
