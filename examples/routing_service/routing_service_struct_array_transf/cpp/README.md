# Example Code: Routing Service Transformation

Below there are the instructions to build and run this example. All the commands
and syntax used assume a Unix-based system. If you run this example in a
different architecture, please adapt the commands accordingly.

## Building

To build the example you will need first to run CMake to generate the build
files. From the directory containing the example sources:

```sh
mkdir build
cd build
cmake .. -DCONNEXTDDS_DIR=$NDDSHOME\
         -DCONNEXTDDS_ARCH=<ARCH> \
         -DBUILD_SHARED_LIBS=ON \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

where:

-   the environment variable `$NDDSHOME` shall point to the installation
    directory of RTI Connext.

-   `<ARCH>` shall be replaced with the target architecture where you are
    running the example (e.g., x64Darwin15clang7.0).

On successful execution of CMake, you will find different new files in your
build directory, including a file with name `Makefile`. This file represents a
Unix makefile that contains the rules to build a shared library containing the
*Transformation* plug-in implementation.

Now run:

```sh
make
```

which upon success it will create in the build directory:

-   A shared library file that represents the *Transformation* plug-in

-   A publisher application  executable with name
    `SensorAttributesCollectionPublisher`.

-   A subscriber application executable with name `SensorDataSubscriber`

## Running

To run this example you will need an instance of the publisher application, and
instance of the subscriber application, and an instance of *RoutingService*.

To run *RoutingService*, you will need first to set up your environment as
follows:

```sh
export RTI_LD_LIBRARY_PATH=$NDDSHOME/lib/<ARCH>
```

where `<ARCH>` shall be replaced with the target architecture you used to build
the example in the previous step.

1.  Run the publisher application on domain 0. You should observe the following
    output:

    ```
    Writing SensorAttributesCollection, count 0
    [sensor_array: {[id: 0, value: nan, is_active: 1], [id: 0, value: inf, is_active: 1],  [id: 0, value: inf, is_active: 1], [id: 0, value: inf, is_active: 1]}]
    Writing SensorAttributesCollection, count 1
    [sensor_array: {[id: 1, value: 0, is_active: 1
    ...
    ```

2.  Run the subscriber application on domain0. You should observe that no data
    is received and output will look like this:

    ```
    SensorData subscriber sleeping for 4 sec...
    SensorData subscriber sleeping for 4 sec...
    SensorData subscriber sleeping for 4 sec...
    ...
    ```

3.  Now run *RoutingService* to provide communication from the publisher
    application to the subscriber application.  Run the following command from
    the example build directory:

    ```sh
    $NDDSHOME/bin/rtiroutingservice \
           -cfgFile ../RsStructArrayTransf.xml \
           -cfgName RsStructArrayTransf
    ```

    You should see how the subscriber application now receives samples with the
    follow:

    ```
    SensorData subscriber sleeping for 4 sec...
    [id: {1, 1, 1, 1}, value: {0, 1, 2, 3}, is_active: {1, 1, 1, 1}]
    SensorData subscriber sleeping for 4 sec...
    [id: {2, 2, 2, 2}, value: {0, 0.5, 1, 1.5}, is_active: {1, 1, 1, 1}]
    ...
    ```
