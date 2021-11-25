# Example Code: Routing Service Processor

## Concept

*Routing Service Processor* is a pluggable-component that allows controlling the
forwarding process that occurs within *Routes*. Refer to the [SDK
documentation](https://community.rti.com/static/documentation/connext-dds/current/doc/api/routing_service/api_cpp/group__RTI__RoutingServiceProcessorModule.html)
to learn more about how to implement and use custom *Processor* plug-ins.

## Example Description

This example shows how to implement a custom *Processor* plug-in, build it into
a shared library and load it with *RoutingService*.

This example illustrates the realization of two common enterprise patterns:
aggregation and splitting. There is a two plug-in implementations, one for
each type of *Processor*:

-   *ShapesAggregator*: *Processor* implementation that performs the
    aggregation of two *ShapeType* objects into a single *ShapeType* object.

-   *ShapesSplitter*: *Processor* implementation that performs the separation of
    a single *ShapeType* object into two  *ShapeType* objects.

In the example, these processors are instantiated as part of a *TopicRoute*, in
which all its inputs and outputs represent instantiations of the *Connext DDS
Adapter StreamReader* and *StreamWriter*, respectively.

In this example you will find files for the following elements:

-   `ShapesAggregator`and ``ShapesSplitter``: the custom *Processor* plug-ins,
    generated as a shared library for each plugin.

-   Configurations for the *RoutingService* that loads the custom *Processor* and
    provides the communication between publisher and subscriber applications.

The *Processor* implementations in this example do not receive any configuration
properties, yet your own implementation can do so. You can specify configuration
properties using the ``<property`` tag under the configuration of your
plugin and processor, and receive those values as ``PropertySet`` upon object
creation (currently that parameter is commented out to avoid compilation
warnings).

### ShapesAggregator

This implementation shows very basic usage of the capabilities *Processor* where
inputs and outputs are accessed by their configuration names (which also match
the *Topic* names, and the aggregation pattern is very simple: it produces
`Triangle` samples whose position is obtained from `Square` samples, and the
size is obtained from the `y` coordinate obtained from `Circle` samples.

**Note**: samples of topics `Square` and `Circle` are routed without modification.

The aggregation logic relies on the reception samples from the `Square`input to
trigger data forwarding, and merge available data from the `Circle` input. Note
that for the purpose of this example, the names of the inputs and outputs are
hardcoded into the plug-in implementation. A recommended approach is to make
this value as arguments to the *Processor* creation, or use an algorithm
independent of these values, as shown in *ShapesAggregatorAdv*.

For the proper behavior of this *Processor* it's required for the DDS inputs
to be configured with a history policy that preserves only the last sample.
This guarantees that memory doesn't grow unbounded since the processor will
never remove the samples from the StreamReader's cache.

### ShapesSplitter

The splitting algorithm consists of assigning:

- Input value `x` to the first output, leaving `y` with value zero.
- Input value `y` to the second output, leaving `x` with value zero.
- Remaining values are set equally from the input values.

This *Processor* implementation does not receive any configuration properties.

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.0.0 or higher.
- CMake version 3.7 or higher
- A target platform supported by *RTI* *RoutingService*.
