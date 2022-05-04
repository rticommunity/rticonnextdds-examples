
# Request-Reply DynamicData Application

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
Running this Example

## Running the Example

This example is composed of two applications: a requester and a replier.

To run this example on a UNIX-based system, type the following in two
different command shells, either on the same machine or on different machines:

```sh
build/PrimeNumberReplier [<domain_id>]
build/PrimeNumberRequester <n> [<primes_per_reply>] [<domain_id>]
```

The Requester and the Replier are configured by the file USER_QOS_PROFILES.xml
located in this directory. You can modify this file to change the example's
behavior.

For more information on using Requesters and Repliers, please consult the
RTI Core Libraries and Utilities Getting Started Guide and User's Manual.
