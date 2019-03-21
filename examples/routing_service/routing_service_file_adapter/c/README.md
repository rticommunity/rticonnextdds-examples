# Example Code: Routing Service File Adapter

## Building C Example

Before compiling or running the example, make sure the environment variable
NDDSHOME is set to the directory where your version of RTI Connext DDS is
installed.

The example includes an example Makefile for the x64Linux2.6gcc4.4.5
architecture. You can modify the makefile to match the requirements of your
specific platform. You will find the flags to use for each specific platform
RTI Routing Service supports in your installation under the
"$ROUTINGHOME/adapters/file/make" directory. If you are using Visual Studio,
you will also find example Visual Studio solutions under
"$ROUTINGHOME/adapters/file/windows". You can modify them to point to this
example.

The example makefile copies the shared libraries that contain the adapter to
$(ROUTINGHOME)/bin/$(ARCH). That means you will probably not need to modify
your path to load the libraries. However, if you find problems loading the
libraries, you will need to add that directory to your Path, if you are using
Windows, or your LD_LIBRARY_PATH if you are using most of Unix-like systems.

## Running C Example

Before running the example, take a look at file_bridge.xml. It defines the
different settings to load and configure the adapter. You will need to specify
on it the path to the input directory that will be scanned to check for files,
as well as the path to the output directory where all the discovered files will
be copied over.

Once you have set the right paths in the XML configuration file, just run RTI
Routing Service like this:
```
./scripts/rtiroutingservice -cfgFile /path/to/file_bridge.xml -cfgName file_to_file
```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify
the default behavior:

* -DCMAKE_BUILD_TYPE -- specifies the build mode. Valid values are Release and
  Debug. See the [CMake documentation for more details.
  (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

* -DBUILD_SHARED_LIBS -- specifies the link mode. Valid values are ON for
  dynamic linking and OFF for static linking. See [CMake documentation for more
  details.
  (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

* -G -- CMake generator. The generator is the native build system to use build
  the source code. All the valid values are described described in the CMake
  documentation [CMake Generators
  Section.](https://cmake.org/cmake/help/v3.13/manual/cmake-generators.7.html)

For example, to build a example in Debug/Static mode run CMake as follows:
```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON ..
```

### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform.If you installed Connext DDS in a custom location, you
can use the CONNEXTDDS_DIR variable to indicate the path to your RTI Connext
DDS installation folder. For example:
```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, If you installed libraries for multiple target architecture on your
system (i.e., you installed more than one target rtipkg), you can use the
CONNEXTDDS_ARCH variable to indicate the architecture of the specific libraries
you want to link against. For example:
```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```

### CMake Build Infrastructure

The CMakeListst.txt script that builds this example uses a generic CMake
function called connextdds_add_example that defines all the necessary
constructs to:

1. Run RTI Code Generator to generate the serialization/deserialization code
   for the types defined in the IDL file associated with the example.

2. Build the corresponding Publisher and Subscriber applications.

3. Copy the USER_QOS_PROFILES.xml file into the directory where the publisher
   and subscriber executables are generated.

You will find the definition of connextdds_add_example, along with detailed
documentation, in
[rescources/cmake/ConnextDdsAddExample.cmake](../../../../rescources/cmake/ConnextDdsAddExample.cmake).

For a more comprehensive example on how to build an RTI Connext DDS application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which
includes a comprehensive CMakeLists.txt script with all the steps and
instructions described in detail.
