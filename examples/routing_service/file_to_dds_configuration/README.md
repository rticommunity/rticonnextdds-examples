# Route/QoS Configuration Example - File to DDS Adapter

## Concept

Routing Service provides an adapter SDK in C, C++, and Java to support the
creation of new adapter plugins.

This flexible and complex file adapter is able to work with structured
information and supports both an input and output adapters. The input adapter
reads lines from a text file and the output adapter saves the provided lines to
an output text file.

The file adapter is provided under `<Routing Service home>/adapters/file` and
source code and projects can be found in
`<RoutingServicehome>/adapters/tutorial/C`.

## Example Description

This example shows how to configure a specific Routing Service Route that will
be using the input adapter to read lines from a text file and output it to a
Shapes Demo application.

The following files are necessary:

1.  [lib]fileadapter.{dll,so,dylib} - A built example of the file adapter.

2.  file_bridge.xml - The Routing Service Route configuration that includes the
    file adapter (We'll be using the file_to_dds route).

3.  MyShape.txt - An example text file that will be read by the input file
    adapter.

## Before Running the Example

1.  Open the file_bridge.xml file.

2.  Ensure that the `<adapter_library>` configuration points to the correct
    library name and create function.

3.  Ensure that path of the MyShape.txt file is well configured in the
    `<input_connection>` configuration.

## How to Run the Example

1.  Open the Shapes Demo application and subscribe to any shape with a history
     of 1.

2.  Open a command line window pointing to the Routing Service file adapter
    library. To do this, you will need to run build the file adapter example
    first:

    ```sh
    cd rti_workspace/<version>/examples/routing_service/adapters/file/make
    make -f Makefile.<your_platform>
    cp  rti_workspace/<version>/examples/routing_service/adapters/file/[lib]fileadapter.{dll,so,dylib} \
    /path/to/your/examples/rticonnextdds-examples/examples/file_to_dds_configuration
    ```

3.  Edit file_bridge.xml and modify the contents of `<dll>` to point to the
    library you just copied.

4.  Run the following command:

    ```sh
    rtiroutingservice -cfgFile file_bridge.xml -cfgName file_to_dds
    ```
