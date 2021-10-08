# Example Code: The Requester Application

## Concept

*Routing Service* uses the common Remote Administration Platform (see [Remote
Administration
Platform](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/services/routing_service/common/remote_admin_platform.html))

Here you have the Requester application.

## Description of the Requester Application

This example shows a one-shot application that can be used to send one remote
command to a running application of Routing Service (you can also try to use
it with Routing Service). The application will prepare a command request based
on the command-line parameters provided in the application invocation. After
sending that request out to the service, it will wait for a command reply to be
sent by the service. The application will then exit.

The following files are part of the example:

- `Requester.hpp` and `Requester.cxx`: these files contain the example C++
    code that prepares and sends the command request and then waits for a reply
    from the running service instance.

- `USER_QOS_PROFILES.xml`: this file defines DDS XML QoS profiles to be used
    by the application and by *Routing Service*.

## Building the Example

Build the example code by running the following command:

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

> **Note**:
>
> When using a multi-configuration generator, make sure you specify
> the `--config` parameter in your call to `cmake --build .`. In general,
> it's a good practice to always provide it.

This will produce a binary directory (*build*) where the `Requester` application
can be found. The XML files in the source directory are also copied over to this
binary directory so that *Routing Service* can be run directly from this
directory as well.

> **Note:**
>
> When you need to cross-compile the example, the above
> command will not work, the assigned compiler won't be the cross-compiler and
> errors may happen when linking against the cross-compiled Connext binaries. To
> fix this, you have to create a file with the architecture name and call CMake
> with a specific flag called `-DCMAKE_TOOLCHAIN_FILE`. An example of the file to
> create with the toolchain settings (e.g. for an ARM architectures):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(toolchain_path "<path to>/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian")
set(CMAKE_C_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-g++")
```

Then you can call CMake like this:

```sh
cmake -DCONNEXTDDS_DIR=<connext dir> \
      -DCMAKE_TOOLCHAIN_FILE=<toolchain file created above> \
      -DCONNEXTDDS_ARCH=<connext architecture> ..
```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify the
default behavior:

- `-DCMAKE_BUILD_TYPE` -- specifies the build mode. Valid values are Release
    and Debug. See the [CMake documentation for more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

- `-DBUILD_SHARED_LIBS` -- specifies the link mode. Valid values are ON for
    dynamic linking and OFF for static linking. See [CMake documentation for
    more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

- `-G` -- CMake generator. The generator is the native build system to use
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

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.1.0 or higher.
- CMake version 3.11 or higher
