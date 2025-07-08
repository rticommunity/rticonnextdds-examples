# Example Code: Routing Service C++11 Socket Adapter

## Example Description

This example shows how to implement a simple Routing Service Adapter plugin
in C++11 to receive data from a UDP socket using RTI Routing Service.

This example requires including a Types.xml file with the data type information.

The code in this directory provides the following components:

-   `src/SocketAdapter` implements the plugin that is loaded by *RTI Routing
Service*. It is responsible to create and delete connections.
-   `src/SocketConnection` implements a connection. This component is
responsible of the creation and deletion of `StreamReaders`.
-   `src/SocketInputDiscoveryStreamReader` implements the logic necessary to
propagate information about the discovered input streams (in this case
sockets) to the Routing Service.
-   `src/SocketStreamReader` implements an `StreamReader` that reads sample
information from a UDP socket.
-   `src/SocketStreamWriter` implements an `StreamWriter` that sends sample
information to a UDP socket.
-   `test/send_shape_to_socket.py` implements a simple tester to send shape
type data to a UDP socket.
-   `test/receive_shape_from_socket.py` implements a simple tester to receive shape
type data from a UDP socket.

For more details, please refer to the *RTI Routing Service SDK* documentation.

## Building C++ example

In order to build this example, you need to define the variables
`CONNEXTDDS_DIR` and `CONNEXTDDS_ARCH`. You can do so by exporting them
manually, by sourcing the `rtisetenv` script for your architecture, or by
passing them to the `cmake` command as arguments:

```bash
mkdir build
cd build
cmake -DCONNEXTDDS_DIR=<Connext DDS Directory> \     # If not exported
      -DCONNEXTDDS_ARCH=<Connext DDS Architecture> \ # If not exported
      -DBUILD_SHARED_LIBS=ON|OFF \                   # ON is preferred
      -DCMAKE_BUILD_TYPE=Debug|Release ..
cmake --build .
cd ..
```

Example command for Windows:

```bash
cmake .. -DCONNEXTDDS_DIR="%NDDSHOME%" -DCONNEXTDDS_ARCH=x64Win64VS2015 -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -A x64 -G "Visual Studio 17 2022"
cd ..
```

**Note**: You do not need to define `CONNEXTDDS_ARCH` if you only have one
architecture target installed in your system.

**Note**: When compiling on a Windows 64-bit machine you will need to add the
`-A x64` parameter to the call to CMake.

**Note:** If you are using a multi-configuration generator, such as Visual
Studio Solutions, you can specify the configuration mode to build as follows:

```bash
cmake --build . --config Release|Debug
```

Here is more information about generating
[Visual Studio Solutions for Windows using CMake](https://cmake.org/cmake/help/v3.16/generator/Visual%20Studio%2016%202019.html#platform-selection).

**Note:** `BUILD_SHARED_LIBS` allows you to control if the generated library
for this example is a static or a dynamic shared library. The following
sections assume you are building a dynamic shared library. However, Routing
Service also supports static linking of adapters. To use this functionality
you would need to create an application that uses Routing Service as a library
component and statically links to this `SocketAdapter` library.

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

## Running C++ example

To run the example, you just need to run the following commands from the top
level folder. This example has been written to allow easy experimentation with
the Shapes Demo shipped with *RTI Connext DDS* installer bundle. You will find
some hardcoded references to ShapeType and Square. If you wish to create a
real Routing Service adapter, you should modify the code and XML accordingly.

There are 2 configurations (`-cfgName`) in the Routing Service XML file:

-   **SocketAdapterToDDS** - It reads data from a UDP socket using the
SocketAdapter and outputs it to DDS. You can visualize the ouptut by
subscribing to Squares in Shapes Demo or running:

```bash
 $NDDSHOME/bin/rtiddsspy -printSample
```

-   **DDSToSocketAdapter** - It sends data from DDS to a UDP socket. 

To run Routing Service, you will need first to set up your environment as
follows.

Before running the RTI Routing Service, you need to specify where the
`SocketAdapterCpp` library is located as shown below:

Linux:

```bash
$export RTI_LD_LIBRARY_PATH=$NDDSHOME/lib/<Connext DDS Architecture>:<Path to SocketAdapterCpp library, the build/ folder>
```

Windows:

```bash
set PATH=%NDDSHOME%/lib/<Connext DDS Architecture>;<Path to SocketAdapterCpp library, the build/Release folder>
```

The SocketAdapterCpp library will be in the `./build` folder.

```bash
# From the build/ directory
$NDDSHOME/bin/rtiroutingservice -cfgFile RsSocketAdapter.xml -cfgName SocketAdapterToDDS
```

Here is an output from a sample run:

```bash
$export RTI_LD_LIBRARY_PATH=~/$NDDSHOME/lib/$CONNEXT_ARCH:~/udp_socket_adapter_typed/build/

$ $NDDSHOME/bin/rtiroutingservice -cfgFile RsSocketAdapter.xml -cfgName SocketAdapterToDDS
RTI Routing Service 7.3.0 executing (with name SocketAdapterToSocketAdapter)
```

Now you'll need to send data to the UDP sockets. By default, Shapes are
expected on `127.0.0.1:10203`. You can change these default values on
`RsSocketAdapter.xml`.

To run the Shape tester that mimics a legacy UDP socket sender, run:

```bash
python3 test/send_shape_to_socket.py 127.0.0.1 10203
```

You can now open a Shapes Demo instance on domain 0 and subscribe to Squares.
You should start receiving a red Square.

Alternatively, you can also execute the test to send UDP sockets from DDS data:

```bash
$export RTI_LD_LIBRARY_PATH=~/$NDDSHOME/lib/$CONNEXT_ARCH:~/udp_socket_adapter_typed/build/

$ $NDDSHOME/bin/rtiroutingservice -cfgFile RsSocketAdapter.xml -cfgName DDSToSocketAdapter
RTI Routing Service 7.3.0 executing (with configuration=DDSToSocketAdapter)
```

And to test the UDP socket content run:
```bash
python3 test/read_shape_from_socket.py 10203
```


## Running a data-diode example

You can configure a data-diode scenario by using two Routing Services instances;
- One using **DDSToSocketAdapter** configuration to publish DDS data over a one direction UDP socket
- The other using **SocketAdapterToDDS** configuration to convert back to DDS samples
```                                                                                        
  ┌───────────┐  ┌─────────────┐                         ┌─────────────┐  ┌───────────┐ 
  │  Connext  │  │   Routing   │    ┌────────────────┐   │   Routing   │  │  Connext  │ 
  │    App    ├─►│   Service   ├───►│ UDP DATA DIODE ├──►│   Service   ├─►│    App    │ 
  │           │  │ DDS TO UDP  │    └────────────────┘   │ UDP TO DDS  │  │           │ 
  └───────────┘  └─────────────┘                         └─────────────┘  └───────────┘ 
```                                                                                        
To run this example in a local machine:
```bash
$export RTI_LD_LIBRARY_PATH=~/$NDDSHOME/lib/$CONNEXT_ARCH:~/udp_socket_adapter_typed/build/

$ $NDDSHOME/bin/rtiroutingservice -cfgFile RsSocketAdapter.xml -cfgName SocketAdapterToDDS
RTI Routing Service 7.3.0 executing (with configuration=SocketAdapterToDDS)
```
And in a different terminal:
```bash
$export RTI_LD_LIBRARY_PATH=~/$NDDSHOME/lib/$CONNEXT_ARCH:~/udp_socket_adapter_typed/build/

$ $NDDSHOME/bin/rtiroutingservice -cfgFile RsSocketAdapter.xml -cfgName DDSToSocketAdapter
RTI Routing Service 7.3.0 executing (with configuration=DDSToSocketAdapter)
```

Using the default configuration from RsSocketAdapter.xml, you need to publish Squares 
on domain id 0 and subscribe to Squares on domain id 1 using rtishapes demo:

```bash
$ $NDDSHOME/bin/rtishapesdemo -domainId 0
```

```bash
$ $NDDSHOME/bin/rtishapesdemo -domainId 1
```
Then start publishing and subscribing to the Square topic.
You should be able to see red squares in the subscriber application.
Keep in mind the shape color has been overwritten in the adapter for showcasing purposes.

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher.
- A target platform with support for RTI Routing Service and C++11.
- Python3.
