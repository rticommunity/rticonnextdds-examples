# C++11 Routing Service Adapter DLL - DDS <--> MSQL Server

This windows example builds a routing service DLL adapter which routes
ShapeType DDS data to a Microsoft MSQL Database. It also reads records from the
MSQL database and publishes them as DDS data on domain 0.

## Environment

The example code should build and run with the following environment.

- Windows 10 (x64)
- Connext DDS Professional 6.0.1 with target x64Win64VS2017
- Microsoft Visual Studio 2017 or 2019
- Microsoft SQL Server Express 2017
- Microsoft SQL Server Management Studio (SSMS)

There are two xml files in the rs_dds_msql_adapter/dll directory.

- RsMsqlShapesAdapter_w_types uses the XML types to get the type.
- RsMsqlShapesAdapter_wo_types uses discovery to get the type.

## Logging

To turn off logging edit CMakeLists.txt and comment out the following.

```c++
target_compile_definitions(rsmain PUBLIC RTI_RS_USE_LOG) # if defined use logging
```

## Building Release DLL adapter:

**Note 1:** The release version of rtiroutingservice ships with Connext DDS
6.0.x and therefore only the Release mode DLL will work correctly.
To Debug the adapter code see the README.md in rs_main directory.

1. Create rs_dds_msql_adapter\rs_dll\build directory. Everything in this
    directory is ignored by git.

2. From rs_dds_msql_adapter\rs_dll\build type the following. Enter
    **-G "Visual Studio 16 2019"** for Visual Studio 2019.

```bash
cmake -DBUILD_SHARED_LIBS=ON
-DCONNEXTDDS_ARCH=x64Win64VS2017
-DCMAKE_BUILD_TYPE=Release
-G "Visual Studio 15 2017" -A x64 ..
```

3. The project can be build from Visual Studio or from the command line.

    - In Visual Studio open MsqlAdapter.sln, select **Release** build
        configuration and build.
    - Or type the following command.

```bash
cmake --build . --config Release
```

## Read DDS data and write to a MSQL database:

1. Publish any shapes on Domain 0

2. From rs_dds_msql_adapter\rs_dll\build run one of the commands below.

    - To run the routing service using the XML types defined in the XML file:

```bash
rtiroutingservice
-cfgFile ../RsMsqlShapesAdapter_w_types.xml
-cfgName RS_DDS_To_MSQL
```

    - To run the routing service using DDS discovery to discover the types:

```bash
rtiroutingservice
-cfgFile ../RsMsqlShapesAdapter_wo_types.xml
-cfgName RS_DDS_To_MSQL
```

## Read from MSQL database and publish to DDS domain:

This example requires data in the database. It will get records from the
database and publish them to DDS domain 0.

1. Subscribe to all shapes on Domain 0

2. From rs_dds_msql_adapter\rs_main\build run one of the commands below.

    - To use the XML types defined in the XML file:

```bashrtiroutingservice
-cfgFile ../RsMsqlShapesAdapter_w_types.xml
-cfgName RS_MSQL_To_DDS
```

    - To use DDS discovery to discover the types:

```bash
rtiroutingservice
-cfgFile ../RsMsqlShapesAdapter_wo_types.xml
-cfgName RS_MSQL_To_DDS
```
