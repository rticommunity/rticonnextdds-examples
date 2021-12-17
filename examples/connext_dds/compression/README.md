# Example Code: Data Compression

## Concept

The compression feature allows compressing the *USER_DATA* samples sent by a
*DataWriter*.
Compression can be configured by 3 QoSs (See the ConnextDDS_CoreLibraries
user-manual for more information):

- `compression_ids.`
- `writer_compression_threshold`
- `writer_compression_level`

## Example description

This example shows how to use compression. You will learn how to:

-   Configure the compression feature using Qos
-   How the bandwidth savings is affected by each compression algorithm.
Configurable by the command line parameter `--compression-id`.

This test measures the number of bytes received at the UDP transport-level on
the subscriber side, this way we can compare how much bandwidth has been saved
by enabling/disabling compression and the different compression algorithms.

The *USER_DATA* can be provided using the `--input-file` command line parameter,
where each of the lines in the file represents a sample. This is a simple
way to test and compare different kinds of data.

## Publisher

The publisher creates a list of samples from the input file (or 1K samples
filled with zeros if no file is provided). Then it send all the samples in a
loop until it reaches the sample count (--sample-count). The selected
compression algorithm (--compression-id) applies to each of the samples.

A generic text file has been added along with this example, you can find it
under `resource/messages/text_payload.txt`.

Execute the publisher with the `--help` command line parameter for more
information.

Example of a publisher execution:

```shell
compression_publisher -c ZLIB -i ../../../../../resources/messages/text_payload.txt -s 100
```

## Subscriber

The subscriber is based on the default compression configuration (which
matches a DataWriter with compression enabled by default). This subscriber will
receive the compressed samples and decompress them automatically. A final report
will be printed showing the number of samples received and how many of them have
been compressed (Check the `writer_compression_threshold` section on the
ConnextDDS user manual for more information about when a sample is eligible to
be compressed). The number of bytes received at the UDP transport level is also
printed so that it is easy to compare how compression affects the bandwidth
usage for the given input data.

To finalize the subscriber, either set the `--sample-count` option or send
a SIGINT/SIGTERM signal. On shutdown, the application will print the
final report and finish gracefully.

## Code generation

The current limit for each line of the input file is 4096 bytes long. If you
want to change this value you need to edit the CMakeLists file
`examples/connext_dds/compression/c++/CMakeLists.txt` and modify the value
under `CODEGEN_ARGS`
