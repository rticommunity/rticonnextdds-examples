# Example Code: Recording Service As a Library

## Concept

*Recording Service* can be run as a library within your executable code. This
example shows how to do that in a very simple way in C++11.

## Example Description

This example shows how to run *Recording Service* as a library. The code first
parses the command-line arguments provided by the user: the role to be used (it
can be set as a Recorder or a Replay instance) and the DDS domain ID to be used.
This domain ID will be used for user-data (it will be the domain ID base) and
also as the monitoring/administration domain IDs. There is an optional third
argument that provides the time in seconds the application should run.

The configuration file to be used by the service depends on the role provided.
The configuration name is the same in both configurations.

The following files are part of the example:

-   `ServiceAsLibExample.cxx`: this file contains the example C++ code that
    processes the command-line arguments and then creates the *Recording
    Service* instance.

-   `recorder_config.xml`: a simple configuration file for the recorder role.

-   `replay_config.xml`: a simple configuration file for the replay role. To
    work properly, it expects data to have been recorded with the recorder
    configuration first.

> **Note**:
>
> The application expects the `recorder_config.xml` or `replay_config.xml` file
> to be located in the current working directory when launching. The build
> system will copy these files to the binary directory but if you're going to
> run the example from a different location, make sure you copy the XML files to
> this location too.

## Building the Example

In order to build this example, you need to provide the following variables to
`CMake`:

-   `CONNEXTDDS_DIR`: pointing to the installation of RTI Connext DDS to be
    used.

-   `CONNEXTDDS_ARCH`: the RTI Connext DDS Target architecture to be used in
    your system.

-   `CMAKE_BUILD_TYPE`: specifies the build mode. Valid values are Release
    and Debug.

-   `BUILD_SHARED_LIBS`: specifies the link mode. Valid values are ON for
    dynamic linking and OFF for static linking.

Build the example code by running the following command:

```sh
mkdir build
cd build
cmake -DCONNEXTDDS_DIR=<connext dir> -DCONNEXTDDS_ARCH=<connext architecture>
      -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ..
cmake --build .
```

> **Note**:
>
> When using a multi-configuration generator, make sure you specify
> the `--config` parameter in your call to `cmake --build .`. In general,
> it's a good practice to always provide it.

In case you are using Windows x64, you have to add the option -A in the cmake
command as follow:

```sh
cmake -DCONNEXTDDS_DIR=<connext dir> -DCONNEXTDDS_ARCH=<connext architecture>
      -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON .. -A x64
```

This will produce a binary directory (*build*) where the `ServiceAsLibExample`
application can be found. The XML files in the source directory are also copied
over to this binary directory so that *Recording Service* can be run directly
from this directory as well.

> **Note**:
>
> **Cross-compilation**. When you need to cross-compile the example, the above
> command will not work, the assigned compiler won't be the cross-compiler and
> errors may happen when linking against the cross-compiled Connext binaries. To
> fix this, you have to create a file with the architecture name and call CMake
> with a specific flag called `-DCMAKE_TOOLCHAIN_FILE`. An example of the file
> to create with the toolchain settings (e.g. for ARM architectures):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(toolchain_path "<path to>/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian")
set(CMAKE_C_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-g++")
```

Then you can call CMake like this:

```sh
cmake -DCONNEXTDDS_DIR=<connext dir>
      -DCMAKE_TOOLCHAIN_FILE=<toolchain file created above>
      -DCONNEXTDDS_ARCH=<connext architecture>
      -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ..
```

## Running the Example

When running the example, make sure that the Routing Service Library
(_librtiroutingservice.so_ on Linux, _librtiroutingservice.dylib_ on Mac,
_rtiroutingservice.dll_ on Windows) is accessible in the current dynamic library
loading path (e.g. `LD_LIBRARY_PATH` on Linux).

The following command-line parameters are expected by the application:

-   The service role, Recorder or Replay, one of the values: `{record|replay}`.

-   The DDS domain ID (integer) where to run the example. This domain ID will be
    used both as the domain ID offset for user-data domain participants in the
    configuration, and to set the administration and monitoring domain IDs.

-   (Optionally) The time in seconds the application should run. The default is
    60 seconds, if not specified.

To run the example in Recorder mode, do:

```sh
cd <binary directory>
./ServiceAsLibExample record <your domain ID>
```

> **Note**:
>
> You can run from other directory, just make sure the `recorder_config.xml`
> file is in the directory where you're running from.

To run the example in Replay mode, for 480 seconds, do:

```sh
cd <binary directory>
./ServiceAsLibExample replay <your domain ID> 480
```

> **Note**:
>
> You can run from other directory, just make sure the `replay_config.xml` file
> is in the directory where you're running from, as well as a directory called
> `cdr_recording`, where Recorder must have saved the data from the recording
> session (so it should contain the `metadata`, `discovery` and user-data
> files).

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.11 or higher
