# C++11 Routing Service Adapter Debug Executable - DDS <--> MSQL Server

This windows executable instantiates a routing service which routes ShapeType
DDS data to a Microsoft MSQL Database. It also reads records from the MSQL
database and publishes them as DDS data on domain 0. It allows you to debug
the routing service adapter code which can later be built into a DLL.

## Environment

The example code should build and run with the following environment.

- Windows 10 (x64)
- Connext DDS Professional 6.0.1 with target x64Win64VS2017
- Microsoft Visual Studio 2017 or 2019
- Microsoft SQL Server Express 2017
- Microsoft SQL Server Management Studio (SSMS)

There are two xml files in the rs_dds_msql_adapter\rs_main directory.
- MainRsMsqlShapesAdapter_w_types uses defined XML types to get the type.
- MainRsMsqlShapesAdapter_wo_types uses discovery to get the type.

## Logging

To turn off logging edit CMakeLists.txt and comment out the following.

```c++
target_compile_definitions(rsmain PUBLIC RTI_RS_USE_LOG) # if defined use logging
```

## Building Debug executable:

1. Create rs_dds_msql_adapter\rs_main\build directory. Everything in this
    directory is ignored by git.

2. From rs_dds_msql_adapter\rs_main\build type the following.
    Enter **-G "Visual Studio 16 2019"** for Visual Studio 2019.

`cmake -DBUILD_SHARED_LIBS=ON -DCONNEXTDDS_ARCH=x64Win64VS2017 -DCONNEXTDDS_IMPORTED_TARGETS_DEBUG=ON -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 15 2017" -A x64 ..`

3. The project can be built from Visual Studio or from the command line.
    - In Visual Studio open RSMain.sln and select **Debug** build
        configuration and build.
    - Or type the following command.

`cmake --build . --config Debug`

## Read DDS data and write to a MSQL database:

1. Publish any shapes on Domain 0

2. From rs_dds_msql_adapter\rs_main\build run one of the commands below.

* To use the XML types defined in the XML file:

`rsmain.exe ../MainRsMsqlShapesAdapter_w_types.xml RS_DDS_To_MSQL`

* To use DDS discovery to discover the types:

`rsmain.exe ../MainRsMsqlShapesAdapter_wo_types.xml RS_DDS_To_MSQL`

## Read from MSQL database and publish to DDS domain:

This example requires data in the database. It will read records from the
database and publish them to DDS domain 0.

1. Subscribe to all shapes on Domain 0

2. From rs_dds_msql_adapter\rs_main\build run one of the commands below.

* To use the XML types defined in the XML file:

`rsmain.exe ../MainRsMsqlShapesAdapter_w_types.xml RS_MSQL_To_DDS`

* To use DDS discovery to discover the types:

`rsmain.exe ../MainRsMsqlShapesAdapter_wo_types.xml RS_MSQL_To_DDS`

## Debugging the Examples:

In Visual Studio add the command line parameters above in
**rsmain Project Configuration Properties --> Debugging --> Command Arguments**

This will allow you to run the examples from Visual Studio, set breakpoints
and debug your adapter.

**Note 1:** In order to debug your example with Visual Studio you will have to
make a small change to a file in order to work around a known issue with 
Connext DDS Professional 6.0.0 and 6.0.1. 
Open **%NDDSHOME%\include\rti\routing\adapter\detail\StreamReaderForwarder.hpp**
and around line 428 change the following:

```c++
holder->sample_seq_[holder->sample_seq_.size()] = holder; // comment out
holder->sample_seq_.data()[holder->sample_seq_.size()] = holder; // add this
```