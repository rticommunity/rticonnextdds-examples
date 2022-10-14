# Example Code: Routing Service C++11 Adapter

## Example Description

This example shows how to implement a custom `Routing Service Adapter plug-in`,
build it into a shared library and load it with Routing Service.

In this concrete example, we show how to write a simple adapter in C++11 to
import/export data from/to files using RTI Routing Service.

The code in this directory provides the following components:

-   `FileAdapter` implements the plugin that is loaded by *RTI Routing Service*.
It is responsible to create and delete connections.
-   `FileConnection` implements a connection. This component is responsible of the
creation and deletion of `StreamReaders` and `StreamWriters`.
-   `FileInputDiscoveryStreamReader` implements the logic necessary to propagate
information about the discovered input streams (in this case files) to the
Routing Service. We do not have a `FileOutputDiscoveryStreamReader` since we
directly write to the output file specified. However, it can be implemented in a
very similar vein to the `FileInputDiscoveryStreamReader`.
-   `FileStreamReader` implements an `StreamReader` that reads sample information
from a CSV file.
-   `FileStreamWriter` implements an `StreamWriter` that writes sample information
to a file in CSV format.

For more details, please refer to the *RTI Routing Service SDK* documentation.

## Building C++ example

In order to build this example, you need to provide the following variables to
`CMake`:

- `CONNEXTDDS_DIR`
- `CONNEXTDDS_ARCH`

```bash
$mkdir build
$cmake -DCONNEXTDDS_DIR=<Connext DDS Directory>
    -DCONNEXTDDS_ARCH=<Connext DDS Architecture>
    -DBUILD_SHARED_LIBS=ON|OFF
    -DCMAKE_BUILD_TYPE=Debug|Release ..
cmake --build .
```

**Note**: when compiling on a Windows 64-bit machine you will need to add the
`-A x64` parameter to the call to CMake.

**Note:** If you are using a multi-configuration generator, such as Visual Studio
Solutions, you can specify the configuration mode to build as follows:

```bash
cmake --build . --config Release|Debug
```

Here is more information about generating
[Visual Studio Solutions for Windows using CMake](https://cmake.org/cmake/help/v3.16/generator/Visual%20Studio%2016%202019.html#platform-selection).

**Note:** The `BUILD_SHARED_LIBS` allows you to control if the generated library
for this example is a static or a dynamic shared library. The following sections
assume you are building a dynamic shared library. However Routing Service also
supports static linking of adapters. To use this functionality you would need to
create an application that uses Routing Service as a library component and
statically links to this `FileAdapter` library.

## Running C++ example

To run the example, you just need to run the following command from the `build`
folder (where the adapter plugin library has been created).

This example has been written to allow easy experimentation with the Shapes Demo
shipped with *RTI Connext DDS* installer bundle. We use the environment variable
```SHAPE_TOPIC``` in the ```RsFileAdapter.xml``` configuration file.
```SHAPE_TOPIC``` should be the type of shape you are publishing or subscribing to.

Possible values for ```SHAPE_TOPIC``` are:

- Square
- Circle
- Triangle

You have 3 different configurations (cfgName) to choose from:

-   **FileAdapterToDDS** - This reads data from a file using the FileAdapter and
outputs it to DDS. You can visualize the ouptut by subscribing to the chosen
```SHAPE_TOPIC``` in Shapes Demo.
-   **DDSToFileAdapter** - This writes data to a file using the FileAdapter by
reading the input from DDS. You can publish data in Shapes Demo for a shape and
set that value to ```SHAPE_TOPIC``` before starting Routing Service.
-   **FileAdapterToFileAdapter** - This reads data from a file and writes data to
another file both using the FileAdapter plug-in. As before you should set the
appropriate value of ```SHAPE_TOPIC``` before starting Routing Service.

To run Routing Service, you will need first to set up your environment as follows.

Before running the RTI Routing Service, you need to specify where the
`fileadapter` library is located as shown below:

```bash
$export RTI_LD_LIBRARY_PATH=<Connext DDS Directory>/lib/<Connext DDS Architecture>:<Path to fileadapter library>
```

```bash
# From the build/ directory
$<Connext DDS Directory>/bin/rtiroutingservice
-cfgFile RsFileAdapter.xml
-cfgName <cfgName>
```

Here is an output from a sample run:

```bash
$export RTI_LD_LIBRARY_PATH=/Applications/rti_connext_dds-7.0.0/lib/x64Darwin16clang8.0

$export SHAPE_TOPIC="Triangle"

$/Applications/rti_connext_dds-7.0.0/bin/rtiroutingservice
-cfgFile RsFileAdapter.xml
-cfgName FileAdapterToFileAdapter

RTI Routing Service 7.0.0 executing (with name FileAdapterToFileAdapter)
Input file name: Input_Triangle.csv
Output file name: Output_Triangle.csv
Received Sample:
   color: "CYAN"
   x: 120
   y: 160
   shapesize: 30

Received Sample:
   color: "PURPLE"
   x: 202
   y: 28
   shapesize: 30

Received Sample:
   color: "ORANGE"
   x: 77
   y: 117
   shapesize: 30

Received Sample:
   color: "CYAN"
   x: 120
   y: 156
   shapesize: 30

Received Sample:
   color: "PURPLE"
   x: 200
   y: 24
   shapesize: 30

Received Sample:
   color: "ORANGE"
   x: 82
   y: 112
   shapesize: 30
```

**Note**: when running on Windows systems, remove the quotes from the value of
the ``SHAPE_TOPIC`` environment variable.

```bash
set SHAPE_TOPIC=Triangle

The general behavior of this example, can be controlled using properties and the
```SHAPE_TOPIC``` environment variable. You can modify properties in the Routing
Service configuration file. This is the list of available properties:

| Property                            | Tag        | Description                                                                                   |
| ----------------------------------- | ---------- | ----------------------------------------------------------------------------------------------|
| `example.adapter.input_file`        | `<input>`  | Path to a CSV file that contains the sample data. File must exist and contain valid CSV data. |
| `example.adapter.sample_period_sec` | `<input>`  | Periodic rate of reading samples from the file                                                |
| `example.adapter.output_file`       | `<output>` | Path to the file where to store the received samples                                          |

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher
- A target platform with support for RTI Routing Service and C++11.
