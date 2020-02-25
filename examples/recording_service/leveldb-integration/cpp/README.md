# Recording Service -- Integration with LevelDB

## Example Description

This example is an implementation of the pluggable *RTI Recording Service* 
storage, using the no-SQL, persistent key-value store, LevelDB. This engine was
developed by Google as a fast key-value storage library that maps from key 
strings to string values. The LevelDB code can be found on [GitHub](https://github.com/google/leveldb).

This example is written in modern C++ (C++11) and shows how integration with a
no-SQL storage technology can be done in *RTI Recording Service*. The example
implements both the Storage Writer and Storage Reader plug-ins, so it can be
used to record, replay and/or convert the data coming from the Connext DDS data
bus. However, take into account that LevelDB is not meant to be accessed by two
different processes at the same time, and hence recording and replaying (or
converting) concurrently is _not supported_. In general, concurrently accessing
the LevelDB database being recorded, replayed or converted is not supported in 
this example. Note that this is not the case with our built-in SQLite 
implementation, although concurrent access in that case is also discouraged.

This example will store any DDS Topic discovered by *RTI Recording Service* that
has been marked as to be recorded (by defining the Topics and Topic Groups in 
the XML configuration).

The example provides the following files:

- `LevelDbWriter.hpp` and `LevelDbWriter.cxx` implement the Storage Writer
  API in *RTI Recording Service* and hence provides the ability for *Recorder*
  to store samples to disk, or to *Converter* to convert data _to_ LevelDB storage
  (coming from a different format or storage).
- `LevelDbReader.hpp` and `LevelDbReader.cxx` implement the Storage Reader 
  API in *RTI Recording Service* and hence provides the ability for *Replay* to
  read samples from a LevelDB database and publish them to DDS, or to *Converter*
  to convert data _from_ LevelDB storage to a different format or storage).
- `Tests.cxx` implements a test application that can be built and run on your
  system to verify the correct working of the provided Storage Writer and 
  Storage Reader classes.
- `Utils.hpp` and `Utils.cxx` implement some helper functions and, more
  importantly, the `UserKeyComparator` class that is used to customise the
  sample ordering in LevelDB. Because *Recorder* and *Replay* work based on time
  (they can be though about as time series storage) we need to make sure that
  samples are ordered in reception timestamp order, ascending and monotonic.
- `leveldb_recorder.xml`, `leveldb_replay.xml` and `leveldb_converter.xml`
  provide XML configurations for *Recorder*, *Replay* and *Converter*,
  respectively. The converter configuration file contains two configurations,
  one to convert to built-in SQLite CDR (serialized) format and another one to
  convert to built-in SQLite JSON format. All configurations are set up to 
  record, replay or convert all topics discovered by the application.

For more details on how to implement custom plugins, please refer to the *RTI
Recording Service API* documentation.

## Getting LevelDB and building it

The LevelDB database engine can be obtained from GitHub (this [link](https://github.com/google/leveldb). The repository can be cloned and there are 
instructions in the documentation to build the library with `CMake`.

> **Note**:
> Note: for this example, *RTI Recording Service* requires the LevelDB library
> to be linked _dynamically_. Make sure that you specify the `BUILD_SHARED_LIBS`
> when calling `CMake` to build LevelDB.

## Building the example

This example is designed to be built with `CMake`. In order to build it, you 
need to provide the following variables to `CMake` when creating the build
system:

- `CONNEXTDDS_DIR`: it should point to the installation of *RTI Connext DDS* 
  to be used for the build.
- `CONNEXTDDS_ARCH`: the *RTI Connext DDS* Target architecture to be used in
  your system.
- `CMAKE_BUILD_TYPE`: specifies the build mode. Valid values are Release
  and Debug. The general recommendation is to build in release mode. Mixing a
  release version *RTI Recording Service* executable with debug libraries may
  have unintended consequences when running the application.
- `LEVELDB_DIR`: it should point to where the LevelDB *dynamic* library is
  located.
- `LEVELDB_INCLUDE`: it should point to where the LevelDB API header files are
  located. This will normally be the `include` directory inside the directory
  where you cloned or extracted the LevelDB source code.
- `BUILD_SHARED_LIBS`: required. The shared libraries flag needs to be on for
  this example, as *RTI Recording Service* works by loading the libraries
  dynamically.

Build the example code by running the following command:

```bash
mkdir build
cd build
cmake -DCONNEXTDDS_DIR=<connext directory> 
      -DCONNEXTDDS_ARCH=<connext architecture>
      -DCMAKE_BUILD_TYPE=Release 
      -DBUILD_SHARED_LIBS=ON
      -DLEVELDB_DIR=<leveldb directory>
      -DLEVELDB_INCLUDE=<leveldb include directory>
      ..
cmake --build . --config Release
```

> **Note**:
>
> When using a multi-configuration generator, make sure you specify
> the `--config` parameter in your call to `cmake --build .`. In general,
> it's a good practice to always provide it.

In case you are using Windows x64, you have to add the option -A in the cmake
command as follows:

```bash
cmake -DCONNEXTDDS_DIR=<connext dir> 
      -DCONNEXTDDS_ARCH=<connext architecture>
      -DCMAKE_BUILD_TYPE=Release 
      -DBUILD_SHARED_LIBS=ON
      -DLEVELDB_DIR=<leveldb directory>
      -DLEVELDB_INCLUDE=<leveldb include directory>
      -A x64
```

**Cross-compilation**.

When you need to cross-compile the example, the above command will not work, the
assigned compiler won't be the cross-compiler and errors may happen when linking
against the cross-compiled Connext binaries. To fix this, you have to create a
file with the architecture name and call CMake with a specific flag called
``-DCMAKE_TOOLCHAIN_FILE``. An example of the file to create with the toolchain
settings (e.g. for an ARM architecture):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(toolchain_path "<path to>/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian")
set(CMAKE_C_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${toolchain_path}/bin/arm-linux-gnueabihf-g++")
```

Then you can call CMake like this:

```bash
cmake -DCONNEXTDDS_DIR=<connext dir> 
      -DCMAKE_TOOLCHAIN_FILE=<toolchain file created above>
      -DCONNEXTDDS_ARCH=<connext architecture>
      -DCMAKE_BUILD_TYPE=Release 
      -DLEVELDB_DIR=<leveldb directory> 
      -DLEVELDB_INCLUDE=<leveldb include directory> 
      ..
```

## Running *Recorder*

First of all, you need to make sure that the `LevelDbWriter` shared library
is in the system's library path.  On Mac OS X, you must use the variable
`RTI_LD_LIBRARY_PATH` instead of the common `DYLD_LIBRARY_PATH`.

This example will work with any DDS Topic discovered on the DDS Domain ID of 
your choice. Run any application publishing data into that domain, for example
*RTI Shapes Demo* or *RTI DDS Ping*.

*RTI Recording Service* expects you to set an environment variable called 
`LEVELDB_WORKING_DIR`. This variable should point to a directory with write
permissions and where all the LevelDB directories and files will be created.

To run the example, you need to run the following command from the `build`
folder (where the storage writer plugin shared library has been created).

```bash
export LEVELDB_WORKING_DIR=<storage location>
cd build
<connext dir>/bin/rtirecordingservice 
        -cfgFile ../leveldb_recorder.xml
        -cfgName LevelDbIntegration 
        -domainIdBase <your chosen domain ID>
```

Of course, the export command has to be translated to your current OS and
shell. After running this command, you should see the following output:

```bash
RTI Recording Service (Recorder) 6.0.1 starting...
RTI Recording Service started
```

The following directories will be created by *Recorder* after creating the
Storage Writer instance:
- `metadata.dat` database directory. This database will contain two key-value 
  entries. They represent the nanoseconds timestamps where the LevelDB Storage 
  Writer was created (started) and destroyed (stopped).
  *Replay* will request these timestamps to know the time span of the database
  and perform its calculations.
- `DCPSPublication.dat` database directory. This database will store the
  publication discovery samples received by *Recorder*. Only a reduced set of
  all the fields in the `DCPSPublication` data type will be stored, just the
  necessary ones for the Storage Reader to identify topics and types being
  discovered.
- For every discovered and matched (allowed by the XML configuration) DDS Topic,
  *Recorder* will create a database whose name will be defined as 
  `<topic-name>@<domain-id>`. For example, if topic `Square` is discovered
  on domain 54 (this is the Domain ID in the XML configuration, not the one
  achieved by offsetting with the `-domainIdBase` command-line parameter), 
  then *Recorder* will create database (directory) `Square@54` and will store
  all Square samples received in that domain in that database.

## Format of the stored data

The format of the data stored by *Recorder* into the LevelDB databases is 
defined in the `LevelDb_RecorderTypes.idl` file. As LevelDB is a key-value
storage system, we need to define the key type. The key type should allow us to:

- Uniquely identify a DDS sample. In DDS, this can be done by using the 
  *original writer virtual GUID* and the *original virtual sequence number* of
  the sample.
- Quickly access the time the sample was received, represented by the sample's
  reception timestamp.
  
This is reflected in type `UserDataKey` that can be found in the IDL file,
defined as:

```idl
struct UserDataKey {
    int64 reception_timestamp;
    octet original_writer_v_guid[GUID_LENGTH];
    SequenceNumber original_v_seq_nr;
};
```

The `UserDataKey` type is used to store keys for both publication data and user
data samples.

The LevelDB library allows for the customization of the key ordering mechanism.
This example implements a key comparator class that extends LevelDB's 
`Comparator` interface. This implementation achieves ordering of samples in a
strictly time-based manner. Resulting databases order their samples in an
ascending, monotinic order based on the reception timestamp. See class
`UserDataKeyComparator` in files `Utils.hpp` and `Utils.cxx`.

### Publication Data

The `DCPSPublication` samples received by *Recorder* will be stored in the
publication database mentioned in the above paragraph. However, for performance
and simplicity reasons, not all the fields in the `DCPSPublication` type will
be stored. The following IDL type describes what information is serialized and
stored in the database:

```idl
struct ReducedDCPSPublication {
    boolean valid_data;
    string<256> topic_name;
    string<256> type_name;
    sequence<octet> type;
};
```

### User Data

The user-data samples received by *Recorder* will be stored in their own LevelDB
database. While we will serialize to CDR and store the whole data sample, there
is no need to store all the Sample Info fields alongside with this data. In fact,
for simplicity and performance reasons, we will only store the valid data flag
along with the user data sample. The following IDL type will be used as the value
(and the aforementioned `UserDataKey` type will be used for the key):

```idl
struct UserDataValue {
    boolean valid_data;
    sequence<char> data_blob;
};
```

## Running *Replay* or *Converter* to access the data

After Recorder has already recorded a database in the working directory provided
by the user, data can be accessed by using the Storage Reader side of this
implementation.

> **Note**:
>
> As mentioned above, both LevelDB and this example are _not prepared_ to have
> *Replay*, *Converter* or any other process access the database while 
> *Recorder* is still running. In general, only one process should access the
> database at a time.
> This is not the case with the built-in SQLite implementation available in
> *RTI Recording Service*, where it is possible to run *Replay* and/or
> *Converter* while *Recorder* is still recording.

To run *Replay* or *Converter*, the `LevelDbReader` shared library should be
accessible to the application, by setting up the environment correctly
(adding the path to the system's library lookup path). On Mac OS X, you must 
use the variable `RTI_LD_LIBRARY_PATH` instead of the common 
`DYLD_LIBRARY_PATH`.

If you recorded one of `Connext DDS`'s demo topics, like Shapes Demo or
RTI DDS Ping, you can run them in subscribing mode now to see the samples
coming from *Replay*. If you're running *Converter*, you can check the
generated SQLite database with the SQLite command-line application or
the SQLite viewer application of your choice.

*RTI Recording Service* expects you to set an environment variable called 
`LEVELDB_WORKING_DIR`. This variable should point to a directory with write
permissions and where all the LevelDB directories and files will be created.

To run *Replay*, you just need to run the following command from the `build`
folder (where the storage reader plugin shared library has been created).

```bash
export LEVELDB_WORKING_DIR=<storage location>
cd build
<connext dir>/bin/rtireplayservice 
        -cfgFile ../leveldb_replay.xml
        -cfgName LevelDbIntegration 
        -domainIdBase <your chosen domain ID>
```

You should see the samples in the file being published and received by the
subscribing application.

The supplied XML *Converter* configuration contains two configurations:
one to convert to SQLite CDR format (`LevelDb_To_SQLiteCDR`), and another 
one to convert to SQLite JSON format (`LevelDb_To_SQLiteJSON`). To run
*Converter*, run the following command:

```bash
export LEVELDB_WORKING_DIR=<storage location>
cd build
<connext dir>/bin/rticonverter 
        -cfgFile ../leveldb_converter.xml
        -cfgName LevelDb_To_SQLiteCDR
```
