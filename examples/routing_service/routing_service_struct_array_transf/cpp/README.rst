Example Code: Routing Service Transformation
********************************************

.. |RS| replace:: *RoutingService*
.. |TRANSF| replace:: *Transformation*

Below there are the instructions to build and run this example. All the commands
and syntax used assume a Unix-based system. If you run this example in a different
architecture, please adapt the commands accordingly.

Building
========

To build the example you will need first to run CMake to generate the build files.
From the directory containing the example sources:

::

    mkdir build
    cd build
    cmake .. -DCONNEXTDDS_DIR=<Connext directory> \
             -DCONNEXTDDS_ARCH=<ARCH> \
             -DBUILD_SHARED_LIBS=ON


where:

- ``<Connext directory>`` shall be replaced with the path to the installation
  directory of *RTI Connext*.
- ``<ARCH>`` shall be replaced with the target architecture where you are
  running the example (e.g., x64Darwin15clang7.0).

.. note::

    Certain CMake generators may require specific value for the target platform
    through the ``-DCMAKE_GENERATOR_PLATFORM``. For example on Windows system,
    you can specify the values ``x86`` or ``x64``.

On successful execution of CMake, you will find different new files in your
build directory that will perform the build of the sources into a shared library.

In Unix, you will find a file with name ``Makefile``. This file represents
a Unix makefile that contains the rules to build a shared library containing
the |TRANSF| plug-in implementation.

To perform the build run:

::

    make


which upon success it will create in the build directory:

- A shared library file that represents the |TRANSF| plug-in
- A publisher application  executable with name 
  ``SensorAttributesCollectionPublisher``.
- A subscriber application executable with name ``SensorDataSubscriber``


Running
=======

To run this example you will need an instance of the publisher application,
and instance of the subscriber application, and an instance of |RS|.

To run |RS|, you will need first to set up your environment as follows:

    ::

        export RTI_LD_LIBRARY_PATH=<Connext directory>/lib/<ARCH>

where ``<ARCH>`` shall be replaced with the target architecture you used to
build the example in the previous step.


#. Run the publisher application on domain 0. You should observe the following
   output:

    ::

        Writing SensorAttributesCollection, count 0
        [sensor_array: {[id: 0, value: nan, is_active: 1], [id: 0, value: inf, is_active: 1], [id: 0, value: inf, is_active: 1], [id: 0, value: inf, is_active: 1]}]
        Writing SensorAttributesCollection, count 1
        [sensor_array: {[id: 1, value: 0, is_active: 1
        ...

#. Run the subscriber application on domain0. You should observe that no data
   is received and output will look like this:

    ::

        SensorData subscriber sleeping for 4 sec...
        SensorData subscriber sleeping for 4 sec...
        SensorData subscriber sleeping for 4 sec...
        ...
   

#. Now run |RS| to provide communication from the publisher application
   to the subscriber application.  Run the following command from the example
   build directory:

    ::

        <Connext directory>/bin/rtiroutingservice \
                -cfgFile ../RsStructArrayTransf.xml \
                -cfgName RsStructArrayTransf

  You should see how the subscriber application now receives samples with the
  follow:

    ::

        SensorData subscriber sleeping for 4 sec...
        [id: {1, 1, 1, 1}, value: {0, 1, 2, 3}, is_active: {1, 1, 1, 1}]
        SensorData subscriber sleeping for 4 sec...
        [id: {2, 2, 2, 2}, value: {0, 0.5, 1, 1.5}, is_active: {1, 1, 1, 1}]
        ...