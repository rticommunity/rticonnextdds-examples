# Example Code: Modern C++ Distributed Logger Application

## Description of the Requester Application

The following files are part of the example:

-   DistLoggerExample.cxx
-   distlogSupport.h (necessary header to use Modern C++ Distributed Logger
in RTI Connextdds 7.2.0)

Note: The `CMakeLists.txt` in this example uses the latest version
of `FindRTIConnextDDS.cmake` from the
[cmake-utils](https://github.com/rticommunity/rticonnextdds-cmake-utils)
repository to find the Modern C++ Distributed Logger libraries.
If you are using RTI Connext 7.2.0, you may want to use a newer version from
that repository as well.

## Building the Example

In order to build this example, you need to define the variable `CONNEXTDDS_DIR`
You can do so by exporting it manually, by sourcing the `rtisetenv` script for
your architecture, or by passing it to the `cmake` command as arguments:

```bash
mkdir build
cd build
cmake -DCONNEXTDDS_DIR=<Connext DDS Directory> \     # If not exported
      -DBUILD_SHARED_LIBS=ON|OFF \
      -DCMAKE_BUILD_TYPE=Debug|Release ..
cmake --build .
```

> **Note**:
>
> You do not need to define `CONNEXTDDS_ARCH` if you only have one architecture
> target installed in your system.
>
> **Note**:
>
> When using a multi-configuration generator, make sure you specify
> the `--config` parameter in your call to `cmake --build .`. In general,
> it's a good practice to always provide it.

This will produce a binary directory (*build*) where the application
can be found.

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

## Running the Example

### Example Application

Run the application in a terminal as follows:

On *Windows* systems run:

```sh
DistLoggerExample.exe [options]
```

On *UNIX* systems run:

```sh
./DistLoggerExample [options]
```

where the options are:

-   `-d, --domain`: Domain ID.

    **Default**: 0.

-   `-s, --sleep`: Number of seconds to sleep between iterations.

    **Default**: 1.

-   `-i, --iterations`: Number of logging iterations.

    **Default**: 50.

-   `-h, --help`: Displays application usage and exits.

You should see the messages that are being logged on each iteration printed
on the terminal.

### Visualizing the log messages

Once the example application is running, open RTI Spy or
RTI Admin Console.
You should be able to visualize the logging messages being sent
by the application.

To learn more about RTI Tools, refer to their section in the
[Connext DDS documentation](https://community.rti.com/documentation).

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
