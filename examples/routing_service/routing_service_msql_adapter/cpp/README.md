# C++11 Routing Service Adapter - DDS <--> MSQL Database

This windows example uses Routing Service 6.0.0 to route ShapeType DDS data on
Domain 0 to a Microsoft MSQL Database. It also reads from the MSQL database
and publishes the records as DDS data on domain 0.

## Environment

The example code should build and run with the following environment.

- Windows 10 (x64)
- Connext DDS Professional 6.0.1 with target x64Win64VS2017
- Microsoft Visual Studio 2017 or 2019
- Microsoft SQL Server Express 2017
- Microsoft SQL Server Management Studio (SSMS)

## Requirements

This example requires a Microsoft SQL Server Database.

## Examples

There are two folders to build examples.

1.  rs_dll - This folder contains the artifacts to create a Release version of
    the msqladapter.dll.

2.  rs_main - This folder contains the artifacts to create a Debug version of
    an executable that can be used to debug the adapter code.


