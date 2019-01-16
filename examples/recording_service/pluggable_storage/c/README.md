## Example Description

This example shows how to implement a custom `Recording Service Storage plug-in`,
build it into a shared library and load it with Recording Service.

In this concrete example, we show how a simple storage plug-in written in C. The 
example is composed of two parts: the writing side (that can be plugged into 
Recorder or Converter) and the reading side (that can be plugged into Replay or 
Converter - although only a Replay configuration file is supplied).

The example works with one single type called HelloMsg. The IDL for the type is
supplied. Any other topics or types won't be recorded or replayed. The purpose
of this example is to show how the Recorder and Replay APIs in C can be used to
plug-in custom storage needs. In this simple example, Recorder will store the
discovered samples in a file called C_PluggableStorage.dat and a companion file
called C_PluggableStorage.dat.info, by using the storage plug-in. The samples 
are stored in a textual format. These samples are later retrieved by Replay by 
using the reading plug-in.

The code in this directory provides the following components:

- `FileStorageWriter` implements the writer plugin that is loaded by *RTI Recorder*. 
   It is responsible for writing the received data samples to file.
- `FileStorageReader` implements the reader plugin that is loaded by *RTI Replay* 
   or *RTI Converter*. This component is responsible for reading data samples from the file.

For more details, please refer to the *RTI Recording Service API* documentation.

## Building the C example

In order to build this example, you need to provide the following variables to
`CMake`:

- `CONNEXTDDS_DIR`
- `CONNEXTDDS_ARCH`

```bash
mkdir build
cmake -DCONNEXTDDS_DIR=<connext dir> -DCONNEXTDDS_ARCH=<connext architecture> ..
cmake --build .
```

## Running the C example (storage writer)

To run the example, you just need to run the following command from the `build`
folder (where the storage writer plugin library has been created).

```bash
cd build
<connext dir>/bin/rtirecordingservice -cfgFile pluggable_storage_example.xml -cfgName C_StorageExample -domainIdBase <your domain ID>
```
After running this command, you will see the following output:

