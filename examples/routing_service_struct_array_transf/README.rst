Example Code: Routing Service Transformation
********************************************

.. |RS| replace:: *RoutingService*
.. |TRANSF| replace:: *Transformation*

Concept
=======

|RS| |TRANSF| is a pluggable-component that allows performing modifications
on the input samples received in *Route* and provide such modifications as
output samples.

Refer to the
`SDK documentation <https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp/group__RTI__RoutingServiceTransformationModule.html>`_
to learn more about how to implement and use custom |TRANSF| plug-ins.

Example Description
===================

This example shows how to implement a custom |TRANSF| plug-in, build it
into a shared library and load it with |RS|.

The example shows the use case of converting data sample that contains
an **array of structs** into individual **arrays of primitives**. The modeled
use case assumes that the information of a sensor in plant is represented
by the type ``SensorAttributes``, which contains just primitive members. Then
a monitoring collection application publishes the information of all the sensors
represented in the form of an array of sensor attributes, contained in the the
type ``SensorAttributesCollection``. However due to some limitations of the
type system in subscriber applications, the information needs to be converted
so that instead of providing the array of structs, it provides a single struct
that contains individual arrays for each primitive attribute. This type is
represented with ``SensorData``.

To perform the communication between the publisher and subscriber applications,
a |RS| instance is placed to perform the conversion between the two different
types. The |RS| is configured with a single *Route* to reads data from
the input topic with type ``SensorAttributesCollection``, performs the conversion
with a custom |TRANSF| and writes the converted data to the output topic with
type ``SensorData``.

In this example you will find files for the following elements:

- ``SensorAttributesCollectionPublisher``: The monitoring collection application
  that publishes the information described above.
- ``SensorDataSubscriber``: The subscriber application that receives the data
  converted as described above
- ``StructArrayTransformation``: the custom |TRANSF| plug-in, generated as a 
  shared library, that performs the data conversion.
- Configuration for the |RS| that loads the custom |TRANSF| and provides
  the communication between publisher and subscriber applications.
- IDL definition of all the required types.


Requirements
============

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher
- A target platform supported by *RTI* |RS|.