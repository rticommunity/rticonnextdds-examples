# Example Code: Deadlines and Content Filtering

## Building the Example :wrench:

To build this example, first run CMake to generate the corresponding build
files. We recommend you use a separate directory to store all the generated
files (e.g., ./build).

```sh
mkdir build
cd build
cmake ..
```

Once you have run CMake, you will find a number of new files in your build
directory (the list of generated files will depend on the specific CMake
Generator). To build the example, run CMake as follows:

```sh
cmake --build .
```

**Note**: if you are using a multi-configuration generator, such as Visual
Studio solutions, you can specify the configuration mode to build as follows:

```sh
cmake --build . --config Release|Debug
```

Alternatively, you can use directly the generated infrastructure (e.g.,
Makefiles or Visual Studio Solutions) to build the example. If you generated
Makefiles in the configuration process, run make to build the example. Likewise,
if you generated a Visual Studio solution, open the solution and follow the
regular build process.

## Running the Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

On *Windows* systems run:

```sh
deadline_contentfilter_publisher.exe  -d <domain_id> -s <samples_to_send>
deadline_contentfilter_subscriber.exe -d <domain_id> -s <samples_to_receive>
```

On *UNIX* systems run:

```sh
./deadline_contentfilter_publisher  -d <domain_id> -s <samples_to_send>
./deadline_contentfilter_subscriber -d <domain_id> -s <samples_to_send>
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples. The default is
    infinite.

## Publisher Output

```plaintext
Writing instance0, x = 0, y = 0
Writing instance1, x = 0, y = 0
Writing instance0, x = 1, y = 1
Writing instance1, x = 1, y = 1
Writing instance0, x = 2, y = 2
Writing instance1, x = 2, y = 2
Writing instance0, x = 3, y = 3
Writing instance1, x = 3, y = 3
Writing instance0, x = 4, y = 4
Writing instance1, x = 4, y = 4
Writing instance0, x = 5, y = 5
Writing instance1, x = 5, y = 5
Writing instance0, x = 6, y = 6
Writing instance1, x = 6, y = 6
Writing instance0, x = 7, y = 7
Writing instance1, x = 7, y = 7
Writing instance0, x = 8, y = 8
Writing instance1, x = 8, y = 8
Writing instance0, x = 9, y = 9
Writing instance1, x = 9, y = 9
Writing instance0, x = 10, y = 10
Writing instance1, x = 10, y = 10
Writing instance0, x = 11, y = 11
Writing instance1, x = 11, y = 11
Writing instance0, x = 12, y = 12
Writing instance1, x = 12, y = 12
Writing instance0, x = 13, y = 13
Writing instance1, x = 13, y = 13
Writing instance0, x = 14, y = 14
Writing instance1, x = 14, y = 14
Writing instance0, x = 15, y = 15
Stopping writes to instance1
Offered deadline missed on instance code = 1
Writing instance0, x = 16, y = 16
Writing instance0, x = 17, y = 17
Offered deadline missed on instance code = 1
Writing instance0, x = 18, y = 18
Offered deadline missed on instance code = 1
Writing instance0, x = 19, y = 19
Offered deadline missed on instance code = 1
Writing instance0, x = 20, y = 20
Writing instance0, x = 21, y = 21
Offered deadline missed on instance code = 1
Writing instance0, x = 22, y = 22
Offered deadline missed on instance code = 1
Writing instance0, x = 23, y = 23
Writing instance0, x = 24, y = 24
Offered deadline missed on instance code = 1
Writing instance0, x = 25, y = 25
```

## Subscriber Output

```plaintext
@ t=0.039139s, Instance0: <0,0>
@ t=0.039220s, Instance1: <0,0>
@ t=0.040054s, Instance0: <1,1>
@ t=0.040110s, Instance1: <1,1>
@ t=0.041008s, Instance0: <2,2>
@ t=0.041068s, Instance1: <2,2>
@ t=0.042381s, Instance0: <3,3>
@ t=0.042381s, Instance1: <3,3>
@ t=0.043977s, Instance0: <4,4>
@ t=0.044066s, Instance1: <4,4>
@ t=0.045628s, Instance0: <5,5>
@ t=0.045695s, Instance1: <5,5>
@ t=0.047707s, Instance0: <6,6>
@ t=0.047764s, Instance1: <6,6>
@ t=0.047989s, Instance0: <7,7>
@ t=0.048066s, Instance1: <7,7>
@ t=0.048592s, Instance0: <8,8>
@ t=0.048649s, Instance1: <8,8>
@ t=0.049307s, Instance0: <9,9>
@ t=0.049374s, Instance1: <9,9>
Starting to filter out instance1
@ t=0.050380s, Instance0: <10,10>
Missed deadline @ t=0.050681s on instance code = 1
@ t=0.050792s, Instance0: <11,11>
@ t=0.051841s, Instance0: <12,12>
Missed deadline @ t=0.052744s on instance code = 1
@ t=0.052918s, Instance0: <13,13>
@ t=0.053325s, Instance0: <14,14>
Missed deadline @ t=0.053606s on instance code = 1
@ t=0.053788s, Instance0: <15,15>
@ t=0.054064s, Instance0: <16,16>
Missed deadline @ t=0.054288s on instance code = 1
@ t=0.054485s, Instance0: <17,17>
@ t=0.054977s, Instance0: <18,18>
Missed deadline @ t=0.055311s on instance code = 1
@ t=0.055489s, Instance0: <19,19>
@ t=0.056071s, Instance0: <20,20>
Missed deadline @ t=0.056533s on instance code = 1
@ t=0.056836s, Instance0: <21,21>
@ t=0.057382s, Instance0: <22,22>
Missed deadline @ t=0.057665s on instance code = 1
@ t=0.057850s, Instance0: <23,23>
@ t=0.058550s, Instance0: <24,24>
Missed deadline @ t=0.058816s on instance code = 1
@ t=0.059073s, Instance0: <25,25>
```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify the
default behavior:

-   `-DCMAKE_BUILD_TYPE` -- specifies the build mode. Valid values are Release
    and Debug. See the [CMake documentation for more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

-   `-DBUILD_SHARED_LIBS` -- specifies the link mode. Valid values are ON for
    dynamic linking and OFF for static linking. See [CMake documentation for
    more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

-   `-G` -- CMake generator. The generator is the native build system to use
    build the source code. All the valid values are described described in the
    CMake documentation [CMake Generators
    Section.](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

For example, to build a example in Debug/Static mode run CMake as follows:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON .. -G "Visual Studio 15 2017" -A x64
```

### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform.If you installed Connext DDS in a custom location, you
can use the CONNEXTDDS_DIR variable to indicate the path to your RTI Connext DDS
installation folder. For example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, If you installed libraries for multiple target architecture on your system
(i.e., you installed more than one target rtipkg), you can use the
CONNEXTDDS_ARCH variable to indicate the architecture of the specific libraries
you want to link against. For example:

```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```

### CMake Build Infrastructure

The CMakeListst.txt script that builds this example uses a generic CMake
function called connextdds_add_example that defines all the necessary constructs
to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Copy the USER_QOS_PROFILES.xml file into the directory where the publisher
    and subscriber executables are generated.

You will find the definition of connextdds_add_example, along with detailed
documentation, in
[resources/cmake/ConnextDdsAddExample.cmake](../../../../resources/cmake/ConnextDdsAddExample.cmake).

For a more comprehensive example on how to build an RTI Connext DDS application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which includes
a comprehensive CMakeLists.txt script with all the steps and instructions
described in detail.
