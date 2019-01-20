# ConnextDDS C example with CMake

This example show how to create a "``hello world``" application written in C
and how to build it using [CMake](https://cmake.org/).

The CMakeList.txt file is self documented to allow you to understand how
the publisher and subscriber build is performed.

## Build the example :wrench:

To build this example, first run CMake to generate the corresponding
build files. We recommend you use a separate directory to store all the
generated files *(e.g., ./build)*.

```bash
mkdir build
cd build
cmake ..
```

Once you have run CMake, you will find a number of new files in your build
directory (the list of generated files will depend on the specific CMake
Generator). To build the example, run CMake as follows:

```bash
cmake --build .
```

**Note**: if you are using a multi-configuration generator, such as Visual
Studio solutions, you can specify the configuration mode to build as follows:

```bash
cmake --build . --configuration Release|Debug
```

Alternatively, you can use directly the generated infrastructure (e.g.,
Makefiles or Visual Studio Solutions) to build the example. If you generated
Makefiles in the configuration process, run make to build the example.
Likewise, if you generated a Visual Studio solution, open the solution and
follow the regular build process.

## Run the Example
After building the example, two applications will be
generated in the binary directory, to run them, you can run the following
commands:

```bash
HelloWorld_publisher <domain_id> <number_samples>
HelloWorld_subscriber <domain_id> <number_samples>
```

### Customizing the Build
#### Configuring Build Type and Generator
By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows). You can use the following CMake variables to modify the
default behavior:
- ``-DCMAKE_BUILD_TYPE`` -- specifies the build mode. Valid values are Release
and Debug. See the [CMake documentation for more details. (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html).
- ``-DBUILD_SHARED_LIBS`` -- specifies the link mode. Valid values are ``ON``
for dynamic linking and ``OFF`` for static linking.
[See CMake documentation for more details.
(Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html).
- ``-G`` -- CMake generator. The generator is the native build system to use
build the source code. [All the valid values are described described in the
CMake documentation CMake Generators Section](https://cmake.org/cmake/help/v3.13/manual/cmake-generators.7.html).

For example, to build a example in Debug/Static mode run CMake as follows:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON ..
```

#### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform.If you installed Connext DDS in a custom location, you
can use the CONNEXTDDS_DIR variable to indicate the path to your RTI Connext
DDS installation folder. For example:

```bash
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, If you installed libraries for multiple target architecture on your
system (i.e., you installed more than one target rtipkg), you can use the
CONNEXTDDS_ARCH variable to indicate the architecture of the specific libraries
you want to link against. For example:

```bash
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```
