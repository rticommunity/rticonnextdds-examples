# Example Code: Access Union Discriminator In Dynamic Data

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

Run the following command from the example directory to execute the application.

On *UNIX* systems:

```sh
./dynamic_data_union_example
```

On *Windows* Systems:

```sh
dynamic_data_union_example
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

This example does not require the usage of rtiddsgen. Therefore, the
`CMakeLists.txt` script just creates the executable and link it with the Connext
API.

For a more comprehensive example on how to build an RTI Connext application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which includes
a comprehensive `CMakeLists.txt` script with all the steps and instructions
described in detail.
