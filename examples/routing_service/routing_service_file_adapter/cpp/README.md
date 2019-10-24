# Example Code :Routing Service C++ Adapter

## Example Description

This example shows how to implement a custom `Routing Service Adapter plug-in`,
build it into a shared library and load it with RoutingService.

In this concrete example, we show how a simple adapter to import/export data from/to files using RTI Routing Service.

The code in this directory provides the following components:

- `FileAdapter` implements the plugin that is loaded by *RTI Routing Service*. It is responsible to create and delete connections.
- `FileConnection` implements a connection. This component is responsible of the creation and deletion of `StreamReaders` and `StreamWriters`.
- `FileInputDiscoveryStreamReader` implements the logic necessary to propagate information about newly discovered streams to RTI Routing Service.
- `FileStreamReader` implements an `StreamReader` that reads sample information from a CSV file.
- `FileStreamWriter` implements an `StreamReader` that writes sample information to a file in JSON format.

For more details, please refer to the *RTI Routing Service SDK* documentation.

## Building C++ example

In order to build this example, you need to provide the following variables to
`CMake`:

- `CONNEXTDDS_DIR`
- `CONNEXTDDS_ARCH`

```bash
mkdir build
cmake -DCONNEXTDDS_DIR=<connext dir> -DCONNEXTDDS_ARCH=<connext architecture> ..
cmake --build .
```

## Running C++ example

To run the example, you just need to run the following command from the `build`
folder (where the adapter plugin library has been created).

```bash
cd build
<connext dir>/bin/rtiroutingservice -cfgFile ../RsFileAdapter.xml -cfgName RSFileAdapter
```
After running this command, you will see the following output:

```bash
RTI Routing Service 6.0.0 executing (with name RSFileAdapter)
FilePath: samples_out.json
Received Sample (JSON):
{
   "id":0,
   "message":" Hello World"
}

Received Sample (JSON):
{
   "id":1,
   "message":" Hello World"
}

```

The general behavior of this example, can be controlled using properties. You
can modify properties in the Routing Service configuration file. This is
the list of available properties:

| Property                            | Tag        | Description                                                                                    |
| ----------------------------------- | ---------- | ---------------------------------------------------------------------------------------------- |
| `example.adapter.input_file`        | `<input>`  | Path to a CSV file that contains the sample data. File must exists and contain valid csv data. |
| `example.adapter.sample_period_sec` | `<input>`  | Period to read samples from the file                                                           |
| `example.adapter.output_file`       | `<output>` | Path to the file where to store the received samples                                           |


## Requirements

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher
- A target platform supported by RTI RoutingService.