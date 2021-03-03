# Example Code: Data Compression

## Concept

The compression feature allows compressing the *USER_DATA* samples sent by a
*DataWriter*.
Compression can be configured by 3 QoSs (Check the ConnextDDS_CoreLibraries
user-manual for more information):

- compression_ids.
- writer_compression_threshold.
- writer_compression_level

## Example description

This example shows how to use compression. You will learn how to:

-   Configure the compression feature by Qos
-   How the Bandwidth saving is affected by each compression algorithm.
Configurable by the command line parameter `--compression-id`.

This test measures the number of bytes received at UDP transport-level on
the subscriber side, this way we can compare how much bandwidth has been saved
by enabling/disabling compression and the different compression algorithm.

The *USER_DATA* can be also configured by `--input-file` command line parameter,
where each of the lines on the files will represent a sample. This is a simple
way to test and compare different kinds of data.

## Publisher

The publisher will create a list of samples from the input file (or 1K samples
fill with zeros if no file is provided). Then will send all the samples on a
loop until hits the sample count (--sample-count). The selected compression
(--compression-id) will apply to each of the samples.

A generic text file has been added along with this examples, you can find it
under `resource/messages/text_payload.txt`.

Execute the publisher with `--help` command line parameter for more information.

Example of a publisher execution:

```shell
compression_publisher -c ZLIB -i ../../../../../resources/messages/text_payload.txt -s 100
```

## Subscriber

The subscriber is base on default configuration (which will match a compressed
DataWriter by default), will receive the compressed samples and decompress them
automatically. A final report will be print showing the number of samples
received and how many of them have been compressed (Check the
`writer_compression_threshold` section on the ConnextDDS user manual for more
information about when a sample is eligible or not to be compressed), also the
amount of bytes received at UDP transport level will be printed, with this we
can compare how compression affects the bandwidth usage over the given input
data.
To finalize the subscriber either set the `--sample-count` option or send
a SIGINT/SIGTERM signal that will be handler by the application printing the
final report and finishing gratefully.

## Code generation

The current limit for each line of the input file is 4096 bytes long. If you
want to change this value you need to edit the CMakeLists file
`examples/connext_dds/compression/c++/CMakeLists.txt` and modify the value
under `CODEGEN_ARGS`
