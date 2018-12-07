Example Code: Routing Service Processor
***************************************

.. |RS| replace:: *RS*
.. |PROCESSOR| replace:: *Processor*

Concept
=======

*Routing Service Processor* is a pluggable-component that allows controlling
the forwarding process that occurs within *Routes*. Refer to the
`SDK documentation <https://community.rti.com/static/documentation/connext-dds/current/doc/api/connext_dds/api_cpp/group__RTI__RoutingServiceProcessorModule.html>`_
to learn more about how to implement and use custom |PROCESSOR| plug-ins.

Example Description
===================

This example shows how to implement a custom |PROCESSOR| plug-in, build it
into a shared library and load it with |RS|.

This example illustrates the realization of two common enterprise patterns:
aggregation and splitting. There is a single plug-in implementation,
*ShapesProcessor* that is factory of two types of |PROCESSOR|\s, one for
each pattern implementation:

- *ShapesAggregator*: |PROCESSOR| implementation that performs the aggregation
  of two *ShapeType* objects into a single *ShapeType* object. 

- *ShapesSplitter*: |PROCESSOR| implementation that performs the separation
  of a single *ShapeType* object into two  *ShapeType* objects.

In the example, these processors are instantiated as part of a *TopicRoute*,
in which all its inputs and outputs represent instantiations of the
*Connext DDS Adapter StreamReader* and *StreamWriter*, respectively.

*ShapesAggregator*
------------------
The data processing is tied to a *leading input*, that represents
the input from which the processor reads *new* data first. For each instance
found in the data read from the leading input, the processor reads *any*
existing data for the equivalent instance.

The aggregation algorithm consists of simply an average of the values ``x``
and ``y`` of all inputs. The remaining elements are set equal to the values of
the data read from the leading input.

Note that for the proper behavior of this |PROCESSOR| it's required for the
DDS inputs to be configured with a history policy that preserves only the last
sample. This guarantees that memory doesn't grow unbounded since the processor
will never remove the samples from the StreamReader's cache.

This |PROCESSOR| implementation can receive the properties in table
`TableShapesAggregatorProperties`_.

.. list-table:: ShapesAggregator Configuration Properties
    :name: TableShapesAggregatorProperties
    :widths: 40 20 20
    :header-rows: 1

    * - Name
      - Value
      - Description
    * - **shapes_processor.kind**
      - ``aggregator``
      - Indicates the plug-in to instantiate a *ShapesAggregator*. If this
        property is not specified, it will create a *ShapesSplitter*.
    * - **shapes_processor.leading_input_index**
      - ``<integer>``
      - Indicates the index of the leading input. In this example, only 
        ``0`` and ``1`` are valid values.


*ShapesSplitter*
----------------

The splitting algorithm consists of assigning:

- Input value ``x`` to the first output, leaving ``y`` with value zero.
- Input value ``y`` to the second output, leaving ``x`` with value zero.
- Remaining values are set equally from the input values.

This |PROCESSOR| implementation does not receive any configuration properties.

Example Description
===================

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.10 or higher
- A target platform supported by *RTI* |RS|.