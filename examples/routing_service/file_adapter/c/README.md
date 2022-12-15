# Example Code: Routing Service File Adapter

## Example Description

This example shows how to create a simple adapter using the RTI Routing Service
Adapter SDK in C. The adapter scans the file system for one specific folder
defined in the XML configuration file. For every file present in that folder,
it creates a stream to communicate with the output. The data that will flow in
this stream are the lines inside the files. In summary, we have a stream for
every file in the specified folder, and every stream transports on the other
side the text lines contained inside the specific file. In the output, the
adapter will take care of creating a file for every stream received. The files
will be created on a specific path, which you can also configure in the XML
configuration file.

## Building the C Example

In order to build this example, you need to provide the following variables to
`CMake`:

-   `CMAKE_BUILD_TYPE`: specifies the build mode. Valid values are Release and
  Debug
-   `BUILD_SHARED_LIBS`: whether to build the library as a shared library or not.
  This should be set to ON when loading the plugin from the Routing Service
  executable. However, Routing Service supports loading of static libraries when
  using the service as a library. See note below.

```bash
mkdir build
cmake -DBUILD_SHARED_LIBS=ON|OFF -DCMAKE_BUILD_TYPE=Debug|Release ..
cmake --build .
```

### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform. If you installed Connext DDS in a custom location, you
can use the `CONNEXTDDS_DIR` variable to indicate the path to your RTI Connext
DDS installation folder. For example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ...
```

Also, If you installed libraries for multiple target architecture on your system
(i.e., you installed more than one target rtipkg), you can use the
`CONNEXTDDS_ARCH` CMake variable to indicate the architecture of the specific
libraries you want to link against. For example:

```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ...
```

**Note:** Since this example uses the `pthread` library, it only works on
UNIX-like systems. It has been successfully tested on Ubuntu and macOS.

**Note:** The `BUILD_SHARED_LIBS` allows you to control if the generated library
for this example is a static or a dynamic shared library. The following sections
assume you are building a dynamic shared library. However Routing Service also
supports static linking of adapters. To use this functionality you would need to
create an application that uses Routing Service as a library component and
statically links to this `FileAdapter` library.

## Running the C Example

Before running the example, take a look at file_bridge.xml. It defines the
different settings to load and configure the adapter. Among other things it
also includes two environment variables that allow you to easily specify the
input and output directories, namely:

- `INPUT_DIRECTORY`
- `OUTPUT_DIRECTORY`

They indicate the path to the input directory that will be scanned to check for
files, as well as the path to the output directory where all the discovered
files will be copied over, respectively.

**Note:** The following instructions assume you are using `bash` shell on a
UNIX-like system. For other shells like `tcsh` adapting shouldn't be very
difficult.

Here is how you would set the environment variables:

```bash
export INPUT_DIRECTORY=<Input Directory>
export OUTPUT_DIRECTORY=<Output Directory>
```

We also advise setting path to your installation of RTI Connext DDS in the
variable `NDDSHOME` as shown below:

```bash
export NDDSHOME=<RTI Connext DDS Directory>
```

Before running the RTI Routing Service, you also need to specify where the
`fileadapter` library is located as shown below:

```bash
export RTI_LD_LIBRARY_PATH=<Path to CMake build folder>
```

Now we can run the RTI Routing Service to copy over the files from
`INPUT_DIRECTORY` to `OUTPUT_DIRECTORY`:

```bash
# From within the /build folder
<NDDSHOME>/bin/rtiroutingservice -cfgFile file_bridge.xml -cfgName file_to_file
```

**Note:** If you wish to run the executable for a target architecture different
than the host, you have to specify your architecture. That way the Routing
Service script can use the specific target binary instead of using the standard
host binary. This can be done by using the `CONNEXTDDS_ARCH` variable.

```bash
export CONNEXTDDS_ARCH=<ARCH>
<NDDSHOME>/bin/rtiroutingservice -cfgFile file_bridge.xml -cfgName file_to_file
```
