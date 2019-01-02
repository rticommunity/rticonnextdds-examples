# Routing Service C++ Adapter Example

## Building the example

- `CONNEXTDDS_DIR`
- `CONNEXTDDS_ARCH`

```bash
mkdir build
cmake -DCONNEXTDDS_DIR=<connext dir> -DCONNEXTDDS_ARCH=<connext architecture> ..
cmake --build .
```

## Running the example

```bash
cd build
<connext dir>/bin/rtiroutingservice -cfgFile ../RsFileAdapter.xml -cfgName RSFileAdapter
```