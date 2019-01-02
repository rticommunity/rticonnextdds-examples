# Example Code :Routing Service C++ Adapter

## Example Description

This example shows how to implement a custom Adapter plug-in, build it
into a shared library and load it with RoutingService.

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

```bash
cd build
<connext dir>/bin/rtiroutingservice -cfgFile ../RsFileAdapter.xml -cfgName RSFileAdapter
```

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher
- A target platform supported by RTI RoutingService.