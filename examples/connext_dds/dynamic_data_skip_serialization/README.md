# Example Code: Skip Data Serialization for Data Recording

## Concept

There are scenarios where the need to deserialize or inspect data is not necessary.
A common example is recording data. In this case, the application can simply
record the binary data as it is received, and then replay it later. This provides
a significant performance improvement.

The DynamicData API provides a mode that allows sending or receiving data in its
CDR format, without serializing or deserializing it.

Note that while Connext provides a Recording Service, there may be situations
where a custom Recording application may be useful.

## Example Description

This example implements a simple recording application that uses the DynamicData
API to receive data in CDR format and directly record it in a file. The example
also provides an replay option that reads the data buffers from the file
and publishes them back. For convenience, an option to publish a few samples
to test the record and replay functionality is also provided.

The key parts of the example are implemented in the ``record()`` and
``replay()`` functions in the example source code for each language.

The example is very simple and uses a single type and topic, but it could be
extended to use discovered types (see the
[built-in topics example](../builtin_topics/)) to implement a more
general-purpose recording application.
