# Example Code: Custom Transport

The example FileTransport plugin is contained in the files: FileTransport.c and
FileTransport.h

Before compiling or running the example, make sure the environment variable
NDDSHOME is set to the directory where your version of RTI Connext is installed.

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

When custom_transport_publisher and custom_transport_subscriber are run they
read the USER_QOS_PROFILES.xml file located in the directory from which the
application is run. This file, a snippet of which appears above, instructs DDS
to load the transport library built from the FileTransport.c and FileTransport.h
and use it for all communications.

In order to run the applications you will need to do the following:

1.  Build the publisher and subscriber applications.

    On MacOSX you can do that with the command:

    ```sh
    make -f makefile_custom_transport_x64Darwin10gcc4.2.1.mk
    ```

2.  Build the transport plugins library.

    On MacOSX you can do that with the command:

    ```sh
    make -f makefile_custom_transport_plugins_x64Darwin10gcc4.2.1.mk
    ```

3.  Set the load library path to include both the Connext DDS libraries and the
    newly created library.

    On MacOSX you can do that with the command:

    ```sh
    export DYLD_LIBRARY_PATH=./objs/x64Darwin10gcc4.2.1:${NDDSHOME}/lib/x64Darwin10gcc4.2.1
    ```

    On Linux you do it with the command:

    ```sh
    export LD_LIBRARY_PATH=./objs/i86Linux2.6gcc4.1.1:${NDDSHOME}/lib/i86Linux2.6gcc4.1.1
    ```

4.  Note that the USER_QOS_PROFILES.xml configures the NDDS_DISCOVERY_PEERS
    using the participant discovery qos. The XML snippet:

    ```xml
    <discovery>
        <initial_peers>
            <element>FileTransport://3.3.3.3</element>
        </initial_peers>
    </discovery>
    ```

    Tells the participant to find its peers using the FileTransport (which is
    the name we assigned to this transport) at address 3.3.3.3 (which is the one
    we configured for the participant using the property
    "dds.transport.FILE.myPlugin.address"

    if we wanted to configure participants to have multiple addresses (e.g.
    3.3.3.3 and 2.2.2.2), then we should also add all these addresses to the
    list of discovery peers on all the participants:

    ```xml
    <discovery>
        <initial_peers>
            <element>FileTransport://3.3.3.3</element>
            <element>FileTransport://2.2.2.2</element>
        </initial_peers>
    </discovery>
    ```

5.  Run the publisher and subscribing applications

You should get output similar to this:

```plaintext
ON THE PUBLISHER:
./objs/x64Darwin10gcc4.2.1/custom_transport_publisher
NDDS_Transport_FILE_newI: pid: 22834 bound to address: "3.3.3.3"
NDDS_Transport_FILE_get_class_name_cEA: FifoTransport
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_open_file_for_port (port 7410): initialized file=0x100d6bf60, fileName: "/tmp/dds/FileTransport/3.3.3.3/7410"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7410
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_share_recvresource_rrEA: resource at port '7410' not reused for port 7411
NDDS_Transport_FILE_open_file_for_port (port 7411): initialized file=0x100d6bff8, fileName: "/tmp/dds/FileTransport/3.3.3.3/7411"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7411
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fb70 (file ="/tmp/dds/FileTransport/2.2.2.2/7412") created for port= 7412, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fc10 (file ="/tmp/dds/FileTransport/2.2.2.2/7410") created for port= 7410, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fd60 (file ="/tmp/dds/FileTransport/3.3.3.3/7412") created for port= 7412, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f5fe00 (file ="/tmp/dds/FileTransport/3.3.3.3/7410") created for port= 7410, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
Writing CustomTransport, count 0
Writing CustomTransport, count 1
Writing CustomTransport, count 2
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x101b17270 (file ="/tmp/dds/FileTransport/3.3.3.3/7413") created for port= 7413, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
Writing CustomTransport, count 3
Writing CustomTransport, count 4
Writing CustomTransport, count 5
Writing CustomTransport, count 6


ON THE SUBSCRIBER:
./objs/x64Darwin10gcc4.2.1/custom_transport_subscriber
NDDS_Transport_FILE_newI: pid: 22836 bound to address: "3.3.3.3"
NDDS_Transport_FILE_get_class_name_cEA: FifoTransport
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_open_file_for_port: file '/tmp/dds/FileTransport/3.3.3.3/7410' already exists
NDDS_Transport_FILE_create_recvresource_rrEA: failed to create receive resource for address= "3.3.3.3", port= 7410
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_open_file_for_port (port 7412): initialized file=0x100d6df60, fileName: "/tmp/dds/FileTransport/3.3.3.3/7412"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7412
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_share_recvresource_rrEA: resource at port '7412' not reused for port 7413
NDDS_Transport_FILE_open_file_for_port (port 7413): initialized file=0x100d6dff8, fileName: "/tmp/dds/FileTransport/3.3.3.3/7413"
NDDS_Transport_FILE_create_recvresource_rrEA: created receive resource for address= "3.3.3.3", port= 7413
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_get_receive_interfaces_cEA: num receive interfaces: 1
    interface[0] = "3.3.3.3" (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e380 (file ="/tmp/dds/FileTransport/2.2.2.2/7412") created for port= 7412, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e420 (file ="/tmp/dds/FileTransport/2.2.2.2/7410") created for port= 7410, address= 2.2.2.2 (0000:0000:0000:0000:0000:0000:0202:0202)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e570 (file ="/tmp/dds/FileTransport/3.3.3.3/7412") created for port= 7412, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x100f4e610 (file ="/tmp/dds/FileTransport/3.3.3.3/7410") created for port= 7410, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)
CustomTransport subscriber sleeping for 4 sec...
NDDS_Transport_FILE_create_sendresource_srEA: Send resource 0x1019468f0 (file ="/tmp/dds/FileTransport/3.3.3.3/7411") created for port= 7411, address= 3.3.3.3 (0000:0000:0000:0000:0000:0000:0303:0303)

   str_value: "Iteration: 3"
   long_value: 3
CustomTransport subscriber sleeping for 4 sec...

   str_value: "Iteration: 4"
   long_value: 4
CustomTransport subscriber sleeping for 4 sec...

   str_value: "Iteration: 5"
   long_value: 5
CustomTransport subscriber sleeping for 4 sec...

   str_value: "Iteration: 6"
   long_value: 6
```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify the
default behavior:

-   `-DCMAKE_BUILD_TYPE` -- specifies the build mode. Valid values are Release
    and Debug. See the [CMake documentation for more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

-   `-DBUILD_SHARED_LIBS` -- specifies the link mode. Valid values are ON for
    dynamic linking and OFF for static linking. See [CMake documentation for
    more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

-   `-G` -- CMake generator. The generator is the native build system used to
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

### CMake Build Infrastructure

The CMakeListst.txt script that builds this example uses a generic CMake
function called connextdds_add_example that defines all the necessary constructs
to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Copy the USER_QOS_PROFILES.xml file into the directory where the publisher
    and subscriber executables are generated.

You will find the definition of connextdds_add_example, along with detailed
documentation, in
[resources/cmake/rticonnextdds-cmake-utils/cmake/Modules/ConnextDdsAddExample.cmake
](https://github.com/rticommunity/rticonnextdds-cmake-utils/blob/main/cmake/Modules/ConnextDdsAddExample.cmake).

For a more comprehensive example on how to build an RTI Connext DDS application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which includes
a comprehensive CMakeLists.txt script with all the steps and instructions
described in detail.
